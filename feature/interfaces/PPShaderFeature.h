#pragma once
#include "Mode.h"
#include <unordered_set>
#include "../../DentEngine/src/mesh.h"
class GLModel;
namespace Hix
{
	namespace Features
	{
		class PPShaderFeature: virtual public Mode
		{
		public:
			PPShaderFeature(const std::unordered_set<GLModel*>& selectedModels);
			void colorFaces(GLModel* model, const std::unordered_set <Hix::Engine3D::FaceConstItr > & faces);
			virtual ~PPShaderFeature();
		protected:
			std::unordered_set<GLModel*> _subjects;
		};

	}
}

