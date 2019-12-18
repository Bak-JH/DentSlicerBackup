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
			void undo()override;
			void redo()override;

		private:
			SupportModel* _removedModel;
			SupportModel* _addedModel;
		};

		class RemoveSupport : public Feature
		{
		public:
			RemoveSupport(SupportModel* target);
			virtual ~RemoveSupport();
			void undo()override;
			void redo()override;

		private:
			SupportModel* _removedModel;
			SupportModel* _addedModel;
		};

		class AddRaft : public Feature
		{
		public: 
			AddRaft();
			virtual ~AddRaft();
			void undo()override;
			void redo()override;

		private:
			RaftModel* _addedRaft;
			RaftModel* _deletedRaft;
		};
		
		class RemoveRaft : public Feature
		{
		public: 
			RemoveRaft();
			virtual ~RemoveRaft();
			void undo()override;
			void redo()override;

		private:
			RaftModel* _addedRaft;
			RaftModel* _deletedRaft;
		};

		std::unique_ptr<Hix::Features::FeatureContainer> clearSupport(std::unordered_set<GLModel*>& models);


		class SupportMode: public SelectFaceMode
		{
		public:
			SupportMode(const std::unordered_set<GLModel*>& selectedModels, QEntity* parent);
			virtual ~SupportMode();
			void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;
			Hix::Features::FeatureContainer* generateAutoSupport();
			Hix::Features::FeatureContainer* clearSupport();
			Hix::Features::Feature* generateRaft();
			Hix::Features::Feature* removeRaft();
			void removeSupport(SupportModel* target);

		private:
			std::unordered_set<GLModel*> _targetModels;
		};
	}
}

