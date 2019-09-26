#include "overhangDetect.h"
#include "DentEngine/src/polyclipping/polyclipping.h"
#include "DentEngine/src/slicer.h"
#include "DentEngine/src/configuration.h"
#include <QtMath>

using namespace Hix;
using namespace Hix::Engine3D;
using namespace Hix::Slicer;
using namespace Hix::OverhangDetect;

const double critical_angle = 45;
const double critical_angle_radian = M_PI * (critical_angle / 180.0);
float area_subdiv = 20.0f;

namespace std
{
	template<>
	struct hash<QVector3D>
	{
		//2D only!
		std::size_t operator()(const QVector3D& pt)const
		{
			constexpr static Hix::Engine3D::MeshVtxHasher hasher;
			return hasher(pt);
			//TODO

		}
	};
}

namespace OverhangDetectPrivate
{
	std::unordered_set<VertexConstItr> localMinFacingDown(const Mesh* mesh);
	std::unordered_map<QVector3D, FaceConstItr> faceOverhangDetect(const Mesh* mesh);
	void faceOverhangPoint(const FaceConstItr& overhangFace, std::unordered_map<QVector3D, FaceConstItr>& output);
	class XYzHasher
	{
	public:
		XYzHasher(float xyMin, float zMin);
		size_t operator()(QVector3D v);
		const float xyMinDist;
		const float zMinDist;
	};

}



using namespace OverhangDetectPrivate;
XYzHasher::XYzHasher(float xyMin, float zMin): xyMinDist(xyMin), zMinDist(zMin)
{

}

size_t XYzHasher::operator()(QVector3D v)
{
	return (size_t(((v.x() + xyMinDist / 2) / xyMinDist)) ^
		(size_t(((v.y() + xyMinDist / 2) / xyMinDist)) << 10) ^
		(size_t(((v.z() + zMinDist / 2) / zMinDist)) << 20));
}


std::unordered_set<VertexConstItr> OverhangDetectPrivate::localMinFacingDown(const Mesh* mesh){

	std::unordered_set<VertexConstItr> minis;
	const auto& vertices = mesh->getVertices();
	for (auto vtx = vertices.cbegin(); vtx != vertices.cend(); ++vtx)
	{
		if (vtx->vn.z() < 0.0f)
		{
			bool localMin = true;
			auto connected = vtx->connectedVertices();
			for (auto& each : connected)
			{
				if (each->position.z() < vtx->position.z())
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



std::unordered_map<QVector3D, FaceConstItr> OverhangDetectPrivate::faceOverhangDetect(const Mesh* mesh) {
	QVector3D printingDirection = QVector3D(0, 0, 1);
	std::unordered_map<QVector3D, FaceConstItr> faceOverhangs;
	auto cosVal =  std::abs(std::cos(critical_angle_radian));
	const auto& faces = mesh->getFaces();
	for (auto face = faces.cbegin(); face != faces.cend(); ++face)
	{
		if (face->fn.z() > 0) continue;
		double cos = double(QVector3D::dotProduct(face->fn, printingDirection));
		if (std::abs(cos) > cosVal) {
			faceOverhangPoint(face, faceOverhangs);
		}
	}
	return faceOverhangs;
}

void OverhangDetectPrivate::faceOverhangPoint(const FaceConstItr& overhangFace, std::unordered_map<QVector3D, FaceConstItr>& output) {
	// area subdivision (recursive case)

	auto startingVertices = overhangFace->meshVertices();
	std::queue<std::array<QVector3D, 3>> q;
	q.emplace(std::array<QVector3D, 3>{startingVertices[0]->position, startingVertices[1]->position, startingVertices[2]->position});
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
			output[overhangPoint] = overhangFace;
		}

	}
}


std::vector<QVector3D> Hix::OverhangDetect::toCoords(const Overhangs& overhangs)
{
	std::vector<QVector3D> coords;
	coords.reserve(overhangs.size());
	for (auto& each : overhangs)
	{
		if (each.index() == 0)
		{
			coords.emplace_back(std::get<0>(each)->position);
		}
		else
		{
			coords.emplace_back(std::get<1>(each).first);
		}
	}
	return coords;
}

Overhangs Hix::OverhangDetect::detectOverhang(const Mesh* shellMesh)
{
	Overhangs overhangs;

	//if density is 0, don't detect anything
	if (scfg->supportDensity == 0)
		return overhangs;
	auto suppDensity = (float)scfg->supportDensity / 100.0f;
	//be generous with bin size for face support
	constexpr float minDist = 0.8f;
	auto ptOverhangMinDist = minDist / suppDensity;
	auto faceOverhangMinDist = ptOverhangMinDist * 4;

	XYzHasher faceHasher(faceOverhangMinDist, faceOverhangMinDist);
	XYzHasher ptHasher(ptOverhangMinDist, ptOverhangMinDist);

	//this is needed to remove too close support points
	std::unordered_map<size_t, FaceOverhang> faceHashedOverhangs;
	std::unordered_map<size_t, std::variant<VertexConstItr, FaceOverhang>> allHashedOverhangs;
	//get all local minimas
	auto pointOverhangs = localMinFacingDown(shellMesh);
	auto faceOverhangs = faceOverhangDetect(shellMesh);
	for (auto& faceOverhang : faceOverhangs)
	{
		faceHashedOverhangs[faceHasher(faceOverhang.first)] = faceOverhang;
	}
	for (auto& ptOverhang : pointOverhangs)
	{
		allHashedOverhangs[ptHasher(ptOverhang->position)] = ptOverhang;
	}
	//rehash
	for (auto& faceOverhang : faceHashedOverhangs)
	{
		allHashedOverhangs[ptHasher(faceOverhang.second.first)] = faceOverhang.second;
	}
	overhangs.reserve(allHashedOverhangs.size());
	for (auto& overhang : allHashedOverhangs)
	{
		overhangs.insert(overhang.second);
	}


	return overhangs;

}
