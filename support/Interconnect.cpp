#include "Interconnect.h"
#include "ModelAttachedSupport.h"
using namespace Hix::Support;
//Hix::Support::Interconnect::Interconnect(SupportRaftManager* manager): SupportModel(manager)
//{
//}
Hix::Support::Interconnect::~Interconnect()
{
}
Interconnectable* Hix::Support::Interconnect::from() const
{
	return _connected[0];
}

Interconnectable* Hix::Support::Interconnect::to() const
{
	return _connected[1];
}

std::array<Interconnectable*, 2> Hix::Support::Interconnect::connected()
{
	return _connected;
}

bool Hix::Support::Interconnect::isConnectedTo(const std::unordered_set<const GLModel*>& models)
{
	for (auto& e : _connected)
	{
		auto modelAttached = dynamic_cast<ModelAttachedSupport*>(e);
		if (modelAttached)
		{
			if (models.find(&modelAttached->getAttachedListedModel()) != models.end())
				return true;
		}
	}
	return false;
}
