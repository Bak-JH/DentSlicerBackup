#pragma once
#include "interfaces/SelectFaceMode.h"
#include "support/SupportModel.h"
#include "support/SupportRaftManager.h"
#include "DentEngine/src/configuration.h"

namespace Hix
{
	namespace Features
	{
		using namespace Support;
		class AddSupport : public Feature
		{
		public:
			AddSupport(const Hix::Engine3D::FaceConstItr face, QVector3D point);
			AddSupport(const OverhangDetect::Overhang& overhang);
			virtual ~AddSupport();
			void undo() override;

		private:
			SupportModel* _addedModel;
		};

		class RemoveSupport : public Feature
		{
		public:
			RemoveSupport(SupportModel* target);
			RemoveSupport(std::unique_ptr<SupportModel>&& target);
			virtual ~RemoveSupport();
			void undo() override;

		private:
			std::unique_ptr<SupportModel> _removedModel;
		};

		class SupportMode: public SelectFaceMode
		{
		public:
			SupportMode(const std::unordered_set<GLModel*>& selectedModels, QEntity* parent);
			virtual ~SupportMode();
			void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;
			std::unique_ptr<Hix::Features::FeatureContainer> generateAutoSupport();
			std::unique_ptr<Hix::Features::FeatureContainer> clearSupport();
			void removeSupport(SupportModel* target);

		private:
			std::unordered_set<GLModel*> _targetModels;
		};
	}
}

