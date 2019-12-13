#pragma once
#include <unordered_set>
#include "../../DentEngine/src/ContourBuilder.h"
#include "../../common/Task.h"
#include "feature/interfaces/Feature.h"

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
			class ZAxialCut : public Feature
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
				void undo()override;
				void redo()override;
			private:
				//top, bottom pair
				Hix::Features::FeatureContainer* _container;
				int top_no, bot_no;
				float _cuttingPlane;
			};
		}
	}
}
