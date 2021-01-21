#pragma once
#include <unordered_set>
#include "interfaces/SelectFaceMode.h"
#include "interfaces/PPShaderMode.h"
#include "render/Bounds3D.h"
#include "interfaces/FlushSupport.h"
#include "interfaces/DialogedMode.h"

class GLModel;

namespace Hix
{
	namespace Features
	{
		class LayFlat: public Feature
		{
		public:
			LayFlat(GLModel* selectedModel, QVector3D normal);
			virtual ~LayFlat();

		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;

		private:
			QVector3D _normal;
			GLModel* _model;
			QMatrix4x4 _prevMatrix;
			Hix::Engine3D::Bounds3D _prevAabb;
		};



		class LayFlatMode : public SelectFaceMode, public PPShaderMode, public DialogedMode
		{
		public:
			LayFlatMode();
			virtual ~LayFlatMode();
			void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;
			void applyButtonClicked()override;

		private:
			std::unordered_map<GLModel*, QVector3D> _args;
		};
	}
}