#pragma once
#include <unordered_set>
#include "interfaces/SelectFaceMode.h"
#include "interfaces/PPShaderFeature.h"
class GLModel;

namespace Hix
{
	namespace Features
	{
		//struct LayFlatArg
		//{
		//	QVector3D normal;
		//	Hix::Bounds3D* aabb;
		//};


		class LayFlat: public Feature//public SelectFaceFeature, public PPShaderFeature
		{
		public:
			LayFlat(GLModel* selectedModel, QVector3D normal, bool isReady);
			void undo() override;
			virtual ~LayFlat();

		private:
			GLModel* _model;
			QQuaternion* _prevRotation;
		};



		class LayFlatMode : public SelectFaceMode, public PPShaderFeature, public Mode
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