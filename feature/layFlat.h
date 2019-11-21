#pragma once
#include <unordered_set>
#include "interfaces/SelectFaceMode.h"
#include "interfaces/PPShaderFeature.h"
class GLModel;

namespace Hix
{
	namespace Features
	{
		class LayFlat;
		class LayFlatMode : public SelectFaceMode, public PPShaderFeature
		{
		public:
			LayFlatMode(const std::unordered_set<GLModel*>& selectedModels);
			virtual ~LayFlatMode();
			void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;
			std::unique_ptr<LayFlat> applyLayFlat();

		private:
			const std::unordered_set<GLModel*> _models;
			QVector3D normal;
			bool isReady;
		};

		class LayFlat: public Feature//public SelectFaceFeature, public PPShaderFeature
		{
		public:
			LayFlat(const std::unordered_set<GLModel*>& selectedModels, QVector3D normal, bool isReady);
			void undo() override {} //TODO: undo
			virtual ~LayFlat();

		private:
			std::deque<GLModel*> _prevModels;
		};
	}
}