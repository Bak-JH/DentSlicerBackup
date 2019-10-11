#include "SupportRaftManager.h"
#include "feature/overhangDetect.h"
#include "VerticalSupportModel.h"
#include "CylindricalRaft.h"
#include "glmodel.h"

using namespace Hix::Support;


Hix::Support::SupportRaftManager::SupportRaftManager(GLModel* parent): _owner(parent)
{
}
Hix::Support::SupportRaftManager::~SupportRaftManager()
{
}

float Hix::Support::SupportRaftManager::supportRaftMinLength() const
{
	return scfg->raft_thickness + scfg->support_base_height;

}

float Hix::Support::SupportRaftManager::raftBottom() const
{
	return _owner->getMesh()->z_min() - supportRaftMinLength();
}
float Hix::Support::SupportRaftManager::supportBottom() const
{
	constexpr float FACTOR = 5.0f;
	auto raftBot = raftBottom();
	if (_raftType == SlicingConfiguration::RaftType::None)
	{
		return raftBot;
	}
	else
	{
		//to prevent z-fighting between raft and support bottoms
		return raftBot + scfg->raft_thickness/ FACTOR;
	}
}




void Hix::Support::SupportRaftManager::generateSuppAndRaft(SlicingConfiguration::SupportType supType, SlicingConfiguration::RaftType raftType)
{
	clear();
	_supportExist = true;
	_raftExist = true;
	_supportType = supType;
	_raftType = raftType;
	Hix::OverhangDetect::Detector detector;
	_overhangs = detector.detectOverhang(_owner->getMesh());
	generateSupport();
	generateRaft();


}

const std::vector<const Mesh*> Hix::Support::SupportRaftManager::getRaftSupportMeshes()const
{
	std::vector<const Mesh*> ptrs;
	ptrs.reserve(_supports.size() + 1);
	for (auto& each : _supports)
	{
		ptrs.emplace_back(each.first->getMesh());
	}
	if (_raft)
	{
		ptrs.emplace_back(_raft->getMesh());
	}
	return ptrs;
}

std::vector<QVector3D> Hix::Support::SupportRaftManager::getSupportBasePts() const
{
	std::vector<QVector3D> basePts;
	basePts.reserve(_supports.size());
	for (auto& pair : _supports)
	{
		auto editStatus = _pendingSupports.find(pair.first);
		if (editStatus == _pendingSupports.end() || editStatus->second == EditType::Added)
		{
			basePts.emplace_back(pair.first->getBasePt());
		}
	}
	return basePts;
}

void Hix::Support::SupportRaftManager::addSupport(const std::variant<VertexConstItr, FaceOverhang>& supportSpec)
{
	if (!_supportExist)
		return;
	switch (_supportType)
	{
	case SlicingConfiguration::SupportType::None:
		break;
	case SlicingConfiguration::SupportType::Vertical:
	{
		_overhangs.insert(supportSpec);
		auto newModel = new VerticalSupportModel(this, supportSpec);
		_supports[newModel] = std::unique_ptr<VerticalSupportModel>(newModel);
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
			_supports.erase(each.first);
			_overhangs.erase(each.first->overhang());
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
			_supports.erase(each.first);
			_overhangs.erase(each.first->overhang());

		}
		else
		{
			//enable hidden pending removes
			each.first->setEnabled(true);
			each.first->setHitTestable(true);

		}
	}
}

void Hix::Support::SupportRaftManager::generateSupport()
{

	switch (_supportType)
	{
	case SlicingConfiguration::SupportType::None:
		break;
	case SlicingConfiguration::SupportType::Vertical:
	{
		for (auto& each : _overhangs)
		{
			auto newModel = new VerticalSupportModel(this, each);
			_supports[newModel] = std::unique_ptr<VerticalSupportModel>(newModel);
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



void Hix::Support::SupportRaftManager::clear()
{
	_supportExist = false;
	_raftExist = false;
	_supports.clear();
	_pendingSupports.clear();
	_raft.reset();
}

GLModel* Hix::Support::SupportRaftManager::getModel()
{
	return _owner;
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
				each.first->setHitTestable(true);
			}
		}
		else
		{
			//disable supports ray cast
			for (auto& each : _supports)
			{
				each.first->setHitTestable(false);
			}
		}
	}

}
