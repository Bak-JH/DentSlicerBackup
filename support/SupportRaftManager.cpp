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
using namespace Qt3DCore;
using namespace Hix::Support;
using namespace Qt3DCore;

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

SupportModel* Hix::Support::SupportRaftManager::addSupport(const OverhangDetect::Overhang& overhang)
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
	addSupport(std::unique_ptr<SupportModel>(newModel));
	return newModel;
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
