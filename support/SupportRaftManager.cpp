#include "SupportRaftManager.h"
#include "../feature/overhangDetect.h"
#include "VerticalSupportModel.h"
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

float Hix::Support::SupportRaftManager::supportRaftBottom() const
{
	return _owner->getMesh()->z_min() - supportRaftMinLength();
}

void Hix::Support::SupportRaftManager::generateSuppAndRaft(SlicingConfiguration::SupportType supType, SlicingConfiguration::RaftType raftType)
{
	_supportExist = true;
	_raftExist = true;

	_supportType = supType;
	_raftType = raftType;
	switch (_supportType)
	{
	case SlicingConfiguration::SupportType::None:
		break;
	case SlicingConfiguration::SupportType::Vertical:
	{
		auto overhangs = Hix::OverhangDetect::detectOverhang(_owner->getMesh());
		for (auto& each : overhangs)
		{
			if (each.index() == 0)
			{
				_vertexSupports[std::get<0>(each)] = std::make_unique<VerticalSupportModel>(this, each);
			}
			else
			{
				_faceSupports[std::get<1>(each).first] = std::make_unique<VerticalSupportModel>(this, each);
			}
		}
	}
		break;
	default:
		break;
	}
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
		if (supportSpec.index() == 0)
		{
			_vertexSupports[std::get<0>(supportSpec)] = std::make_unique<VerticalSupportModel>(this, supportSpec);
		}
		else
		{
			_faceSupports[std::get<1>(supportSpec).first] = std::make_unique<VerticalSupportModel>(this, supportSpec);
		}
	}
	break;
	default:
		break;
	}
}

void Hix::Support::SupportRaftManager::removeSupport(const std::variant<VertexConstItr, FaceOverhang>& supportSpec)
{
	if (supportSpec.index() == 0)
	{
		_vertexSupports.erase(std::get<0>(supportSpec));
	}
	else
	{
		_faceSupports.erase(std::get<1>(supportSpec).first);
	}
}

void Hix::Support::SupportRaftManager::clear()
{
	_vertexSupports.clear();
	_faceSupports.clear();
}




bool Hix::Support::SupportRaftManager::supportActive() const
{
	return _supportExist;
}

bool Hix::Support::SupportRaftManager::raftActive() const
{
	return _raftExist;
}



Hix::Support::SupportEditMode Hix::Support::SupportRaftManager::supportEditMode() const
{
	return _supportEditMode;
}

void Hix::Support::SupportRaftManager::setSupportEditMode(Hix::Support::SupportEditMode mode)
{
	_supportEditMode = mode;
}
