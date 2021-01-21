#pragma once
#include "Mode.h"
#include <unordered_set>
#include "../../Mesh/mesh.h"
class GLModel;
namespace Hix
{
	namespace Features
	{
		class PPShaderMode : virtual public Mode
		{
		public:
			PPShaderMode();
			PPShaderMode(const std::unordered_set<GLModel*>& selectedModels);
			void initPPShader(GLModel& model);
			void colorFaces(GLModel* model, const std::unordered_set <Hix::Engine3D::FaceConstItr > & faces);
			virtual ~PPShaderMode();
		private:
			std::unordered_set<GLModel*> _subjects;
		};

	}
}

