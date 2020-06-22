#pragma once
#include "SupportModel.h"
#include <array>
class GLModel;
namespace Hix
{
	namespace Support
	{
		//class Interconnectable
		//{

		//};
		class SupportRaftManager;
		typedef SupportModel Interconnectable;
		class Interconnect: public virtual SupportModel
		{
		public:
			using SupportModel::SupportModel;
			//Interconnect(SupportRaftManager* manager);
			virtual ~Interconnect();
			Interconnectable* from()const;
			Interconnectable* to()const;
			std::array<Interconnectable*, 2> connected();
			bool isConnectedTo(const std::unordered_set<const GLModel*>& models);
		protected:
			std::array<Interconnectable*, 2> _connected;
		};
	}
}