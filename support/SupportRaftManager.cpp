#include "SupportRaftManager.h"
#include "feature/overhangDetect.h"
#include "VerticalSupportModel.h"
#include "CylindricalRaft.h"
#include "glmodel.h"
#include <functional>
#include "../Mesh/BVH.h"
#include "../Mesh/MTRayCaster.h"
#include "../common/Debug.h"
#include "../application/ApplicationManager.h"
#include "feature/SupportFeature.h"
#include "ModelAttachedSupport.h"
#include "Interconnect.h"
#include "DiagInterconnect.h"
#include "../feature/Shapes2D.h"
using namespace Qt3DCore;
using namespace Hix::Support;
using namespace Hix::Shapes2D;
Hix::Support::SupportRaftManager::SupportRaftManager(): _root(new QEntity())
{
}
void Hix::Support::SupportRaftManager::initialize(Qt3DCore::QEntity* parent)
{
	_root->setParent(parent);
	_root->setEnabled(true);
}
Hix::Support::SupportRaftManager::~SupportRaftManager()
{
	//qobjects will delete themselves
	_raft.release();
	for (auto& m : _supports)
	{
		m.second.release();
	}
}

float Hix::Support::SupportRaftManager::supportRaftMinLength()
{
	auto& setting = Hix::Application::ApplicationManager::getInstance().settings().supportSetting;
	return setting.raftThickness + setting.supportBaseHeight;
}

float Hix::Support::SupportRaftManager::raftBottom()
{
	constexpr float BOTTOM = -0.0001f;
	return BOTTOM;
}

float Hix::Support::SupportRaftManager::supportBottom()
{
	constexpr float FACTOR = 5.0f;
	//to prevent z-fighting between raft and support bottoms
	auto& setting = Hix::Application::ApplicationManager::getInstance().settings().supportSetting;
	return setting.raftThickness/ FACTOR;
}



std::vector<QVector3D> Hix::Support::SupportRaftManager::getSupportBasePts() const
{
	std::vector<QVector3D> basePts;
	basePts.reserve(_supports.size());
	for (auto& each : _supports)
	{
		auto baseSupport = dynamic_cast<BaseSupport*>(each.first);
		if (baseSupport && baseSupport->hasBasePt())
		{
			basePts.emplace_back(baseSupport->getBasePt());
		}
	}
	return basePts;
}

std::vector<std::unique_ptr<SupportModel>> Hix::Support::SupportRaftManager::createInterconnects(const std::array<SupportModel*, 2>& models)
{
	auto& sett = Hix::Application::ApplicationManager::getInstance().settings().supportSetting;
	constexpr float CNT_ANGLE = 45;
	//constexpr float CNT_HEIGHT_MULT = std::tanf(CNT_ANGLE); due to legacy reasons, just hard code tanf
	constexpr float CNT_HEIGHT_MULT = 1.0f;

	auto connType = sett.interconnectType;

	std::vector<std::unique_ptr<SupportModel>> itcs;
	auto pillar0 = dynamic_cast<ContourModel*>(models[0])->verticalSegment().value();
	auto pillar1 = dynamic_cast<ContourModel*>(models[1])->verticalSegment().value();
	auto xyDist = pillar0[0].distanceToPoint(pillar1[0]);
	auto zDist = xyDist * CNT_HEIGHT_MULT;
	auto zMax = std::min(pillar0[1].z(), pillar1[1].z()) - zDist;
	auto zMin = std::max(pillar0[0].z(), pillar1[0].z());

	auto xyFrom = pillar0[0];
	auto xyTo = pillar1[0];

	//true: from->to, false to->from
	bool flip = false;

	//for raycasting
	auto circle = generateCircle(sett.supportRadiusMax, 4);
	auto circleF = circle;
	{
		QVector3D from(xyFrom);
		QVector3D to(xyTo);
		from.setZ(0);
		to.setZ(zDist);
		rotateContour(circle, QQuaternion::rotationTo(to - from, QVector3D(0, 0, 1)));

	}
	{
		QVector3D from(xyFrom);
		QVector3D to(xyTo);
		from.setZ(zDist);
		to.setZ(0);
		rotateContour(circleF, QQuaternion::rotationTo(from-to, QVector3D(0, 0, 1)));
	}
	for (float currZ = zMin; currZ < zMax; currZ += zDist)
	{
		switch (connType)
		{
		case Hix::Settings::SupportSetting::InterconnectType::None:
			break;
		//case Hix::Settings::SupportSetting::InterconnectType::Automatic:
		//	break;
		case Hix::Settings::SupportSetting::InterconnectType::Cross:
		{
			QVector3D from(xyFrom);
			QVector3D to(xyTo);
			auto fromM(models[0]);
			auto toM(models[1]);
			auto* curCircle = &circleF;
			if (!flip)
			{
				std::swap(from, to);
				std::swap(fromM, toM);
				curCircle = &circle;
			}
			from.setZ(currZ);
			to.setZ(currZ + zDist);
			if(_rayCaster->checkCSecInteresect(from, to, *curCircle))
				itcs.push_back(std::unique_ptr<SupportModel>(new DiagInterconnect(this, { from, to }, { fromM, toM })));
		}
		//continue to zigzag
		case Hix::Settings::SupportSetting::InterconnectType::ZigZag:
		{
			QVector3D from(xyFrom);
			QVector3D to(xyTo);
			auto fromM(models[0]);
			auto toM(models[1]);
			auto* curCircle = &circle;
			if (flip)
			{
				std::swap(from, to);
				std::swap(fromM, toM);
				curCircle = &circleF;
			}
			from.setZ(currZ);
			to.setZ(currZ + zDist);
			if (_rayCaster->checkCSecInteresect(from, to, *curCircle))
				itcs.push_back(std::unique_ptr<SupportModel>(new DiagInterconnect(this, { from, to }, { fromM, toM})));
		}
			break;
		default:
			break;
		}
		flip = !flip;
	}
	return itcs;

}

std::unique_ptr<SupportModel> Hix::Support::SupportRaftManager::createSupport(const OverhangDetect::Overhang& overhang)
{
	SupportModel* newModel = nullptr;
	switch (Hix::Application::ApplicationManager::getInstance().settings().supportSetting.supportType)
	{
	//case Hix::Settings::SupportSetting::SupportType::None:
	//	break;
	case Hix::Settings::SupportSetting::SupportType::Vertical:
	{
		newModel = dynamic_cast<SupportModel*>(new VerticalSupportModel(this, overhang));
	}
	break;
	}
	return std::unique_ptr<SupportModel>(newModel);
}

SupportModel* Hix::Support::SupportRaftManager::addSupport(std::unique_ptr<SupportModel> target)
{
	auto key = target.get();
	_supports.insert(std::make_pair(key,std::move(target)));
	addToModelMap(key);
	key->setEnabled(true);
	if (supportEditMode() == Support::EditMode::Manual)
	{
		key->setHitTestable(true);
		key->setHoverable(true);

	}
	return key;
}

SupportModel* Hix::Support::SupportRaftManager::addInterconnect(const std::array<SupportModel*, 2>& pts)
{
	return nullptr;
}


void Hix::Support::SupportRaftManager::addToModelMap(SupportModel* support)
{
	auto modelAttached = dynamic_cast<ModelAttachedSupport*>(support);
	if (modelAttached)
	{
		auto model = &modelAttached->getAttachedModel();
		auto set = _modelSupportMap.find(model);
		if (set == _modelSupportMap.end())
		{
			std::unordered_set<ModelAttachedSupport*> newSet{ modelAttached };
			_modelSupportMap.insert(std::make_pair(model, std::move(newSet)));
		}
		else
		{
			set->second.insert(modelAttached);
		}
	}
}

void Hix::Support::SupportRaftManager::removeFromModelMap(SupportModel* support)
{

	auto modelAttached = dynamic_cast<ModelAttachedSupport*>(support);
	if (modelAttached)
	{
		auto model = &modelAttached->getAttachedModel();
		auto set = _modelSupportMap.find(model);
		if (set != _modelSupportMap.end())
		{
			set->second.erase(modelAttached);
			if (set->second.empty())
			{
				_modelSupportMap.erase(model);
			}
		}
	}
}

std::unique_ptr<SupportModel> Hix::Support::SupportRaftManager::removeSupport(SupportModel* e)
{
	std::unique_ptr<SupportModel> result;
	e->setEnabled(false);
	e->setHitTestable(false);
	e->setHoverable(false);
	auto found = _supports.find(e);
	if (found != _supports.end())
	{
		result = std::move(found->second);
	}
	removeFromModelMap(e);
	_supports.erase(e);
	return result;
}

bool Hix::Support::SupportRaftManager::supportsEmpty()const
{
	return _supports.empty();
}

bool Hix::Support::SupportRaftManager::modelHasSupport(const GLModel* listed) const
{
	std::unordered_set<const GLModel*> models;
	listed->getChildrenModels(models);
	models.insert(listed);

	for (auto& model : models)
	{
		if (_modelSupportMap.find(model) != _modelSupportMap.cend())
			return true;
	}
	return false;
}

RaftModel* Hix::Support::SupportRaftManager::generateRaft()
{
	auto basePts = getSupportBasePts();
	_raft = std::make_unique<CylindricalRaft>(this, basePts);
	return _raft.get();
} 

RaftModel* Hix::Support::SupportRaftManager::addRaft(std::unique_ptr<RaftModel> raft)
{
	_raft = std::move(raft);
	_raft->setParent(&rootEntity());
	return _raft.get();
}

std::unique_ptr<RaftModel> Hix::Support::SupportRaftManager::removeRaft()
{
	if(_raft)
		_raft->setParent((QNode*)nullptr);
	return std::move(_raft);
}

Hix::OverhangDetect::Overhangs Hix::Support::SupportRaftManager::detectOverhang(const GLModel& listed)
{
	std::unordered_set<const GLModel*> models;
	listed.getChildrenModels(models);
	Hix::OverhangDetect::Overhangs overhangs;
	models.insert(&listed);
	QVector3D straightDown(0, 0, -1);

	for (auto model : models)
	{
		Hix::OverhangDetect::Detector detector;
		auto eachOverhangs =   detector.detectOverhang(model->getMesh());
		overhangs.insert(overhangs.end(), eachOverhangs.begin(), eachOverhangs.end());

	}
	//raycaster for support generation
	prepareRaycaster(listed);
	return overhangs;
}

void  Hix::Support::SupportRaftManager::clear(const GLModel& model)
{
	std::unordered_set<const GLModel*> models;

	model.getChildrenModels(models);
	models.insert(&model);
	clearImpl(models);
}


void Hix::Support::SupportRaftManager::clearImpl(const std::unordered_set<const GLModel*>& models)
{
	for (auto curr = _supports.begin(); curr != _supports.end();)
	{
		auto attachedSupport = dynamic_cast<ModelAttachedSupport*>(curr->second.get());
		if (attachedSupport)
		{
			auto ptr = &attachedSupport->getAttachedModel();
			if (models.find(ptr) != models.end())
			{
				curr = _supports.erase(curr);
			}
			else
			{
				++curr;
			}
		}
	}
}

void Hix::Support::SupportRaftManager::prepareRaycasterSelected()
{
	std::unordered_set<const GLModel*> models;

	for (auto& e : Hix::Application::ApplicationManager::getInstance().partManager().selectedModels())
	{
		e->getChildrenModels(models);
		models.emplace(e);
	}
	_rayCaster.reset(new MTRayCaster());
	_rayCaster->addAccelerator(new Hix::Engine3D::BVH(models));
}



void Hix::Support::SupportRaftManager::prepareRaycaster(const GLModel& model)
{
	_rayCaster.reset( new MTRayCaster());
	_rayCaster->addAccelerator(new Hix::Engine3D::BVH(model));
}

RaftModel* Hix::Support::SupportRaftManager::raftModel() 
{
	return _raft.get();
}

const RaftModel* Hix::Support::SupportRaftManager::raftModel() const
{
	return _raft.get();
}

std::vector<const Hix::Render::SceneEntity*> Hix::Support::SupportRaftManager::supportModels() const
{
	std::vector<const Hix::Render::SceneEntity*> entities;
	entities.reserve(_supports.size());
	for (auto& each : _supports)
	{
		entities.emplace_back(each.first);
	}
	return entities;
}

std::vector<SupportModel*> Hix::Support::SupportRaftManager::modelAttachedSupports(const std::unordered_set<GLModel*>& models)const
{
	std::vector<SupportModel*> supps;
	std::unordered_set<const GLModel*> childs;

	for (auto model : models)
	{
		model->getChildrenModels(childs);
		childs.insert(model);
	}
	for (auto curr = _supports.cbegin(); curr != _supports.cend(); ++curr)
	{
		auto attachedSupport = dynamic_cast<ModelAttachedSupport*>(curr->first);
		if (attachedSupport)
		{
			auto ptr = &attachedSupport->getAttachedModel();
			if (childs.find(ptr) != childs.end())
			{
				supps.push_back(curr->first);
			}
		}
	}
	return supps;

}

std::vector<SupportModel*> Hix::Support::SupportRaftManager::interconnects() const
{
	std::vector<SupportModel*> supps;
	for (auto curr = _supports.cbegin(); curr != _supports.cend(); ++curr)
	{
		auto conn = dynamic_cast<Interconnect*>(curr->first);
		if (conn)
		{
			supps.push_back(curr->first);

		}
	}
	return supps;
}

Qt3DCore::QEntity& Hix::Support::SupportRaftManager::rootEntity()
{
	return *_root;
}

size_t Hix::Support::SupportRaftManager::supportCount() const
{
	return _supports.size();
}

RayCaster& Hix::Support::SupportRaftManager::supportRaycaster()
{
	return *_rayCaster.get();
}

std::vector<std::array<SupportModel*, 2>> Hix::Support::SupportRaftManager::interconnectPairs() const
{
	std::vector<std::array<SupportModel*, 2>> pairs;
	auto& setting = Hix::Application::ApplicationManager::getInstance().settings().supportSetting;

	//grab all supports that can be connected
	std::vector<ContourModel*> candidates;
	for (auto itr = _supports.begin(); itr != _supports.end(); ++itr)
	{
		auto curr = itr->second.get();
		auto connectible = dynamic_cast<ContourModel*>(curr);
		if (connectible)
		{
			auto verticalPath = connectible->verticalSegment();
			if (verticalPath)
			{
				candidates.emplace_back(connectible);
			}
		}
	}
	//create pairs
	if (candidates.empty())
		return pairs;
	auto last = candidates.end() - 1;
	for (auto itr = candidates.begin(); itr != last; ++itr)
	{
		auto from = (*itr)->verticalSegment().value()[0];
		for (auto next = itr + 1; next != candidates.end(); ++next)
		{
			auto to = (*next)->verticalSegment().value()[0];
			if (from.distanceToPoint(to) < setting.maxConnectDistance)
			{
				pairs.emplace_back( std::array{dynamic_cast<SupportModel*>(*itr), dynamic_cast<SupportModel*>(*next) });
			}
		}
	}
	return pairs;
}




bool Hix::Support::SupportRaftManager::supportActive() const
{
	return !_supports.empty();
}

bool Hix::Support::SupportRaftManager::raftActive() const
{
	return _raft.get() != nullptr;
}



Hix::Support::EditMode Hix::Support::SupportRaftManager::supportEditMode() const
{
	return _supportEditMode;
}

void Hix::Support::SupportRaftManager::setSupportEditMode(Hix::Support::EditMode mode)
{
	if (_supportEditMode != mode)
	{
		_supportEditMode = mode;
		if (_supportEditMode == EditMode::Manual)
		{
			//enable supports ray cast
			for (auto& each : _supports)
			{
				each.first->setHitTestable(true);
				each.first->setHoverable(true);
				
			}
		}
		else
		{
			//disable supports ray cast
			for (auto& each : _supports)
			{
				each.first->setHitTestable(false);
				each.first->setHoverable(false);
			}
		}
	}

}
