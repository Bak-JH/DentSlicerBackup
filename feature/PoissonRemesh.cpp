#include "PoissonRemesh.h"
#include "../DentEngine/src/mesh.h"

//PCL
#include <pcl/PCLPointCloud2.h>
#include <pcl/point_types.h>
#include <pcl/surface/marching_cubes_rbf.h>
#include <pcl/surface/marching_cubes_hoppe.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/search/kdtree.h>
#include <pcl/pcl_macros.h>
#include <pcl/features/feature.h>
#include <pcl/geometry/triangle_mesh.h>
#include <pcl/geometry/mesh_conversion.h>
#include <pcl/surface/poisson.h>
#include <pcl/filters/passthrough.h>

using namespace Hix;
using namespace Hix::Engine3D;
using namespace Hix::Features;

namespace PoissonRemeshPrivate
{
	Eigen::Matrix3f  calcCovarianceMatrix(const std::unordered_set<FaceConstItr>& faces);
	void calculateCurvature(pcl::PointNormal& output, const Eigen::Matrix3f& covarianceMat);
	void calculateNormalPoint(pcl::PointNormal& output, const VertexConstItr& meshVtx);
}

void PoissonRemeshPrivate::calculateNormalPoint(pcl::PointNormal& output, const VertexConstItr& meshVtx)
{
	//add position
	output.x = meshVtx.localPosition().x();
	output.y = meshVtx.localPosition().y();
	output.z = meshVtx.localPosition().z();

	auto connectedFaces = meshVtx.connectedFaces();
	//calculate vtx normal
	QVector3D vn;
	for (auto& face : connectedFaces)
	{
		vn += face.localFn();
	}
	vn.normalize();
	output.normal_x = vn.x();
	output.normal_y = vn.y();
	output.normal_z = vn.z();
	//calculate curvature
	auto convarianceMat = PoissonRemeshPrivate::calcCovarianceMatrix(connectedFaces);
	PoissonRemeshPrivate::calculateCurvature(output, convarianceMat);

}


void PoissonRemeshPrivate::calculateCurvature(pcl::PointNormal& output, const Eigen::Matrix3f& covarianceMat)
{
	// Extract the smallest eigenvalue and its eigenvector
	EIGEN_ALIGN16 Eigen::Vector3f::Scalar eigen_value;
	EIGEN_ALIGN16 Eigen::Vector3f eigen_vector;
	pcl::eigen33(covarianceMat, eigen_value, eigen_vector);


	// Compute the curvature surface change
	float eig_sum = covarianceMat.coeff(0) + covarianceMat.coeff(4) + covarianceMat.coeff(8);
	if (eig_sum != 0)
		output.curvature = fabsf(eigen_value / eig_sum);
	else
		output.curvature = 0;
}

Eigen::Matrix3f  PoissonRemeshPrivate::calcCovarianceMatrix(const std::unordered_set<FaceConstItr>& faces)
{
	Eigen::Matrix3f covariance_matrix = Eigen::Matrix3f::Zero();

	// create the buffer on the stack which is much faster than using cloud[indices[i]] and centroid as a buffer
	Eigen::Matrix<float, 1, 9, Eigen::RowMajor> accu = Eigen::Matrix<float, 1, 9, Eigen::RowMajor>::Zero();
	std::size_t point_count;

	std::unordered_set<VertexConstItr> neigbors;
	for (auto& each : faces)
	{
		auto faceVertices = each.meshVertices();
		for (auto& vtx : faceVertices)
		{
			neigbors.emplace(vtx);
		}
	}
	point_count = neigbors.size();
	for (auto& vtx : neigbors)
	{
		accu[0] += vtx.localPosition().x() * vtx.localPosition().x();
		accu[1] += vtx.localPosition().x() * vtx.localPosition().y();
		accu[2] += vtx.localPosition().x() * vtx.localPosition().z();
		accu[3] += vtx.localPosition().y() * vtx.localPosition().y();
		accu[4] += vtx.localPosition().y() * vtx.localPosition().z();
		accu[5] += vtx.localPosition().z() * vtx.localPosition().z();
		accu[6] += vtx.localPosition().x();
		accu[7] += vtx.localPosition().y();
		accu[8] += vtx.localPosition().z();
	}



	accu /= static_cast<float> (point_count);

	covariance_matrix.coeffRef(0) = accu[0] - accu[6] * accu[6];
	covariance_matrix.coeffRef(1) = accu[1] - accu[6] * accu[7];
	covariance_matrix.coeffRef(2) = accu[2] - accu[6] * accu[8];
	covariance_matrix.coeffRef(4) = accu[3] - accu[7] * accu[7];
	covariance_matrix.coeffRef(5) = accu[4] - accu[7] * accu[8];
	covariance_matrix.coeffRef(8) = accu[5] - accu[8] * accu[8];
	covariance_matrix.coeffRef(3) = covariance_matrix.coeff(1);
	covariance_matrix.coeffRef(6) = covariance_matrix.coeff(2);
	covariance_matrix.coeffRef(7) = covariance_matrix.coeff(5);
	return covariance_matrix;
}

template <bool IsManifoldT>
struct MeshTraits
{
	using VertexData = pcl::PointXYZ;
	using HalfEdgeData = pcl::geometry::NoData;
	using EdgeData = pcl::geometry::NoData;
	using FaceData = pcl::geometry::NoData;
	using IsManifold = std::integral_constant <bool, IsManifoldT>;
};

template<class PointClass>
QVector3D pclToQtPt(const PointClass& input)
{
	QVector3D pt;
	pt.setX(input.x);
	pt.setY(input.y);
	pt.setZ(input.z);
	return pt;
}


Engine3D::Mesh* Hix::Features::PoissonRemesh::remesh(const Engine3D::Mesh& mesh)
{
	pcl::PointCloud<pcl::PointNormal>::Ptr cloud(new pcl::PointCloud<pcl::PointNormal>);

	//cloud->reserve(mesh->getVertices().size());
	auto vtxCend = mesh.getVertices().cend();

	pcl::PointCloud<pcl::PointNormal>::Ptr benchmark(new pcl::PointCloud<pcl::PointNormal>);
	{

		using namespace pcl;
		pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);

		PointCloud<PointXYZ>::Ptr ptCloud(new PointCloud<PointXYZ>);
		for (auto vtxItr = mesh.getVertices().cbegin(); vtxItr != vtxCend; ++vtxItr)
		{
			auto vtxNormal = vtxItr.localVn();
			auto vtxPos = vtxItr.localPosition();
			ptCloud->push_back(pcl::PointXYZ());
			ptCloud->back().x = vtxPos.x();
			ptCloud->back().y = vtxPos.y();
			ptCloud->back().z = vtxPos.z();
		}

		NormalEstimationOMP<PointXYZ, Normal> ne;
		search::KdTree<PointXYZ>::Ptr tree1(new search::KdTree<PointXYZ>);
		tree1->setInputCloud(ptCloud);
		ne.setInputCloud(ptCloud);
		ne.setNumberOfThreads(6);
		ne.setSearchMethod(tree1);
		ne.setKSearch(20);
		ne.compute(*normals);
		for (size_t i = 0; i < normals->size(); ++i) {
			normals->points[i].normal_x *= -1;
			normals->points[i].normal_y *= -1;
			normals->points[i].normal_z *= -1;
		}

		concatenateFields(*ptCloud, *normals, *benchmark);
	}
	for (auto vtxItr = mesh.getVertices().cbegin(); vtxItr != vtxCend; ++vtxItr)
	{
		cloud->push_back(pcl::PointNormal());
		PoissonRemeshPrivate::calculateNormalPoint(cloud->back(), vtxItr);
	}


	pcl::PointCloud<pcl::PointNormal> outPts;
	std::vector<pcl::Vertices> outPoly;

	pcl::Poisson<pcl::PointNormal> poisson;
	poisson.setDepth(11);
	poisson.setScale(1.1f);
	//poisson.setMinDepth(5);
	poisson.setInputCloud(cloud);
	poisson.reconstruct(outPts, outPoly);

	auto newMesh = new Hix::Engine3D::Mesh();
	for (auto& tri : outPoly)
	{
		if (tri.vertices.size() != 3)
		{
			throw std::runtime_error("output mesh from marching cubes is not triangular");
		}
		newMesh->addFace(
			pclToQtPt(outPts[tri.vertices[0]]),
			pclToQtPt(outPts[tri.vertices[1]]),
			pclToQtPt(outPts[tri.vertices[2]])
		);
	}
	return newMesh;
}
