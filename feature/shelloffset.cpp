#include "shelloffset.h"
#include "../qml/components/Inputs.h"
#include "../qml/components/ControlOwner.h"
#include "feature/repair/meshrepair.h"
#include "Mesh/mesh.h"
#include "cut/ZAxialCut.h"
#include "deleteModel.h"
#include "application/ApplicationManager.h"
#include "common/debugging/DebugRenderObject.h"


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
	Hix::Engine3D::Mesh* originalMesh = _target->getMeshModd();
	_prevMesh.reset(originalMesh);
	auto hollowMesh = new Mesh(*originalMesh);

	Mesh* newMesh = new Mesh();
	_samplingBound.localBoundUpdate(*hollowMesh);
	_samplingBound = _samplingBound.centred();

	int xMin = std::floorf(_samplingBound.xMin());
	int xMax = std::ceilf(_samplingBound.xMax());
	int yMin = std::floorf(_samplingBound.yMin());
	int yMax = std::ceilf(_samplingBound.yMax());
	int zMin = std::floorf(_samplingBound.zMin());
	int zMax = std::ceilf(_samplingBound.zMax());

	_samplingBound.setMinPt(QVector3D(xMin, yMin, zMin));
	_samplingBound.setMaxPt(QVector3D(xMax, yMax, zMax));

	int lengthX = std::ceilf((xMax - xMin + 1) / _resolution);
	int lengthY = std::ceilf((yMax - yMin + 1) / _resolution);
	int lengthZ = std::ceilf((zMax - zMin + 1) / _resolution);

	_SDF.resize(lengthX * lengthY * lengthZ);


	std::vector<float> zVec;
	for (float z = zMin; z <= zMax; z += _resolution)
		zVec.push_back(z);

	_rayCaster.reset(new MTRayCaster());
	_rayAccel.reset(new Hix::Engine3D::BVH(*_target, false));
	_rayCaster->addAccelerator(_rayAccel.get());

	std::vector<QVector3D> voxel;
	std::for_each(std::execution::par_unseq, std::begin(zVec), std::end(zVec), [&](int z)
		//for (float z = zMin; z <= zMax; z += _resolution)
		{
			for (float y = yMin; y <= yMax; y += _resolution)
			{
				for (float x = xMin; x <= xMax; x += _resolution)
				{
					QVector3D currPt = QVector3D(x, y, z);
					auto bvhdist = _rayAccel->getClosestDistance(currPt);

					int indxex = std::floorf(((x + std::abs(xMin)) / _resolution) +
						(((y + std::abs(yMin)) / _resolution) * lengthX) +
						((z + std::abs(zMin)) / _resolution) * (lengthX * lengthY));

					RayHits hits;
					for (auto fixValue = -1.0f; fixValue < 1.0f; fixValue += 0.1f)
					{
						QVector3D rayDirection((_samplingBound.centre() + QVector3D(fixValue, fixValue, 0)) - currPt);
						rayDirection.normalize();
						hits = getRayHitPoints(currPt, rayDirection);
						if (hits.size() != 0 && hits.at(0).type != HitType::Degenerate)
						{
							break;
						}
					}

					auto distSign = hits.size() % 2 == 1 ? -1.0f : 1.0f;
					_SDF[indxex] = bvhdist.first * distSign;
				}
			}
		});

	for (float z = zMin + _resolution; z <= zMax; z += _resolution)
	{
		for (float y = yMin + _resolution; y <= yMax; y += _resolution)
		{
			for (float x = xMin + _resolution; x <= xMax; x += _resolution)
			{
				auto cubeindex = 0;

				auto p0 = QVector3D(x - _resolution, y, z - _resolution);
				auto p1 = QVector3D(x, y, z - _resolution);
				auto p2 = QVector3D(x, y - _resolution, z - _resolution);
				auto p3 = QVector3D(x - _resolution, y - _resolution, z - _resolution);
				auto p4 = QVector3D(x - _resolution, y, z);
				auto p5 = QVector3D(x, y, z);
				auto p6 = QVector3D(x, y - _resolution, z);
				auto p7 = QVector3D(x - _resolution, y - _resolution, z);

				auto v0 = getSDFValue(p0);
				auto v1 = getSDFValue(p1);
				auto v2 = getSDFValue(p2);
				auto v3 = getSDFValue(p3);
				auto v4 = getSDFValue(p4);
				auto v5 = getSDFValue(p5);
				auto v6 = getSDFValue(p6);
				auto v7 = getSDFValue(p7);

				auto isolevel = -_offset;

				if (v0 < isolevel) { cubeindex |= 1;   /*voxel.push_back(p0);*/ }
				if (v1 < isolevel) { cubeindex |= 2;   /*voxel.push_back(p1);*/ }
				if (v2 < isolevel) { cubeindex |= 4;   /*voxel.push_back(p2);*/ }
				if (v3 < isolevel) { cubeindex |= 8;   /*voxel.push_back(p3);*/ }
				if (v4 < isolevel) { cubeindex |= 16;  /*voxel.push_back(p4);*/ }
				if (v5 < isolevel) { cubeindex |= 32;  /*voxel.push_back(p5);*/ }
				if (v6 < isolevel) { cubeindex |= 64;  /*voxel.push_back(p6);*/ }
				if (v7 < isolevel) { cubeindex |= 128; /*voxel.push_back(p7);*/ }

				QVector3D edgeList[12];


				/* Find the vertices where the surface intersects the cube */
				if (edgeTable[cubeindex] & 1)
					edgeList[0] = VertexInterp(isolevel, p0, p1, v0, v1);
				if (edgeTable[cubeindex] & 2)
					edgeList[1] = VertexInterp(isolevel, p1, p2, v1, v2);
				if (edgeTable[cubeindex] & 4)
					edgeList[2] = VertexInterp(isolevel, p2, p3, v2, v3);
				if (edgeTable[cubeindex] & 8)
					edgeList[3] = VertexInterp(isolevel, p3, p0, v3, v0);
				if (edgeTable[cubeindex] & 16)
					edgeList[4] = VertexInterp(isolevel, p4, p5, v4, v5);
				if (edgeTable[cubeindex] & 32)
					edgeList[5] = VertexInterp(isolevel, p5, p6, v5, v6);
				if (edgeTable[cubeindex] & 64)
					edgeList[6] = VertexInterp(isolevel, p6, p7, v6, v7);
				if (edgeTable[cubeindex] & 128)
					edgeList[7] = VertexInterp(isolevel, p7, p4, v7, v4);
				if (edgeTable[cubeindex] & 256)
					edgeList[8] = VertexInterp(isolevel, p0, p4, v0, v4);
				if (edgeTable[cubeindex] & 512)
					edgeList[9] = VertexInterp(isolevel, p1, p5, v1, v5);
				if (edgeTable[cubeindex] & 1024)
					edgeList[10] = VertexInterp(isolevel, p2, p6, v2, v6);
				if (edgeTable[cubeindex] & 2048)
					edgeList[11] = VertexInterp(isolevel, p3, p7, v3, v7);


				auto ntriang = 0;
				for (auto i = 0; triTable[cubeindex][i] != -1; i += 3) {
					newMesh->addFace(edgeList[triTable[cubeindex][i]],
						edgeList[triTable[cubeindex][i + 1]],
						edgeList[triTable[cubeindex][i + 2]]);
				}

			}
		}
	}
	newMesh->reverseFaces();
	*hollowMesh += *newMesh;
	_target->setMesh(hollowMesh);
}

bool IsSimillar(float i) { return std::abs(i); }

Hix::Engine3D::RayHits Hix::Features::HollowMesh::getRayHitPoints(QVector3D rayOrigin, QVector3D rayDirection)
{
	auto normal = _rayCaster->rayIntersectDirection(rayOrigin, rayDirection);
	std::vector<QVector3D> hitPoints;
	RayHits result;

	for (auto& r : normal)
	{
		if ((r.type != HitType::Miss && r.type != HitType::Degenerate)
			&& std::find(hitPoints.begin(), hitPoints.end(), r.intersection) == hitPoints.end())
		{
			hitPoints.push_back(r.intersection);
			result.push_back(r);
		}
	}		

	return result;
}

float Hix::Features::HollowMesh::getSDFValue(QVector3D point)
{
	int xMin = std::floorf(_samplingBound.xMin());
	int xMax = std::ceilf(_samplingBound.xMax());
	int yMin = std::floorf(_samplingBound.yMin());
	int yMax = std::ceilf(_samplingBound.yMax());
	int zMin = std::floorf(_samplingBound.zMin());
	int zMax = std::ceilf(_samplingBound.zMax());

	int lengthX = std::ceilf((xMax - xMin + 1) / _resolution);
	int lengthY = std::ceilf((yMax - yMin + 1) / _resolution);
	int lengthZ = std::ceilf((zMax - zMin + 1) / _resolution);

	int indxex = std::floorf(((point.x() + std::abs(xMin)) / _resolution) +
		(((point.y() + std::abs(yMin)) / _resolution) * lengthX) +
		((point.z() + std::abs(zMin)) / _resolution) * (lengthX * lengthY));

	return _SDF[indxex];
}

QVector3D Hix::Features::HollowMesh::VertexInterp(float isolevel, QVector3D p1, QVector3D p2, float valp1, float valp2)
{
	double mu;
	QVector3D p;

	if (std::abs(isolevel - valp1) < 0.00001)
		return(p1);
	if (std::abs(isolevel - valp2) < 0.00001)
		return(p2);
	if (std::abs(valp1 - valp2) < 0.00001)
		return(p1);

	mu = (isolevel - valp1) / (valp2 - valp1);

	p.setX(p1.x() + mu * (p2.x() - p1.x()));
	p.setY(p1.y() + mu * (p2.y() - p1.y()));
	p.setZ(p1.z() + mu * (p2.z() - p1.z()));

	return(p);
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
