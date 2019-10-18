#include "SupportRaftManager.h"
#include "feature/overhangDetect.h"
#include "VerticalSupportModel.h"
#include "CylindricalRaft.h"
#include "glmodel.h"
#include <functional>
using namespace Hix::Support;
using namespace Hix::Memory;

Hix::Support::SupportRaftManager::SupportRaftManager()
{
}
void Hix::Support::SupportRaftManager::initialize(Qt3DCore::QEntity* parent)
{
	_root.setParent(parent);
	_root.setEnabled(true);
}
Hix::Support::SupportRaftManager::~SupportRaftManager()
{
	_root.setParent((QNode*)nullptr);
}

float Hix::Support::SupportRaftManager::supportRaftMinLength()
{
	return scfg->raft_thickness + scfg->support_base_height;
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
	return scfg->raft_thickness/ FACTOR;
}




void Hix::Support::SupportRaftManager::autoGenSuppRaft(const GLModel& model, SlicingConfiguration::SupportType supType, SlicingConfiguration::RaftType raftType)
{
	clear();
	_supportExist = true;
	_raftExist = true;
	_supportType = supType;
	_raftType = raftType;
	autoGenSuppRaftRecursive(model);
	generateRaft();

}



void Hix::Support::SupportRaftManager::autoGenSuppRaftRecursive(const GLModel& model)
{
	for (auto childNode : model.childNodes())
	{
		auto mainModel = dynamic_cast<GLModel*>(childNode);
		if (mainModel)
		{
			autoGenSuppRaftRecursive(*mainModel);
		}
	}
	generateSupport(model);


}



std::vector<QVector3D> Hix::Support::SupportRaftManager::getSupportBasePts() const
{
	std::vector<QVector3D> basePts;
	basePts.reserve(_supports.size());
	for (auto& each : _supports)
	{
		auto editStatus = _pendingSupports.find(each.get());
		if (editStatus == _pendingSupports.end() || editStatus->second == EditType::Added)
		{
			auto baseSupport = dynamic_cast<BaseSupport*>(each.get());
			if (baseSupport)
			{
				basePts.emplace_back(baseSupport->getBasePt());
			}
		}
	}
	return basePts;
}

void Hix::Support::SupportRaftManager::addSupport(const OverhangDetect::Overhang& overhang)
{
	if (!_supportExist)
		return;
	switch (_supportType)
	{
	case SlicingConfiguration::SupportType::None:
		break;
	case SlicingConfiguration::SupportType::Vertical:
	{
		auto newModel = new VerticalSupportModel(this, overhang);
		_supports.emplace(toUnique(dynamic_cast<SupportModel*>(newModel)));
		//since addition only happens in edit mode
		newModel->setHitTestable(true);
		_pendingSupports[newModel] = EditType::Added;
	}
	break;
	default:
		break;
	}
}

void Hix::Support::SupportRaftManager::removeSupport(SupportModel* e)
{
	e->setEnabled(false);
	e->setHitTestable(false);
	_pendingSupports[e] = EditType::Removed;
}

void Hix::Support::SupportRaftManager::applyEdits()
{
	//remove all pending removes
	for (auto& each : _pendingSupports)
	{
		if (each.second == EditType::Removed)
		{
			_supports.erase(toDummy(each.first));
		}
	}
}

void Hix::Support::SupportRaftManager::cancelEdits()
{
	//remove all pending adds
	for (auto& each : _pendingSupports)
	{
		if (each.second == EditType::Added)
		{
			_supports.erase(toDummy(each.first));
		}
		else
		{
			//enable hidden pending removes
			each.first->setEnabled(true);
			each.first->setHitTestable(true);

		}
	}
}

void Hix::Support::SupportRaftManager::generateSupport(const GLModel& model)
{
	Hix::OverhangDetect::Detector detector;
	auto overhangs = detector.detectOverhang(model.getMesh());
	switch (_supportType)
	{
	case SlicingConfiguration::SupportType::None:
		break;
	case SlicingConfiguration::SupportType::Vertical:
	{
		for (auto& each : overhangs)
		{
			auto newModel = new VerticalSupportModel(this, each);
			_supports.emplace(toUnique(dynamic_cast<SupportModel*>(newModel)));
		}
	}
	break;
	default:
		break;
	}

}

void Hix::Support::SupportRaftManager::generateRaft()
{

	switch (_raftType)
	{
	case SlicingConfiguration::RaftType::None:
		break;
	case SlicingConfiguration::RaftType::General:
	{
		auto basePts = getSupportBasePts();
		_raft = std::make_unique<CylindricalRaft>(this, basePts);
		break;
	}
	default:
		break;
	}
}
size_t Hix::Support::SupportRaftManager::clear(const GLModel& model)
{
	size_t count = 0;
	for (auto curr = _supports.begin(); curr != _supports.end();)
	{
		auto attachedSupport = dynamic_cast<ModelAttachedSupport*>(curr->get());
		if (attachedSupport)
		{
			if (&model == &attachedSupport->getAttachedModel())
			{
				curr = _supports.erase(curr);
				++count;
			}
			else
			{
				++curr;
			}
		}
	}
	if (_supports.size() == 0)
	{
		clear();
	}
	return count;
}

void Hix::Support::SupportRaftManager::clear(const std::unordered_set<const GLModel*>& models)
{
	for (auto curr = _supports.begin(); curr != _supports.end();)
	{
		auto attachedSupport = dynamic_cast<ModelAttachedSupport*>(curr->get());
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
	if (_supports.size() == 0)
	{
		clear();
	}
}

const Hix::Render::SceneEntity* Hix::Support::SupportRaftManager::raftModel() const
{
	return _raft.get();
}

std::vector<std::reference_wrapper<const Hix::Render::SceneEntity>> Hix::Support::SupportRaftManager::supportModels() const
{
	std::vector<std::reference_wrapper<const Hix::Render::SceneEntity>> entities;
	entities.reserve(_supports.size());
	for (auto& each : _supports)
	{
		entities.emplace_back(dynamic_cast<const Hix::Render::SceneEntity&>(*each.get()));
	}
	return entities;
}


void Hix::Support::SupportRaftManager::clear()
{
	_supportExist = false;
	_raftExist = false;
	_supports.clear();
	_pendingSupports.clear();
	_raft.reset();
}

Qt3DCore::QEntity& Hix::Support::SupportRaftManager::rootEntity()
{
	return _root;
}



bool Hix::Support::SupportRaftManager::supportActive() const
{
	return _supportExist;
}

bool Hix::Support::SupportRaftManager::raftActive() const
{
	return _raftExist;
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
				each->setHitTestable(true);
			}
		}
		else
		{
			//disable supports ray cast
			for (auto& each : _supports)
			{
				each->setHitTestable(false);
			}
		}
	}

}
