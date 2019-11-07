#pragma once
#include <unordered_set>
#include "interfaces/SelectFaceFeature.h"
#include "interfaces/PPShaderFeature.h"
class GLModel;

namespace Hix
{
	namespace Features
	{
		class LayFlat: public SelectFaceFeature, public PPShaderFeature
		{
		public:
			LayFlat(const std::unordered_set<GLModel*>& selectedModels);
			void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;
			void generateLayFlat();
			virtual ~LayFlat();
		private:
			QVector3D _selectedNormal;
			bool _isReady = false;
		};
	}
}