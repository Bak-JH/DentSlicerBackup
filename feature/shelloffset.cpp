#include "shelloffset.h"
#include "../qml/components/Inputs.h"
#include "../qml/components/ControlOwner.h"
#include "feature/repair/meshrepair.h"
#include "Mesh/mesh.h"
#include "cut/ZAxialCut.h"
#include "deleteModel.h"
#include "application/ApplicationManager.h"

#include <pcl/PCLPointCloud2.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/surface/marching_cubes_hoppe.h>
#include <pcl/geometry/triangle_mesh.h>
#include <pcl/geometry/mesh_conversion.h>
#include <pcl/surface/poisson.h>
#include <pcl/keypoints/uniform_sampling.h>

#include <algorithm>
#include <execution>

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

void generateVertexBuffer(float xOffset, float yOffset, bool xInverted, std::vector<float>& buffer, const Hix::Engine3D::Mesh& mesh, Hix::Engine3D::Bounds3D& bound)
{
	auto& faces = mesh.getFaces();
	if (xInverted)
	{
		for (auto itr = faces.cbegin(); itr != faces.cend(); ++itr)
		{

			auto mvs = itr.meshVertices();

			for (int j = 2; j >= 0; --j)
			{
				auto mv = mvs[j].worldPosition();
				QVector3D pt{ -mv.x() + xOffset, mv.y() + yOffset, mv.z() };
				bound.update(pt);
				buffer.emplace_back(pt.x());
				buffer.emplace_back(pt.y());
				buffer.emplace_back(pt.z());
			}
		}
	}
	else
	{
		for (auto itr = faces.cbegin(); itr != faces.cend(); ++itr)
		{

			auto mvs = itr.meshVertices();
			for (int j = 0; j < 3; ++j)
			{
				auto pt = mvs[j].worldPosition();
				bound.update(pt);
				buffer.emplace_back(pt.x() + xOffset);
				buffer.emplace_back(pt.y() + yOffset);
				buffer.emplace_back(pt.z());
			}
		}
	}
}

std::vector<float> toVtxBuffer(Hix::Engine3D::Bounds3D& bounds, Mesh& mesh)
{
	std::vector<float> vtcs;

	size_t vtxCnt = 0;

	vtxCnt += mesh.getVertices().size();

	auto& printerSetting = Hix::Application::ApplicationManager::getInstance().settings().printerSetting;
	auto& setting = Hix::Application::ApplicationManager::getInstance().settings().sliceSetting;

	;
	vtcs.reserve(vtxCnt);
	generateVertexBuffer(0,0,false, vtcs, mesh, bounds);

	return vtcs;
}

QVector3D getAbs(const QVector3D vec)
{
	return QVector3D(std::abs(vec.x()), std::abs(vec.y()), std::abs(vec.z()));
}

float getDistance(const QVector3D p1, const QVector3D p2)
{
	return std::powf((std::powf((p2.x() - p1.x()), 2.0f) + std::powf((p2.y() - p1.y()), 2.0f) + std::powf((p2.z() - p1.z()), 2.0f)), 1 / 2);
}


void Hix::Features::HollowMesh::uniformSampling(float offset)
{
	Hix::Engine3D::Bounds3D aabb = _target->aabb();

	QVector3D center = aabb.minPt() - aabb.maxPt();
	float distance = toNorm(center);

	QVector3D diagonal = QVector3D(distance / 10.0f + std::abs(offset), 
								   distance / 10.0f + std::abs(offset), 
								   distance / 10.0f + std::abs(offset));

	Hix::Engine3D::Mesh* offsetMesh = _target->getMeshModd();

	QVector3D offsetMinPt = aabb.minPt() - diagonal;
	QVector3D offsetMaxPt = aabb.maxPt() + diagonal;

	aabb.setMinPt(offsetMinPt);
	aabb.setMaxPt(offsetMaxPt);

	QVector3D box_size = aabb.maxPt() - aabb.minPt();
	float cellSize = distance / 50.0f;

	QVector3D volumeDim;
	BestDim(box_size, cellSize, volumeDim);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="offset"></param>
	float default_iso_level = 0.1f;
	float default_extend_percentage = 10.0f;
	int default_grid_res = 100;
	float default_off_surface_displacement = 0.01f;
	pcl::PointCloud<pcl::PointNormal> cloud;

	auto* mesh = _target->getMesh();
	auto vtxCend = mesh->getVertices().cend();

	pcl::PointCloud<pcl::PointXYZ>::Ptr cloudPt(new pcl::PointCloud<pcl::PointXYZ>);
	for (auto vtxItr : mesh->getVertices())
	{
		pcl::PointXYZ pclPt = pcl::PointXYZ(vtxItr.position.x(), vtxItr.position.y(), vtxItr.position.z());
		cloudPt->push_back(pclPt);
	}

	pcl::NormalEstimation<pcl::PointXYZ, pcl::PointNormal> normalEstimation;
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>);

	pcl::UniformSampling<pcl::PointXYZ> filter;
	filter.setInputCloud(cloudPt);     // 입력 
	filter.setRadiusSearch(1.0F);   // 탐색 범위 0.01F
	filter.filter(*cloud_filtered);  // 필터 적용 

	pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>);
	tree->setInputCloud(cloud_filtered);
	normalEstimation.setInputCloud(cloud_filtered);
	normalEstimation.setSearchMethod(tree);
	normalEstimation.setKSearch(20);
	normalEstimation.compute(cloud);

	pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals(new pcl::PointCloud<pcl::PointNormal>);
	pcl::concatenateFields(*cloud_filtered, cloud, *cloud_with_normals);

	int index = 0;
	long temp_size = cloud_filtered->points.size();

	for (unsigned int i = 0; i < temp_size; i = i + 20) {
		// x range -35m to 35 m, y ranges -25 to 25m, z ranges 0 to 66
		auto pt = cloud_filtered->points[i];
		auto ptX = cloud_filtered->points[i].x;
		auto ptY = cloud_filtered->points[i].y;
		auto ptZ = cloud_filtered->points[i].z;
		cloud.points[index].x = cloud_filtered->points[i].x;
		cloud.points[index].y = cloud_filtered->points[i].y;
		cloud.points[index].z = cloud_filtered->points[i].z;
		index++;
	}







	const Mesh* originalMesh = _target->getMesh();
	Mesh* newMesh = new Mesh();
	std::vector<QVector3D> DF;
	auto& setting = Hix::Application::ApplicationManager::getInstance().settings().sliceSetting;

	auto xMin = -1280;
	auto xMax = 1280;
	auto yMin = -810;
	auto yMax = 810;
	auto zMin = _target->aabb().zMin();
	auto zMax = _target->aabb().zMax();

	for (auto x = xMin; x < xMax; ++x)
	{
		for (auto y = yMin; y < yMax; ++y)
		{
			for (auto z = zMin; z < zMax; ++z)
			{
				auto closest_distance = 9999.0f;
				QVector3D closest_point;

				for (auto face = originalMesh->getFaces().cbegin(); face != originalMesh->getFaces().cend(); ++face)
				{

					qDebug() << PtOnTri(QVector3D(x, y, z), face.meshVertices());
					auto temp_distance = getDistance(QVector3D(x, y, z), PtOnTri(QVector3D(x, y, z), face.meshVertices()));
					if (temp_distance < closest_distance)
					{
						qDebug() << temp_distance;
						closest_distance = temp_distance;
						closest_point = getAbs(QVector3D(x, y, z) - PtOnTri(QVector3D(x, y, z), face.meshVertices()));
					}
				}

				DF.push_back(closest_point);
			}
		}
	}


	auto tmpPath = std::filesystem::temp_directory_path() / "tmpSlice";
	//Hix::Slicer::SlicerGL slicer(setting.layerHeight, tmpPath, setting.AAXY, setting.AAZ, setting.minHeight);
	//slicer.setScreen(0.1f, 2560, 1620);
	

	std::vector<float> buffer;
	for (auto pt : DF)
	{

		buffer.emplace_back(pt.x());
		buffer.emplace_back(pt.y());
		buffer.emplace_back(pt.z());
	}


	auto t = originalMesh->getFaces().begin().meshVertices();
	std::filesystem::path file = tmpPath / "2.png";


	std::vector<float> tbuffer;
	tbuffer.push_back(1.0f);
	tbuffer.push_back(0.0f);
	tbuffer.push_back(0.0f);
	tbuffer.push_back(1.0f);
	tbuffer.push_back(0.0f);
	tbuffer.push_back(0.0f);
	tbuffer.push_back(1.0f);
	tbuffer.push_back(0.0f);
	tbuffer.push_back(0.0f);

	//stbi_write_png(file.c_str(), buffer.size(), y_cnt, 1, buffer.data(), x_cnt);



	//pcl::PointCloud<pcl::PointNormal> outPts;
	//std::vector<pcl::Vertices> outPoly;
	//pcl::PolygonMesh output;

	//pcl::MarchingCubesHoppe<pcl::PointNormal> mc;
	//mc.setIsoLevel(default_iso_level);
	//mc.setGridResolution(default_grid_res, default_grid_res, default_grid_res);
	//mc.setPercentageExtendGrid(default_extend_percentage);
	//mc.setInputCloud(cloud_with_normals);
	//mc.reconstruct(outPts, outPoly);

	////add back to mesh;
	////polygon should be triangular due to the nature of marching cubes
	//using trianglemesh = pcl::geometry::TriangleMesh<MeshTraits<true>>;
	//trianglemesh trimesh;
	//pcl::geometry::toFaceVertexMesh(trimesh, output);
	//auto& facedata = trimesh.getFaceDataCloud();

	//for (auto& tri : outPoly)
	//{
	//	if (tri.vertices.size() != 3)
	//	{
	//		throw std::runtime_error("output mesh from marching cubes is not triangular");
	//	}
	//	newMesh->addFace(
	//		pclToQtPt(outPts[tri.vertices[0]]),
	//		pclToQtPt(outPts[tri.vertices[1]]),
	//		pclToQtPt(outPts[tri.vertices[2]])
	//	);
	//}
	//_target->setMesh(newMesh);
}