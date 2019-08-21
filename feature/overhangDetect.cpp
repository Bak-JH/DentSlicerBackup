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


Overhangs Hix::OverhangDetect::detectOverhang(const Mesh* shellMesh)
{
	Overhangs overhangs;
	//be generous with bin size for face support
	XYzHasher faceHasher(8 * scfg->support_density, 8 * scfg->support_density);
	XYzHasher ptHasher(2 * scfg->support_density, scfg->layer_height);

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
		overhangs.push_back(overhang.second);
	}


	return overhangs;

}
