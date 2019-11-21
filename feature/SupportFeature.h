#pragma once
#include "interfaces/SelectFaceMode.h"

namespace Hix
{
	namespace Features
	{
		class SupportFeature: public Hix::Features::Feature
		{
		public:
			SupportFeature();
			//void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;
			virtual ~SupportFeature();
			void undo() override {} //TODO: undo
		};

	}
}

