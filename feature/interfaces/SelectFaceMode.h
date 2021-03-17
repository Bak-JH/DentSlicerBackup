#pragma once
#include"Mode.h"
#include "../../Mesh/mesh.h"
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
		class SelectFaceMode : virtual public Hix::Features::Mode
		{
		public:
			virtual void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit) =0;
			SelectFaceMode();
			virtual ~SelectFaceMode();
		protected:
		};

	}
}

