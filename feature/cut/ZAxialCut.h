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
				ZAxialCut(GLModel* subject, float cuttingPlane);

				void doChildrenRecursive(GLModel* subject, float cuttingPlane);
				void undo()override;
				void redo()override;
				std::unordered_set<GLModel*>& upperModels();
				std::unordered_set<GLModel*>& lowerModels();

			private:
				//top, bottom pair
				Hix::Features::FeatureContainer* _container;
				int top_no, bot_no;
				float _cuttingPlane;
				std::unordered_set<GLModel*> _upperModels;
				std::unordered_set<GLModel*> _lowerModels;

			};
		}
	}
}
