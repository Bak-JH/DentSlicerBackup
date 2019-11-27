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
			virtual ~AddSupport();
			void undo() override;

		private:
			SupportModel* _addedModel;
		};

		class RemoveSupport : public Feature
		{
		public:
			RemoveSupport(SupportModel* target);
			virtual ~RemoveSupport();
			void undo() override;

		private:
			SupportModel* _removedModel;
		};
		class SupportFeature: public Feature
		{
		public:
			SupportFeature(std::unordered_set<GLModel*>& selectedModel);
			virtual ~SupportFeature();
			void undo() override; //TODO: undo

		private:
			std::unordered_set<Hix::Memory::HetUniquePtr<SupportModel>> _prevSupports;
		};

		class SupportMode: public SelectFaceMode
		{
		public:
			SupportMode(const std::unordered_set<GLModel*>& selectedModels, QEntity* parent);
			virtual ~SupportMode();
			void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;
			void generateAutoSupport();

		private:
			std::unordered_set<GLModel*> _targetModels;
		};
	}
}

