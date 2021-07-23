#include "shelloffset.h"
#include "../qml/components/Inputs.h"
#include "../qml/components/ControlOwner.h"
#include "feature/repair/meshrepair.h"
#include "Mesh/mesh.h"
#include "cut/ZAxialCut.h"
#include "deleteModel.h"
#include "application/ApplicationManager.h"


#include <QString>

#include <algorithm>
#include <execution>
#include <limits>
#include <chrono>

#include "sampling.h"
#include "slice/gpuSlicer.h"



//
//#define STBI_WINDOWS_UTF16
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "slice/include/stb_image_write.h"

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

QVector3D getAbs(const QVector3D vec)
{
	return QVector3D(std::abs(vec.x()), std::abs(vec.y()), std::abs(vec.z()));
}

void Hix::Features::HollowMesh::uniformSampling(float offset)
{
	constexpr float resolution = 1.0f;

	Hix::Engine3D::Bounds3D aabb = _target->aabb();
	aabb.localBoundUpdate(*_target->getMesh());
	aabb = aabb.centred();

	//int xMin = std::floorf(aabb.xMin());
	//int xMax = std::ceilf(aabb.xMax());
	//int yMin = std::floorf(aabb.yMin());
	//int yMax = std::ceilf(aabb.yMax());
	//int zMin = std::floorf(aabb.zMin());
	//int zMax = std::ceilf(aabb.zMax());

	auto half = 50.0f;

	float xMin = -half;
	float xMax = half;
	float yMin = -half;
	float yMax = half;
	float zMin = aabb.zMin();
	float zMax = aabb.zMax();

	int lengthX = std::ceilf((xMax - xMin) / resolution);
	int lengthY = std::ceilf((yMax - yMin) / resolution);
	int lengthZ = std::ceilf((zMax - zMin) / resolution);

	const Mesh* originalMesh = _target->getMesh();
	Mesh* newMesh = new Mesh();
	std::vector<std::pair<float, int>> DF(lengthX * lengthY * lengthZ);


	std::vector<float> zVec;
	for (float z = zMin; z < zMax; z += resolution)
		zVec.push_back(z);

	_rayAccel.reset(new Hix::Engine3D::BVH(*_target, false));


	std::for_each(std::execution::par_unseq, std::begin(zVec), std::end(zVec), [&](int z)
	{
		for (float y = yMin; y < yMax; y += resolution)
		{
			for (float x = xMin; x < xMax; x += resolution)
			{
				QVector3D currPt = QVector3D(x, y, z);
				auto bvhdist = _rayAccel->getClosestDistance(currPt);

				//DF[((x + std::abs(xMin)) / resolution) +
				//	(((y + std::abs(yMin)) / resolution) * lengthX) +
				//	((z + std::abs(zMin)) / resolution) * (lengthX * lengthY)] = bvhdist;
				//DF[((x + std::abs(xMin)) / resolution) +
				//	(((y + std::abs(yMin)) / resolution) * lengthX) +
				//	((z + std::abs(zMin)) / resolution) * (lengthX * lengthY)] = bvhdist;

				//auto normalST = currPt - closestPoint;
				//normalST.normalize();
				//auto dot = QVector3D::dotProduct(normalST, closestFace.localFn());
				//auto closestDistance_sign = dot > 0.0f ? 1.0f : -1.0f;

				//qDebug() << "closest: " << closestPoint;
				//qDebug() << "normalize: " << normalST;
				//qDebug() << "localFn: " << closestFace.localFn();
				//qDebug() << "dot: " << dot;

				//DF.push_back(std::make_pair(bvhdist.first, bvhdist.second));

				//qDebug() << (settingdone - start)z.count();
				//qDebug() << (useRay - loopstart).count();
				//qDebug() << (faceloopStart - useRay).count();
				//qDebug() << (pushBack - faceloopStart).count();
			}
		}
	});
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
		auto pixel = (dist.first / max_dist->first) * 255;

		mapped_DF.push_back(pixel);

		if (dist.second > 0)
			mapped_DF.push_back(0);
		else
			mapped_DF.push_back(255);
		mapped_DF.push_back(0);
		++cnt;

	}
	qDebug() << pixSet.size();
	qDebug() << pixSet_t.size();

	uint8_t res_x = (xMax - xMin) / resolution;
	uint8_t res_y = (yMax - yMin) / resolution;

	//stbi_write_png(file.c_str(), lengthX, lengthY, 3, mapped_DF.data(), lengthX*3);
}