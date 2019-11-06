#pragma once
#include <unordered_set>
#include "../../DentEngine/src/ContourBuilder.h"
#include "../../common/Task.h"

class GLModel;
namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
	}

	namespace Features
	{
		namespace Cut
		{
			class ZAxialCut
			{
				
			public:
				//should cut keep both halves
				enum Result
				{
					KeepBoth,
					KeepTop,
					KeepBottom
				};
				ZAxialCut(GLModel* subject, float cuttingPlane, Result option = Result::KeepBoth);
				void doChildrenRecursive(GLModel* subject, float cuttingPlane, Result option);
			private:
				//top, bottom pair
				std::unordered_map<GLModel*, std::pair<GLModel*, GLModel*>> _divisionMap;
				std::unordered_set<GLModel*> _topChildren;
				std::unordered_set<GLModel*> _botChildren;
				float _cuttingPlane;
			};
		}
	}
}
