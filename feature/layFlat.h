#pragma once
#include <unordered_set>
#include "interfaces/SelectFaceMode.h"
#include "interfaces/PPShaderMode.h"
#include "DentEngine/src/Bounds3D.h"
#include "interfaces/FlushSupport.h"
class GLModel;

namespace Hix
{
	namespace Features
	{
		class LayFlat: public Feature
		{
		public:
			LayFlat(GLModel* selectedModel, QVector3D normal, bool isReady);
			void undo()override;
			void redo()override;
			virtual ~LayFlat();

		private:
			GLModel* _model;
			QMatrix4x4 _prevMatrix;
			Hix::Engine3D::Bounds3D _prevAabb;
		};



		class LayFlatMode : public SelectFaceMode, public PPShaderMode
		{
		public:
			LayFlatMode(const std::unordered_set<GLModel*>& selectedModels);
			virtual ~LayFlatMode();
			void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;
			FeatureContainerFlushSupport* applyLayFlat();

		private:
			bool isReady = false;
			std::unordered_map<GLModel*, QVector3D> _args;
		};
	}
}