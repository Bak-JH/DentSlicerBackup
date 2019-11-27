#pragma once
#include <unordered_set>
#include "interfaces/SelectFaceMode.h"
#include "interfaces/PPShaderFeature.h"
#include "DentEngine/src/Bounds3D.h"
class GLModel;

namespace Hix
{
	namespace Features
	{
		class LayFlat: public Feature//public SelectFaceFeature, public PPShaderFeature
		{
		public:
			LayFlat(GLModel* selectedModel, QVector3D normal, bool isReady);
			void undo() override;
			virtual ~LayFlat();

		private:
			GLModel* _model;
			QQuaternion* _prevRotation;
			Hix::Engine3D::Bounds3D* _prevAabb;
		};



		class LayFlatMode : public SelectFaceMode, public PPShaderFeature
		{
		public:
			LayFlatMode(const std::unordered_set<GLModel*>& selectedModels);
			virtual ~LayFlatMode();
			void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;
			std::unique_ptr<Hix::Features::FeatureContainer> applyLayFlat();

		private:
			GLModel* _model = nullptr;
			bool isReady = false;
			std::unordered_map<GLModel*, QVector3D> _args;
		};
	}
}