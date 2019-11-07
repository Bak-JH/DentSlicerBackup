#pragma once
#include "Feature.h"
#include <unordered_set>
#include "../../DentEngine/src/mesh.h"
class GLModel;
namespace Hix
{
	namespace Features
	{
		class PPShaderFeature: virtual public Hix::Features::Feature
		{
		public:
			PPShaderFeature();
			void colorFaces(GLModel* model, const std::unordered_set <Hix::Engine3D::FaceConstItr > & faces);
			virtual ~PPShaderFeature();
		protected:
		};

	}
}

