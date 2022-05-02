#pragma once
#include <unordered_set>
#include "../slice/ContourBuilder.h"
#include "feature/interfaces/FlushSupport.h"

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
			enum KeepType
			{
				KeepBoth,
				KeepTop,
				KeepBottom
			};
			class ZAxialCut :public Hix::Features::FeatureContainerFlushSupport
			{
				
			public:

				ZAxialCut(std::unordered_set<GLModel*>& subject, float cuttingPlane, KeepType keep, bool keepName = false);
				ZAxialCut(GLModel* subject, float cuttingPlane, KeepType keep, bool keepName = false, bool seperate = true);

				void doChildrenRecursive(GLModel* subject, float cuttingPlane);
				std::unordered_set<GLModel*>& upperModels();
				std::unordered_set<GLModel*>& lowerModels();
			protected:
				void runImpl()override;
			private:
				//top, bottom pair
				std::unordered_set<GLModel*> _subjects;
				KeepType _keep;
				float _cuttingPlane;
				bool _keepName;
				bool _seperate;
			};
		}
	}
}
