#include "shelloffset.h"
#include "../qml/components/Inputs.h"
#include "../qml/components/ControlOwner.h"
#include "feature/repair/meshrepair.h"
#include "Mesh/mesh.h"
#include "cut/ZAxialCut.h"
#include "deleteModel.h"
#include "application/ApplicationManager.h"


#include <QString>

#include <pcl/PCLPointCloud2.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/surface/marching_cubes_hoppe.h>
#include <pcl/geometry/triangle_mesh.h>
#include <pcl/geometry/mesh_conversion.h>
#include <pcl/surface/poisson.h>
#include <pcl/keypoints/uniform_sampling.h>

#include <algorithm>
#include <execution>
#include <limits>
#include <chrono>

#include "sampling.h"
#include "slice/gpuSlicer.h"




#define STBI_WINDOWS_UTF16
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "slice/include/stb_image_write.h"

// offset shell with mm

using namespace Hix::Engine3D;
using namespace Hix::Features;
using namespace Hix::Features::Cut;
const QUrl OFFSET_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupShellOffset.qml");


const float toNorm(QVector3D pt)
{
	return std::sqrt(pt.x() * pt.x() +
					pt.y() * pt.y() +
					pt.z() * pt.z());
}

void BestDim(const int64_t elems, const QVector3D size, QVector3D& dim)
{
	const __int64 mincells = 1;		// Numero minimo di celle
	const double GFactor = 1;	// GridEntry = NumElem*GFactor
	double diag = toNorm(size);	// Diagonale del box
	double eps = diag * 1e-4;		// Fattore di tolleranza

	assert(elems > 0);
	assert(size[0] >= 0.0);
	assert(size[1] >= 0.0);
	assert(size[2] >= 0.0);


	__int64 ncell = (__int64)(elems * GFactor);	// Calcolo numero di voxel
	if (ncell < mincells)
		ncell = mincells;

	dim[0] = 1;
	dim[1] = 1;
	dim[2] = 1;

	if (size[0] > eps)
	{
		if (size[1] > eps)
		{
			if (size[2] > eps)
			{
				double k = pow((double)(ncell / (size[0] * size[1] * size[2])), double(1.0 / 3.f));
				dim[0] = int(size[0] * k);
				dim[1] = int(size[1] * k);
				dim[2] = int(size[2] * k);
			}
			else
			{
				dim[0] = int(::sqrt(ncell * size[0] / size[1]));
				dim[1] = int(::sqrt(ncell * size[1] / size[0]));
			}
		}
		else
		{
			if (size[2] > eps)
			{
				dim[0] = int(::sqrt(ncell * size[0] / size[2]));
				dim[2] = int(::sqrt(ncell * size[2] / size[0]));
			}
			else
				dim[0] = int(ncell);
		}
	}
	else
	{
		if (size[1] > eps)
		{
			if (size[2] > eps)
			{
				dim[1] = int(::sqrt(ncell * size[1] / size[2]));
				dim[2] = int(::sqrt(ncell * size[2] / size[1]));
			}
			else
				dim[1] = int(ncell);
		}
		else if (size[2] > eps)
			dim[2] = int(ncell);
	}
	dim[0] = std::max(dim[0], 1.0f);
	dim[1] = std::max(dim[1], 1.0f);
	dim[2] = std::max(dim[2], 1.0f);
}

void BestDim(const QVector3D box_size, const float voxel_size, QVector3D& dim)
{
	__int64 elem_num = (__int64)(box_size.x() / voxel_size + 0.5) *
		(__int64)(box_size.y() / voxel_size + 0.5) *
		(__int64)(box_size.z() / voxel_size + 0.5);
	BestDim(elem_num, box_size, dim);
}






Hix::Features::ShellOffsetMode::ShellOffsetMode():DialogedMode(OFFSET_POPUP_URL)
{
	if (Hix::Application::ApplicationManager::getInstance().partManager().selectedModels().empty())
	{
		Hix::Application::ApplicationManager::getInstance().modalDialogManager().needToSelectModels();
		return;
	}
	auto& co = controlOwner();
	co.getControl(_offsetValue, "offsetValue");

	auto selectedModels = Hix::Application::ApplicationManager::getInstance().partManager().selectedModels();
	if (selectedModels.size() == 1)
	{
		_subject = *selectedModels.begin();
		_modelBound = _subject->recursiveAabb();
	}
	else
	{
		throw std::runtime_error("A single model must be selected");
	}
}

Hix::Features::ShellOffsetMode::~ShellOffsetMode()
{
}


void Hix::Features::ShellOffsetMode::applyButtonClicked()
{
	auto container = new Hix::Features::FeatureContainer();
	for (auto each : Hix::Application::ApplicationManager::getInstance().partManager().selectedModels())
	{
		container->addFeature(new ShellOffset(each, _offsetValue->getValue()));
	}
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(container);
}





Hix::Features::ShellOffset::ShellOffset(GLModel* target, float offset) : _target(target), _offset(offset)
{}

Hix::Features::ShellOffset::~ShellOffset()
{
}


void Hix::Features::ShellOffset::runImpl()
{
	addFeature(new HollowMesh(_target, _offset));
	FeatureContainer::runImpl();
}

Hix::Features::HollowMesh::HollowMesh(GLModel* target, float offset) : _target(target), _offset(offset)
{}

Hix::Features::HollowMesh::~HollowMesh()
{
}

void Hix::Features::HollowMesh::undoImpl()
{
	postUIthread([this]() {
		auto tmp = _target->getMeshModd();
		_target->setMesh(_prevMesh.release());
		_prevMesh.reset(tmp);
		});


}

void Hix::Features::HollowMesh::redoImpl()
{
	postUIthread([this]() {
		auto tmp = _target->getMeshModd();
		_target->setMesh(_prevMesh.release());
		_prevMesh.reset(tmp);
		});
}

void Hix::Features::HollowMesh::runImpl()
{
	Hix::Engine3D::Mesh* orig = _target->getMeshModd();
	_prevMesh.reset(orig);
	//_prevMesh->reverseFaces();
	auto hollowMesh = new Mesh(*orig);

	uniformSampling(_offset);

	////Mesh offsetMesh(*orig);
	//hollowMesh->vertexOffset(-_offset);
	////auto testeste = new Mesh(offsetMesh);

	//// 
	////offsetMesh.reverseFaces();
	////*hollowMesh += offsetMesh;


	//postUIthread([this, &hollowMesh]() {
	//	_target->setMesh(hollowMesh);
	//});

}

namespace MeshRepairPrivate
{
	Eigen::Matrix3f  calcCovarianceMatrix(const std::unordered_set<FaceConstItr>& faces);
	void calculateCurvature(pcl::PointNormal& output, const Eigen::Matrix3f& covarianceMat);
	void calculateNormalPoint(pcl::PointNormal& output, const VertexConstItr& meshVtx, pcl::PointXYZ offsetVtx);
}

void MeshRepairPrivate::calculateNormalPoint(pcl::PointNormal& output, const VertexConstItr& meshVtx, pcl::PointXYZ offsetVtx)
{
	//add position
	output.x = offsetVtx.x;
	output.y = offsetVtx.y;
	output.z = offsetVtx.z;

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
	auto convarianceMat = MeshRepairPrivate::calcCovarianceMatrix(connectedFaces);
	MeshRepairPrivate::calculateCurvature(output, convarianceMat);

}


void MeshRepairPrivate::calculateCurvature(pcl::PointNormal& output, const Eigen::Matrix3f& covarianceMat)
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

Eigen::Matrix3f  MeshRepairPrivate::calcCovarianceMatrix(const std::unordered_set<FaceConstItr>& faces)
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
		accu[0] += vtx.worldPosition().x() * vtx.worldPosition().x();
		accu[1] += vtx.worldPosition().x() * vtx.worldPosition().y();
		accu[2] += vtx.worldPosition().x() * vtx.worldPosition().z();
		accu[3] += vtx.worldPosition().y() * vtx.worldPosition().y();
		accu[4] += vtx.worldPosition().y() * vtx.worldPosition().z();
		accu[5] += vtx.worldPosition().z() * vtx.worldPosition().z();
		accu[6] += vtx.worldPosition().x();
		accu[7] += vtx.worldPosition().y();
		accu[8] += vtx.worldPosition().z();
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

template<class PointClass>
QVector3D pclToQtPt(const PointClass& input)
{
	QVector3D pt;
	pt.setX(input.x);
	pt.setY(input.y);
	pt.setZ(input.z);
	return pt;
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

QVector3D getAbs(const QVector3D vec)
{
	return QVector3D(std::abs(vec.x()), std::abs(vec.y()), std::abs(vec.z()));
}

float getDistanceSquared(const QVector3D p1, const QVector3D p2)
{
	return (p1 - p2).lengthSquared();
	//return p1.distanceToPoint(p2);
	//return std::powf((std::powf((p2.x() - p1.x()), 2.0f) + std::powf((p2.y() - p1.y()), 2.0f) + std::powf((p2.z() - p1.z()), 2.0f)), 1 / 2);
}


void Hix::Features::HollowMesh::uniformSampling(float offset)
{
	auto start = std::chrono::high_resolution_clock::now();

	Hix::Engine3D::Bounds3D aabb = _target->aabb();
	//aabb.localBoundUpdate(*_target->getMesh());
	//aabb = aabb.centred();

	const Mesh* originalMesh = _target->getMesh();
	Mesh* newMesh = new Mesh();
	std::vector<std::pair<float, float>> DF;

	//int xMin = aabb.xMin();
	//int xMax = aabb.xMax();
	//int yMin = aabb.yMin();
	//int yMax = aabb.yMax();
	//int zMin = aabb.zMin();
	//int zMax = aabb.zMax();

	constexpr auto half = 20;

	float xMin = -half;
	float xMax = half;
	float yMin = -half;
	float yMax = half;
	float zMin = aabb.zMin();
	float zMax = aabb.zMax();

	constexpr float resolution = 1.0f;

	qDebug() << "center: " << aabb.centre();

	//_rayCaster.reset(new MTRayCaster());
	_rayAccel.reset(new Hix::Engine3D::BVH(*_target, false));
	//_rayCaster->addAccelerator(_rayAccel.get());

	auto settingdone = std::chrono::high_resolution_clock::now();

	//for (auto z1 = zMin; z1 < zMax; z1 += resolution)
	//{
		for (auto y = yMin; y < yMax; y += resolution)
		{
			for (auto x = xMin; x < xMax; x += resolution)
			{
				auto loopstart = std::chrono::high_resolution_clock::now();

				auto z = (zMin + ((zMax - zMin) / 2));
				auto closestDistance = std::numeric_limits<float>::max();

				QVector3D closestPoint;
				Hix::Engine3D::FaceConstItr closestFace;

				QVector3D currPt = QVector3D(x,y,z);

				auto useRay = std::chrono::high_resolution_clock::now();

				//auto r = _rayAccel->getClosest(currPt);
				//if (r.size() == 0)
				//	qDebug() << "not found";
				auto faceloopStart = std::chrono::high_resolution_clock::now();

				for (auto face = _target->getMesh()->getFaces().begin(); face != _target->getMesh()->getFaces().end(); ++face)
				//for(auto& face : r)
				{
					auto tmpClosest = PtOnTri(currPt, face);
					auto tempDistance = getDistanceSquared(currPt, tmpClosest);
					if (tempDistance < closestDistance)
					{
						closestDistance = tempDistance;
						closestPoint = tmpClosest;
						closestFace = face;
					}
				}

				auto pushBack = std::chrono::high_resolution_clock::now();

				auto normalST = currPt - closestPoint;
				normalST.normalize();
				auto dot = QVector3D::dotProduct(normalST, closestFace.localFn());
				//qDebug() << "closest: " << closestPoint;
				//qDebug() << "normalize: " << normalST;
				//qDebug() << "localFn: " << closestFace.localFn();
				//qDebug() << "dot: " << dot;
				auto closestDistance_sign = dot > 0.0f ? 1.0f : -1.0f;
				DF.push_back(std::make_pair(closestDistance, closestDistance_sign));

				//qDebug() << (settingdone - start).count();
				//qDebug() << (useRay - loopstart).count();
				//qDebug() << (faceloopStart - useRay).count();
				//qDebug() << (pushBack - faceloopStart).count();
			}
		}
	//}
	auto loopend = std::chrono::high_resolution_clock::now();

	qDebug() << "end";


	auto tmpPath = std::filesystem::temp_directory_path() / "tmpSlice";
	//Hix::Slicer::SlicerGL slicer(setting.layerHeight, tmpPath, setting.AAXY, setting.AAZ, setting.minHeight);
	//slicer.setScreen(0.1f, 2560, 1620);

	std::filesystem::path file = tmpPath / "test.png";


	// mapping
	std::vector<uint8_t> mapped_DF;
	std::unordered_set<float> pixSet;
	std::unordered_set<uint8_t> pixSet_t;
	auto max_dist = std::max_element(DF.begin(), DF.end());
	int cnt = 0;
	for (auto dist : DF)
	{
		pixSet.insert(dist.first);
		//auto pixel = 122;
		//if (dist.second < 0.0)
		//	pixel = 255;

		//auto pixel = (uint8_t)((dist.first / max_dist->first) * dist.second * 127 + 127) ;
		auto pixel = (dist.first / max_dist->first) * 255;
		pixSet_t.insert(pixel);
		//qDebug() << pixel;
		//if (pixel == 255)
		//{
		//	
		//	pixel = 0;
		//}
		mapped_DF.push_back(pixel);
		++cnt;

	}
	qDebug() << pixSet.size();
	qDebug() << pixSet_t.size();

	uint8_t res_x = aabb.xMax() - aabb.xMin();
	uint8_t res_y = aabb.yMax() - aabb.yMin();

	//stbi_write_png(file.c_str(), res_x, res_y, 1, mapped_DF.data(), res_x);
	stbi_write_png(file.c_str(), half*2, half*2, 1, mapped_DF.data(), half*2);
}