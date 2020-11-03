#include "VerticalSupportModel.h"
#include "SupportRaftManager.h"
#include "../feature/Extrude.h"
#include "SupportRaftManager.h"
#include "../glmodel.h"
#include "../feature/Shapes2D.h"
#include "../application/ApplicationManager.h"
constexpr  float SUPPORT_CONE_LENGTH =  1.0f;
constexpr  float SUPPORT_BOTTOM_MAX_MULT = 2.0f;
constexpr  float SUPPORT_BOTTOM_MAX_LENGTH = 1.0f;

//constexpr  float SUPPORT_OVERLAP_LENGTH = SUPPORT_CONE_LENGTH/2;


using namespace Hix::Engine3D;
using namespace Hix::Input;
using namespace Hix::Render;
using namespace Hix::Support;
using namespace Hix::OverhangDetect;
using namespace Hix::Shapes2D;
using namespace Qt3DCore;



Hix::Support::VerticalSupportModel::VerticalSupportModel(SupportRaftManager* manager, const Overhang& overhang):
	SupportModel(manager), _overhang(overhang)
{
	generateMesh();

}

VerticalSupportModel::~VerticalSupportModel()
{
	//unlike other SceneEntities, Raft and support owns their mesh data
}

bool Hix::Support::VerticalSupportModel::hasBasePt() const
{
	return _hasBasePt;
}

const QVector3D& Hix::Support::VerticalSupportModel::getBasePt() const
{
	return _basePt;
}

const Overhang& Hix::Support::VerticalSupportModel::getOverhang() const
{
	return _overhang;
}

std::optional<std::array<QVector3D, 2>> Hix::Support::VerticalSupportModel::verticalSegment()
{
	return _vertSeg;
}


//sort in decreasing z order
struct RayHitZSort {
	bool operator()(const RayHit& a, const RayHit& b) const
	{
		return a.intersection.z() > b.intersection.z();
	}
};
RayHit getTopMost(RayHits& hits)
{
	RayHit& top = hits.front();
	for (auto& each : hits)
	{
		if (each.intersection.z() > top.intersection.z())
			top = each;
	}
	return top;
}



RayHits rayCastBottomPyramid(RayCaster& caster, const QVector3D& origin, float bottomZ, float pyramidBaseRadius,
	const std::vector<QVector3D> crossSec, std::vector<QVector3D>& misses)
{
	RayHits allRayHits;
	QVector3D rayDest = origin;
	rayDest.setZ(bottomZ);
	float x = rayDest.x() - pyramidBaseRadius;
	float y = rayDest.y() - pyramidBaseRadius;
	constexpr size_t STEPS = 20;
	misses.reserve(STEPS * STEPS);
	float stepSize = pyramidBaseRadius * 2 / STEPS;
	for (size_t i = 0; i < STEPS; ++i)
	{
		for (size_t j = 0; j < STEPS; ++j)
		{
			auto rayCastResults = caster.rayIntersect(origin, rayDest);
			if (rayCastResults.empty())
			{
				//check if this ray can support min support radius
				//just adding on x,y results in cylinder that is always bigger than the support itself
				if (caster.checkCSecInteresect(origin, rayDest, crossSec))
				{
					misses.emplace_back(rayDest);
					return allRayHits;
				}
			}
			else
			{
				allRayHits.emplace_back(getTopMost(rayCastResults));
			}
			rayDest[1] += stepSize;
		}
		rayDest[0] += stepSize;
	}
	return allRayHits;
}




float getRadius(const QVector3D& origin, float bottomZ)
{
	//45 degrees
	return (origin.z() - bottomZ)/2;
}


void getProjectedTris





void Hix::Support::VerticalSupportModel::generateSupportPath(float bottom, std::vector<float>& scales)
{	 
	auto& setting = Hix::Application::ApplicationManager::getInstance().settings().supportSetting;
	float minSupportScale = setting.supportRadiusMin / setting.supportRadiusMax;

	//calculate overlap distance
	//float intoMeshLength = 0.0f; //120 micron

	auto overhangModelCoordinates = _overhang.owner()->ptToLocal(_overhang.coord());
	auto overhangModelNormal = _overhang.owner()->ptToLocal(_overhang.normal()).normalized();
	auto overhangIntoModelNormal = -overhangModelNormal;
	auto startingFace = _overhang.nearestFace();
	//less than 90 degrees, so 2
	//float maxNormSquaredDiff = 2.0f;
	//auto overlapFaces = _overhang.owner()->findNearSimilarFaces(overhangModelCoordinates, overhangModelNormal, startingFace, maxNormSquaredDiff, 100, setting.supportRadiusMin);
	//std::unordered_set<VertexConstItr> attachmentPts;
	//for (auto& f : overlapFaces)
	//{
	//	auto vtcs = f.meshVertices();
	//	for (auto& e : vtcs)
	//	{
	//		attachmentPts.insert(e);
	//	}
	//}
	//for (auto& v : attachmentPts)
	//{
	//	auto local = v.localPosition();
	//	auto dist = v.localPosition().distanceToPlane(overhangModelCoordinates, overhangIntoModelNormal);
	//	if (dist > 0.0f)
	//	{
	//		intoMeshLength = std::max(intoMeshLength, dist);
	//	}
	//}
	//constexpr float intoMeshLength = 0.120f; //120 micron
	constexpr float coneLength = SUPPORT_CONE_LENGTH;

	float botNormalLength = 0.100f;
	float layerHeight = Hix::Application::ApplicationManager::getInstance().settings().sliceSetting.layerHeight;

	auto zCloseMin = _overhang.coord().z() - layerHeight;
	auto rayOrigin(_overhang.coord());
	auto rayEnd(_overhang.coord());
	rayEnd.setZ(0);
	auto rayCastResults = _manager->supportRaycaster().rayIntersect(rayOrigin, rayEnd);
	_hasBasePt = false;
	//get rid of hits that won't  be visible when sliced
	for (auto hitItr = rayCastResults.begin(); hitItr != rayCastResults.end();)
	{
		if (zCloseMin < hitItr->intersection.z())
		{
			hitItr = rayCastResults.erase(hitItr);
		}
		else
			++hitItr;
	}
	if (!rayCastResults.empty())
	{

		//setMaterialColor(Hix::Render::Colors::OutOfBound);
		std::sort(rayCastResults.begin(), rayCastResults.end(), RayHitZSort());
		//if the raycasted side is on the backside of a triangle, this is not overhang
		auto& topMost = rayCastResults.front();
		if (topMost.type == HitType::BackSide)
			return;
		auto gapLength = _overhang.coord().z() -  topMost.intersection.z();
		auto doConeLength = coneLength * 2;
		if (gapLength < doConeLength)
		{
			//just do normal cylinder between the areas
			QVector3D extendedTip = _overhang.coord();
			extendedTip[2] += intoMeshLength;
			_contour.emplace_back(extendedTip);
			_contour.emplace_back(topMost.intersection);
			scales = { minSupportScale, minSupportScale};
			return;
		}
		else
		{
			QVector3D coneNarrow(_overhang.coord());
			QVector3D tipNormal(_overhang.normal());
			//tip normal needs to be facing downard, ie) cone needs to be pointing upward,
			constexpr float normalizedVectorZMax = -1.0f; //tan 45
			QVector2D xy(tipNormal.x(), tipNormal.y());
			auto zMax = normalizedVectorZMax * xy.length();
			tipNormal.setZ(std::min(zMax, tipNormal.z()));
			tipNormal.normalize();

			//because of float error, it's safer to overlap support and mesh a little bit, so extend endtip into mesh a bit
			QVector3D extendedTip = coneNarrow - (intoMeshLength* tipNormal);
			QVector3D upperZPath = coneNarrow + tipNormal * coneLength;

			constexpr float cos30 = 0.86602540378;
			constexpr float cos0 = 1;
			constexpr QVector3D zUp(0, 0, 1);
			float currSmallestAngle = std::numeric_limits<float>::max();
			std::vector<QVector3D> misses;
			auto radiusMax = Hix::Application::ApplicationManager::getInstance().settings().supportSetting.supportRadiusMax;
			auto suppCSec = Hix::Shapes2D::generateCircle(radiusMax, 4);
			auto pyramidRayCasts = rayCastBottomPyramid(_manager->supportRaycaster(), upperZPath, 0, getRadius(upperZPath, 0), suppCSec, misses);
			if (!misses.empty())
			{
				_contour.emplace_back(misses.front());
				_contour.emplace_back(upperZPath);
				_contour.emplace_back(coneNarrow); //min
				_contour.emplace_back(extendedTip);
				scales = { 1.0f,1.0f, minSupportScale, minSupportScale};
				_hasBasePt = true;
				_basePt = misses.front();
				return;
			}
			else
			{
				//cache fn
				std::unordered_map<FaceConstItr, QVector3D> fnCache;
				RayHit* bestHit = nullptr;

				for (auto& each : pyramidRayCasts)
				{
					//cache face noraml cause it's kinda expensive
					auto cached = fnCache.find(each.face);
					QVector3D fn;
					if (cached == fnCache.end())
					{
						fn = each.face.worldFn();
						fnCache.insert(std::make_pair(each.face, fn));
					}
					else
					{
						fn = cached->second;
					}

					//angle between positive z Axis and face normal
					auto cosWithZ = QVector3D::dotProduct(zUp, fn);
					if (cos30 < cosWithZ && cosWithZ <= cos0)
					{
						//angle between ray and face normal
						QVector3D reverseRay = upperZPath - each.intersection;
						reverseRay.normalize();
						auto dotWithRay = QVector3D::dotProduct(zUp, fn);
						if (dotWithRay >= 0 && dotWithRay < currSmallestAngle)
						{
							currSmallestAngle = dotWithRay;
							bestHit = &each;
						}
					}
				}
				if (!bestHit)
					return;
				QVector3D bottom = bestHit->intersection;
				QVector3D bottomZPath = bottom + (fnCache[bestHit->face] * coneLength);
				QVector3D bottomExtendedTip = bottom - (fnCache[bestHit->face] * intoMeshLength);
				_contour.emplace_back(bottomExtendedTip); //min
				_contour.emplace_back(bottom); //min
				_contour.emplace_back(bottomZPath); //max
				_contour.emplace_back(upperZPath); //max
				_contour.emplace_back(coneNarrow); //min
				_contour.emplace_back(extendedTip); //min
				scales = { minSupportScale, minSupportScale, 1.0f, 1.0f,  minSupportScale, minSupportScale};
				return;
			}

		}
	}
	else
	{
		//vertical support path should be very simple, cone pointy bit(endtip), cone ending bit, bottom bit.

		QVector3D coneNarrow(_overhang.coord());
		QVector3D tipNormal(_overhang.normal());
		//tip normal needs to be facing downard, ie) cone needs to be pointing upward,
		constexpr float normalizedVectorZMax = -1.0f; //tan 45
		QVector2D xy(tipNormal.x(), tipNormal.y());
		auto zMax = normalizedVectorZMax * xy.length();
		tipNormal.setZ(std::min(zMax, tipNormal.z()));
		tipNormal.normalize();

		//because of float error, it's safer to overlap support and mesh a little bit, so extend endtip into mesh a bit
		QVector3D extendedTip = coneNarrow - (intoMeshLength * tipNormal);

		//cone of support is always in the direction of the overhang normal to minimize contact between support and mesh
		QVector3D coneWidePart = coneNarrow +  (coneLength * tipNormal);
		//part of support that's in raft
		QVector3D supportStart = coneWidePart;
		QVector3D raftAttachStart = coneWidePart;
		QVector3D raftAttachEnd = coneWidePart;

		supportStart.setZ(0);
		raftAttachStart.setZ(setting.raftThickness);
		raftAttachEnd.setZ(setting.raftThickness + SUPPORT_BOTTOM_MAX_LENGTH);
		_contour.emplace_back(supportStart);
		_contour.emplace_back(raftAttachStart);
		_contour.emplace_back(raftAttachEnd);
		_contour.emplace_back(coneWidePart);
		_contour.emplace_back(coneNarrow);
		_contour.emplace_back(extendedTip);
		_vertSeg = { raftAttachEnd, coneWidePart };
		auto botMult = 1.0f;
		if (setting.thickenFeet)
			botMult = SUPPORT_BOTTOM_MAX_MULT;
		scales = { botMult, botMult, 1.0f, 1.0f, minSupportScale, minSupportScale};
		_hasBasePt = true;
		_basePt = supportStart;
	}


	return;
}	 
	 

	 



QVector4D Hix::Support::VerticalSupportModel::getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)
{
	return QVector4D();
}

void Hix::Support::VerticalSupportModel::generateMesh()
{
	std::vector<QVector3D> crossContour;
	std::vector<float> scales;
	auto mesh = new Mesh();

	auto radiusMax = Hix::Application::ApplicationManager::getInstance().settings().supportSetting.supportRadiusMax;
	generateSupportPath(_manager->supportBottom(), scales);
	if (!_contour.empty())
	{
		crossContour = generateCircle(radiusMax, 16);
		std::vector<std::vector<QVector3D>> jointContours;
		std::function<void(std::vector<QVector3D>&, float)> uniformScaler(Hix::Shapes2D::scaleContour);

		Hix::Features::Extrusion::extrudeAlongPath(mesh, QVector3D(0, 0, 1), crossContour, _contour, jointContours, &scales, &uniformScaler);

		//create endcaps using joint contours;
		circleToTri(mesh, jointContours.front(), _contour.front(), true);
		circleToTri(mesh, jointContours.back(), _contour.back(), false);
	}
	setMesh(mesh);

}

