#pragma once
#include"Feature.h"
#include "../../DentEngine/src/mesh.h"
class GLModel;

namespace Qt3DRender
{
	class QRayCasterHit;
}
namespace Hix
{
	namespace Input
	{
		class MouseEventData;
	}
	namespace Features
	{
		class SelectFaceFeature : virtual public Hix::Features::Feature
		{
		public:
			virtual void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit) =0;
			SelectFaceFeature();
			virtual ~SelectFaceFeature();
		protected:
		};

	}
}

