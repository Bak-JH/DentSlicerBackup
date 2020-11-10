#include "overhangDetect.h"
#include "DentEngine/src/polyclipping/polyclipping.h"
#include "DentEngine/src/slicer.h"
#include "../application/ApplicationManager.h"
#include <QtMath>
#include "../utils/mathutils.h"
using namespace Hix;
using namespace Hix::Utils::Math;
using namespace Hix::Engine3D;
using namespace Hix::Slicer;
using namespace Hix::OverhangDetect;
const double critical_angle = 45;
const double critical_angle_radian = M_PI * (critical_angle / 180.0);
float area_subdiv = 20.0f;


//scene graph position of each support from model is:
//same coordinate system as model's

//namespace std
//{
//	template<>
//	struct hash<QVector3D>
//	{
//		//2D only!
//		std::size_t operator()(const QVector3D& pt)const
//		{
//			constexpr static Hix::Engine3D::MeshVtxHasher hasher;
//			return hasher(pt);
//			//TODO
//
//		}
//	};
//}


//since normals too horizontal can't be printed
QVector3D calcPractivalNormal(const QVector3D& overhangNormal)
{
	QVector3D tipNormal(overhangNormal);
	//tip normal needs to be facing downard, ie) cone needs to be pointing upward,
	constexpr float normalizedVectorZMax = -1.0f; //tan 45
	QVector2D xy(tipNormal.x(), tipNormal.y());
	auto zMax = normalizedVectorZMax * xy.length();
	tipNormal.setZ(std::min(zMax, tipNormal.z()));
	tipNormal.normalize();
	return tipNormal;

}


class XYzHasher
{
public:
	XYzHasher(float xyMin, float zMin);
	size_t operator()(QVector3D v);
	const float xyMinDist;
	const float zMinDist;
};
Hix::OverhangDetect::Overhang::Overhang(const FaceConstItr& face, const QVector3D& coord) : _primitive(face), _coord(coord), _primNormal(face.worldFn()), _normal(calcPractivalNormal(_primNormal))
{
}

Hix::OverhangDetect::Overhang::Overhang(const VertexConstItr& vtx) : _primitive(vtx), _coord(vtx.worldPosition()), _primNormal(vtx.worldVn()), _normal(calcPractivalNormal(_primNormal))
{
}

const QVector3D& Hix::OverhangDetect::Overhang::coord() const
{
	return _coord;
}

bool Hix::OverhangDetect::Overhang::operator==(const Overhang& o) const
{
	return _primitive == o._primitive && _coord == o._coord;
}

const Hix::Engine3D::Mesh* Hix::OverhangDetect::Overhang::owner() const
{
	if (_primitive.index() == 0)
	{
		return std::get<0>(_primitive).owner();
	}
	else
	{
		return std::get<1>(_primitive).owner();
	}
}

const QVector3D& Hix::OverhangDetect::Overhang::normal() const
{
	return _normal;
}

const QVector3D& Hix::OverhangDetect::Overhang::primitiveOverhang() const
{
	return _primNormal;
}

FaceConstItr Hix::OverhangDetect::Overhang::nearestFace() const
{
	if (_primitive.index() == 0)
	{
		return std::get<0>(_primitive);
	}
	else
	{
		auto cf = std::get<1>(_primitive).connectedFaces();
		return *cf.begin();
	}
}

bool Hix::OverhangDetect::Overhang::isVertex() const
{
	if (_primitive.index() == 1)
	{
		return true;
	}
}

//void Hix::OverhangDetect::Overhang::setRaycastResult(const RayHits& hit)
//{
//	_rayHit = hit;
//}
//
//const RayHits& Hix::OverhangDetect::Overhang::rayHit() const
//{
//	return _rayHit;
//}
//
//





XYzHasher::XYzHasher(float xyMin, float zMin): xyMinDist(xyMin), zMinDist(zMin)
{

}

size_t XYzHasher::operator()(QVector3D v)
{
	return (size_t(((v.x() + xyMinDist / 2) / xyMinDist)) ^
		(size_t(((v.y() + xyMinDist / 2) / xyMinDist)) << 10) ^
		(size_t(((v.z() + zMinDist / 2) / zMinDist)) << 20));
}


std::unordered_set<VertexConstItr> Hix::OverhangDetect::Detector::localMinFacingDown(const Mesh* mesh){

	std::unordered_set<VertexConstItr> minis;
	auto vtxCend = mesh->getVertices().cend();
	for (auto vtx = mesh->getVertices().cbegin(); vtx != vtxCend; ++vtx)
	{
		if (vtx.worldVn().z() < 0.0f)
		{
			bool localMin = true;
			auto connected = vtx.connectedVertices();
			for (auto& each : connected)
			{
				if (each.worldPosition().z() < vtx.worldPosition().z())
				{
					localMin = false;
					break;
				}
			}
			if (localMin)
				minis.insert(vtx);
		}
	}
	return minis;
}



std::unordered_multimap<FaceConstItr, QVector3D> Hix::OverhangDetect::Detector::faceOverhangDetect(const Mesh* mesh) {
	QVector3D printingDirection = QVector3D(0, 0, 1);
	std::unordered_multimap<FaceConstItr, QVector3D> faceOverhangs;
	auto cosVal =  std::abs(std::cos(critical_angle_radian));
	const auto& faces = mesh->getFaces();
	for (auto face = faces.cbegin(); face != faces.cend(); ++face)
	{
		auto worldFn = _worldFNCache[face];
		if (worldFn.z() >= 0) continue;
		double cos = double(QVector3D::dotProduct(worldFn, printingDirection));
		if (std::abs(cos) > cosVal) {
			faceOverhangPoint(face, faceOverhangs);
		}
	}
	return faceOverhangs;
}

void Hix::OverhangDetect::Detector::faceOverhangPoint(const FaceConstItr& overhangFace,
	std::unordered_multimap<FaceConstItr, QVector3D>& output) {
	// area subdivision (recursive case)

	auto startingVertices = overhangFace.meshVertices();
	std::queue<std::array<QVector3D, 3>> q;
	q.emplace(std::array<QVector3D, 3>
	{startingVertices[0].worldPosition(), startingVertices[1].worldPosition(), startingVertices[2].worldPosition()});
	while (!q.empty())
	{
		auto curr = q.front();
		q.pop();
		auto area = QVector3D::crossProduct(curr[1] - curr[0],curr[2] - curr[0]).length();
		if (area > area_subdiv)
		{
			//divide the triangle;
			auto mid01 = (curr[0] + curr[1]) / 2;
			auto mid02 = (curr[0] + curr[2]) / 2;
			auto mid12 = (curr[1] + curr[2]) / 2;

			q.emplace(std::array<QVector3D, 3>{ mid01, mid12, mid02  });
			q.emplace(std::array<QVector3D, 3>{ curr[0], mid01, mid02});
			q.emplace(std::array<QVector3D, 3>{ mid01, curr[1], mid12});
			q.emplace(std::array<QVector3D, 3>{ mid02, mid12, curr[2]});
		}
		else
		{
			QVector3D overhangPoint = (curr[0] + curr[1] + curr[2]) / 3;
			output.emplace(overhangFace, overhangPoint);
		}

	}
}



Hix::OverhangDetect::Overhangs Hix::OverhangDetect::Detector::detectOverhang(const Mesh* shellMesh)
{
	Hix::OverhangDetect::Overhangs overhangs;
	//precalculate world fn values
	auto faceCend = shellMesh->getFaces().cend();
	for (auto face = shellMesh->getFaces().cbegin(); face != faceCend; ++face)
	{
		_worldFNCache.emplace(face, face.worldFn());
	}
	//if density is 0, don't detect anything
	auto supportDensity = Hix::Application::ApplicationManager::getInstance().settings().supportSetting.supportDensity;
	if (floatAreSame(supportDensity, 0.0f))
		return overhangs;
	auto suppDensity = supportDensity / 100.0f;
	//be generous with bin size for face support
	constexpr float minDist = 0.8f;
	auto ptOverhangMinDist = minDist / suppDensity;
	auto faceOverhangMinDist = ptOverhangMinDist * 4;

	XYzHasher faceHasher(faceOverhangMinDist, faceOverhangMinDist);
	XYzHasher ptHasher(ptOverhangMinDist, ptOverhangMinDist);

	//this is needed to remove too close support points
	std::unordered_map<size_t, Overhang> faceHashedOverhangs;
	std::unordered_map<size_t, Overhang> allHashedOverhangs;
	//get all local minimas
	auto pointOverhangs = localMinFacingDown(shellMesh);
	auto faceOverhangs = faceOverhangDetect(shellMesh);
	for (auto& faceOverhang : faceOverhangs)
	{
		faceHashedOverhangs.try_emplace(faceHasher(faceOverhang.second), Overhang(faceOverhang.first, faceOverhang.second));
	}
	for (auto& ptOverhang : pointOverhangs)
	{
		allHashedOverhangs.try_emplace(ptHasher(ptOverhang.worldPosition()),Overhang(ptOverhang));
	}
	//rehash
	for (auto& faceOverhang : faceHashedOverhangs)
	{
		allHashedOverhangs.try_emplace(ptHasher(faceOverhang.second.coord()), faceOverhang.second);
	}
	overhangs.reserve(allHashedOverhangs.size());
	for (auto& hashedPt : allHashedOverhangs)
	{
		overhangs.emplace_back(hashedPt.second);
	}
	//raycast

	return overhangs;

}

