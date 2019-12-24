#pragma once
#include "Mode.h"
#include <unordered_set>
#include "../../DentEngine/src/mesh.h"
class GLModel;
namespace Hix
{
	namespace Features
	{
		class PPShaderMode : virtual public Mode
		{
		public:
			PPShaderMode(const std::unordered_set<GLModel*>& selectedModels);
			void colorFaces(GLModel* model, const std::unordered_set <Hix::Engine3D::FaceConstItr > & faces);
			virtual ~PPShaderMode();
		protected:
			std::unordered_set<GLModel*> _subjects;
		};

	}
}

