#pragma once
#include <unordered_set>
#include "../../DentEngine/src/ContourBuilder.h"
#include "feature/interfaces/FlushSupport.h"
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
			class ZAxialCut :public Hix::Features::FeatureContainerFlushSupport
			{
				
			public:
				ZAxialCut(GLModel* subject, float cuttingPlane);

				void doChildrenRecursive(GLModel* subject, float cuttingPlane);
				std::unordered_set<GLModel*>& upperModels();
				std::unordered_set<GLModel*>& lowerModels();

			private:
				//top, bottom pair
				int top_no, bot_no;
				float _cuttingPlane;
				std::unordered_set<GLModel*> _upperModels;
				std::unordered_set<GLModel*> _lowerModels;

			};
			void MeshZCutAway(Hix::Engine3D::Mesh& mesh, float cuttingPlane, bool keepTop);
		}
	}
}
