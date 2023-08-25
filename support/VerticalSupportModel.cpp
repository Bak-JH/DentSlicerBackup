#include "VerticalSupportModel.h"
#include "SupportRaftManager.h"
#include "../feature/Extrude.h"
#include "SupportRaftManager.h"
#include "../glmodel.h"
#include "../feature/Shapes2D.h"
#include "../application/ApplicationManager.h"
#include <unordered_map>
#include "../Mesh/mesh.h"
#include "../Mesh/FaceSelectionUtils.h"
#include "../feature/Plane3D.h"
#include "../common/Debug.h"

constexpr  float SUPPORT_CONE_LENGTH =  1.0f;
constexpr  float SUPPORT_BOTTOM_MAX_MULT = 2.0f;
constexpr  float SUPPORT_BOTTOM_MAX_LENGTH = 1.0f;

using namespace Hix::Engine3D;
using namespace Hix::Input;
using namespace Hix::Render;
using namespace Hix::Support;
using namespace Hix::OverhangDetect;
using namespace Hix::Shapes2D;
using namespace Hix::Features::Extrusion;
using namespace Qt3DCore;

struct Attachment
{
	QVector3D start;
	Hix::Plane3D::PDPlane end;
};


Hix::Support::VerticalSupportModel::VerticalSupportModel(SupportRaftManager* manager, const Overhang& overhang):
	SupportModel(manager), _overhang(overhang)
{
	generateMesh();

}

Hix::Support::VerticalSupportModel::VerticalSupportModel(Hix::Render::SceneEntity* parent, SupportRaftManager* manager, LoadSupportInfo& info, float supportRadiusMax):
	SupportModel(manager), _overhang(parent)
{
	std::vector<QVector3D> crossContour;
	auto mesh = new Mesh();

	if (!info.contour.empty())
	{
		crossContour = generateCircle(supportRadiusMax, 16);
		Hix::Features::Extrusion::UniformScaler scaler(info.scales);
		//auto jointDir = Hix::Features::Extrusion::interpolatedJointNormals(_contour);
		auto jointContours = Hix::Features::Extrusion::extrudeAlongPath(
			mesh, QVector3D(0, 0, 1), crossContour, info.contour, info.jointDir, &scaler);

		//create endcaps using joint contours;
		circleToTri(mesh, jointContours.front(), info.contour.front(), true);
		circleToTri(mesh, jointContours.back(), info.contour.back(), false);

		_contour = info.contour;
		_jointDir = info.jointDir;
		_scales = info.scales;

		//set base point
		_hasBasePt = true;
		_basePt = info.contour.front();
	}
	setMesh(mesh);
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

const std::vector<QVector3D>& Hix::Support::VerticalSupportModel::getJointDir()
{
	return _jointDir;
}

const Hix::Features::Extrusion::Contour& Hix::Support::VerticalSupportModel::getContour()
{
	return _contour;
}

const std::vector<float>& Hix::Support::VerticalSupportModel::getScales()
{
	return _scales;
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

	constexpr size_t STEPS = 20;
	misses.reserve(STEPS * STEPS);
	float stepSize = pyramidBaseRadius * 2 / STEPS;

	constexpr float EPS = std::numeric_limits<float>::epsilon();

	for (size_t i = 0; i < STEPS; ++i)
	{
		for (size_t j = 0; j < STEPS; ++j)
		{
			RayHits rayCastResults;
			rayCastResults = caster.rayIntersectWithoutDegen(origin, rayDest);

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





//Get set of triangles that projected circle, radius is minSupportRadius
std::unordered_set<FaceConstItr> circleProjectedTris(const Mesh& mesh, const FaceConstItr& targetFace,
	const QVector3D& circleNormal, const QVector3D& circleCenter, float radius)
{
	//cache for faster calc
	std::unordered_map<Hix::Engine3D::VertexConstItr, float> vtxPrjDst;
	constexpr auto MAXFIND = 15000;
	//constexpr auto MAXFIND = 4;

	auto insideProj = [&vtxPrjDst, circleCenter, circleNormal, radius](const FaceConstItr& nf)->bool {
		//2(1-cos15 degrees) = 0.068
		if ((nf.worldFn() - circleNormal).lengthSquared() < 0.5857864376)
		{
			auto vtcs = nf.meshVertices();
			for (auto& v : vtcs)
			{
				//for optimized non-squarerooted distance, we subtract and find length
				auto cached = vtxPrjDst.find(v);
				float distance;
				if (cached == vtxPrjDst.end())
				{
					distance = v.worldPosition().distanceToLine(circleCenter, circleNormal);
					vtxPrjDst[v] = distance;
				}
				else
				{
					distance = cached->second;
				}
				if (distance > radius)
				{
					return false;
				}
			}
			return true;
		}
		return false;

	};
	return mesh.findNearFaces(targetFace, insideProj, MAXFIND);
}

Attachment calculateAttachmentInfo(const QVector3D& supportNormal, const Overhang& overhang)
{
	Attachment att;
	auto& setting = Hix::Application::ApplicationManager::getInstance().settings().supportSetting;
	auto startingFace = overhang.nearestFace();

	auto projTris = circleProjectedTris(*overhang.owner(), startingFace,
		supportNormal, overhang.coord(), setting.supportRadiusMin);
	//debugging
	//auto entity = dynamic_cast<SceneEntityWithMaterial*>(const_cast<SceneEntity*> (overhang.owner()->entity()));
	//auto& debugger = Hix::Debug::DebugRenderObject::getInstance();
	//debugger.clear();
	//debugger.registerDebugColorFaces(entity, projTris);
	//debugger.colorDebugFaces();
	std::unordered_set<VertexConstItr> vtcs;
	vtcs.reserve(projTris.size() * 3);
	for (auto& f : projTris)
	{
		auto mvs = f.meshVertices();
		vtcs.insert(mvs[0]);
		vtcs.insert(mvs[1]);
		vtcs.insert(mvs[2]);
	}
	std::vector<QVector3D> vv;
	vv.reserve(vtcs.size());
	std::transform(vtcs.begin(), vtcs.end(), std::back_inserter(vv), [](const VertexConstItr& v) {return v.worldPosition(); });
	att.end = Hix::Plane3D::bestFittingPlane(vv);
	if (att.end.normal.z() < 0)
	{
		att.end.normal *= -1.0f;
	}
	float maxDistFromPlane = std::numeric_limits<float>::lowest();	
	float minDistFromPlane = std::numeric_limits<float>::max();

	for (auto& v : vtcs)
	{
		auto worldPos = v.worldPosition();
		auto dist = worldPos.distanceToPlane(att.end.point, att.end.normal);
		if (dist > maxDistFromPlane)
		{
			maxDistFromPlane = dist;
		}
		if (dist < minDistFromPlane)
		{
			minDistFromPlane = dist;
		}
	}
	att.start = att.end.point + (att.end.normal * (minDistFromPlane));
	att.end.point = att.end.point + (att.end.normal * maxDistFromPlane);

	//std::swap(att.start, att.end.point);
	//qDebug()<< "maxDistFromPlane: " << maxDistFromPlane << "minDistFromPlane: " << minDistFromPlane;
	//qDebug() << "end: " << att.end.point << "start: " << att.start;
	//qDebug() << "supportNormal: " << supportNormal << "att.end.normal: " << att.end.normal;

	//att.end.normal *= -1.0f;
	return att;
}
//
//QVector3D calculateFarthest()
//{
//
//}


//std::vector<QVector3D>  projectToPlane(QVector3D normal, QVector3D plane, const std::deque<HalfEdgeConstItr>& path)
//{
//	std::vector<QVector3D> vec;
//	vec.reserve(path.size() * 2);
//	for (auto& e : path)
//	{
//		auto v0 = e.from().worldPosition();
//		auto v1 = e.to().worldPosition();
//		auto projDistv0 = v0.distanceToPlane(plane, normal);
//		auto projDistv1 = v1.distanceToPlane(plane, normal);
//		auto e0 = v0 - (projDistv0 * normal);
//		auto e1 = v1 - (projDistv1 * normal);
//		//vec.emplace_back(e1);
//		//vec.emplace_back(v0);
//		//vec.emplace_back(v1);
//
//		//vec.emplace_back(e1);
//		//vec.emplace_back(e0);
//		//vec.emplace_back(v0);
//
//		vec.emplace_back(v1);
//		vec.emplace_back(v0);
//		vec.emplace_back(e1);
//
//
//		vec.emplace_back(v0);
//		vec.emplace_back(e0);
//		vec.emplace_back(e1);
//
//		//mesh.addFace(e1, v0, v1);
//		//mesh.addFace(e1, e0, v0);
//	}
//	return vec;
//}



void Hix::Support::VerticalSupportModel::generateSupportPath(float bottom, std::vector<float>& scales)
{	 
 	auto& setting = Hix::Application::ApplicationManager::getInstance().settings().supportSetting;
	float minSupportScale = setting.supportRadiusMin / setting.supportRadiusMax;
	constexpr float coneLength = SUPPORT_CONE_LENGTH;
	constexpr float doConeLength = coneLength * 2.0f;
	constexpr float upDigLength = 0.300;
	constexpr float EPS = std::numeric_limits<float>::epsilon();

	float botNormalLength = 0.100f;
	float layerHeight = Hix::Application::ApplicationManager::getInstance().settings().sliceSetting.layerHeight/1000.0f;

	auto zCloseMin = _overhang.coord().z() - layerHeight;
	auto rayOrigin(_overhang.coord());

	_manager->prepareRaycasterSelected();

	RayHits rayCastResults;
	auto rayEnd = QVector3D(_overhang.coord().x(), _overhang.coord().y(), 0);
	rayCastResults = _manager->supportRaycaster().rayIntersectWithoutDegen(rayOrigin, rayEnd);


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
		if (gapLength < doConeLength)
		{
			//just do normal cylinder between the areas
			//QVector3D extendedTip = _overhang.coord();
			//extendedTip[2] += upDigLength;
			QVector3D tipNormal = topMost.intersection - _overhang.coord();
			tipNormal.normalize();
			auto attachment = calculateAttachmentInfo(tipNormal, _overhang);

			auto& man = Hix::Application::ApplicationManager::getInstance().supportRaftManager();
			auto& supSetting = Hix::Application::ApplicationManager::getInstance().settings().supportSetting;
			for (auto pt : man.getSupportBasePts())
			{
				if (attachment.end.point.distanceToPoint(pt + QVector3D(0, 0, supSetting.raftThickness + supSetting.supportBaseHeight)) <= 0.001f)
				{
					return;
				}
			}

			_contour.emplace_back(topMost.intersection);
			_contour.emplace_back(attachment.end.point);
			scales = { minSupportScale, minSupportScale};
			_jointDir = Hix::Features::Extrusion::interpolatedJointNormals(_contour);
			_jointDir.back() = attachment.end.normal;
			return;
		}
		else
		{
			//because of float error, it's safer to overlap support and mesh a little bit, so extend endtip into mesh a bit
			//QVector3D extendedTip = coneNarrow - (upDigLength * _overhang.normal());
			auto attachment = calculateAttachmentInfo(_overhang.normal(), _overhang);

			auto& man = Hix::Application::ApplicationManager::getInstance().supportRaftManager();
			auto& supSetting = Hix::Application::ApplicationManager::getInstance().settings().supportSetting;
			for (auto pt : man.getSupportBasePts())
			{
				if (attachment.end.point.distanceToPoint(pt + QVector3D(0, 0, supSetting.raftThickness + supSetting.supportBaseHeight)) <= 0.001f)
				{
					return;
				}
			}

			auto coneNarrowNormal = attachment.end.normal * -1.0f;
			QVector3D upperZPath = attachment.start + coneNarrowNormal * coneLength;

			float currSmallestAngle = std::numeric_limits<float>::max();
			std::vector<QVector3D> misses;
			auto radiusMax = Hix::Application::ApplicationManager::getInstance().settings().supportSetting.supportRadiusMax;
			auto suppCSec = Hix::Shapes2D::generateCircle(radiusMax, 4);
			auto pyramidRayCasts = rayCastBottomPyramid(_manager->supportRaycaster(), 
				upperZPath, 0, getRadius(upperZPath, 0), suppCSec, misses);
			if (!misses.empty())
			{
				_contour.emplace_back(misses.front());
				_contour.emplace_back(upperZPath);
				_contour.emplace_back(attachment.start); //min
				_contour.emplace_back(attachment.end.point);
				scales = { 1.0f,1.0f, minSupportScale, minSupportScale};
				_hasBasePt = true;
				_basePt = misses.front();
				_jointDir = Hix::Features::Extrusion::interpolatedJointNormals(_contour);
				_jointDir.back() = attachment.end.normal;
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

					constexpr float cos30 = 0.86602540378;
					constexpr float cos0 = 1;
					constexpr QVector3D zUp(0, 0, 1);
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

				auto& man = Hix::Application::ApplicationManager::getInstance().supportRaftManager();
				auto& supSetting = Hix::Application::ApplicationManager::getInstance().settings().supportSetting;
				for (auto pt : man.getSupportBasePts())
				{
					if (attachment.end.point.distanceToPoint(pt + QVector3D(0, 0, supSetting.raftThickness + supSetting.supportBaseHeight)) <= 0.001f)
					{
						return;
					}
				}

				QVector3D bottomZPath = bottom + (fnCache[bestHit->face] * coneLength);
				constexpr float bottomDigLength = 0.120;
				QVector3D bottomExtendedTip = bottom - (fnCache[bestHit->face] * bottomDigLength);
				_contour.emplace_back(bottomExtendedTip); //min
				_contour.emplace_back(bottom); //min
				_contour.emplace_back(bottomZPath); //max
				_contour.emplace_back(upperZPath); //max
				_contour.emplace_back(attachment.start); //min
				_contour.emplace_back(attachment.end.point);
				scales = { minSupportScale, minSupportScale, 1.0f, 1.0f,  minSupportScale, minSupportScale };
				_jointDir = Hix::Features::Extrusion::interpolatedJointNormals(_contour);
				_jointDir.back() = attachment.end.normal;
				return;
			}

		}
	}
	else
	{
		//vertical support path should be very simple, cone pointy bit(endtip), cone ending bit, bottom bit.
		auto attachment = calculateAttachmentInfo(_overhang.normal(), _overhang);

		auto& man = Hix::Application::ApplicationManager::getInstance().supportRaftManager();
		auto& supSetting = Hix::Application::ApplicationManager::getInstance().settings().supportSetting;
		for (auto pt : man.getSupportBasePts())
		{
			if (attachment.end.point.distanceToPoint(pt + QVector3D(0, 0, supSetting.raftThickness + supSetting.supportBaseHeight)) <= 0.001f)
			{
				return;
			}
		}

		//cone of support is always in the direction of the overhang normal to minimize contact between support and mesh
		auto coneNarrowNormal = attachment.end.normal * -1.0f;
		QVector3D coneWidePart = attachment.start +  (coneLength * coneNarrowNormal);
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
		_contour.emplace_back(attachment.start);
		_contour.emplace_back(attachment.end.point);
		_vertSeg = { raftAttachEnd, coneWidePart };
		auto botMult = 1.0f;
		if (setting.thickenFeet)
			botMult = SUPPORT_BOTTOM_MAX_MULT;
		scales = { botMult, botMult, 1.0f, 1.0f, minSupportScale, minSupportScale };
		_hasBasePt = true;
		_basePt = supportStart;
		_jointDir = Hix::Features::Extrusion::interpolatedJointNormals(_contour);
		_jointDir.back() = attachment.end.normal;
	}


	return;
}



QVector4D Hix::Support::VerticalSupportModel::getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)
{
	if (selectedFaces.find(faceItr) != selectedFaces.end())
	{
		return Hix::Render::Colors::SelectedFace;
	}
	else
	{
		return Hix::Render::Colors::Selected;
	}
}





void Hix::Support::VerticalSupportModel::generateMesh()
{
	std::vector<QVector3D> crossContour;
	auto mesh = new Mesh();

	auto radiusMax = Hix::Application::ApplicationManager::getInstance().settings().supportSetting.supportRadiusMax;
	generateSupportPath(_manager->supportBottom(), _scales);
	if (!_contour.empty())
	{
		crossContour = generateCircle(radiusMax, 16);
		Hix::Features::Extrusion::UniformScaler scaler(_scales);
		//auto jointDir = Hix::Features::Extrusion::interpolatedJointNormals(_contour);
		auto jointContours = Hix::Features::Extrusion::extrudeAlongPath(
			mesh, QVector3D(0, 0, 1), crossContour, _contour, _jointDir, &scaler);

		//create endcaps using joint contours;
		circleToTri(mesh, jointContours.front(), _contour.front(), true);
		circleToTri(mesh, jointContours.back(), _contour.back(), false);

	}
	setMesh(mesh);

}

