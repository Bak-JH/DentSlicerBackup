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
			AddSupport(const OverhangDetect::Overhang& overhang);
			virtual ~AddSupport();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;

		private:
			OverhangDetect::Overhang _overhang;
			std::variant<SupportModel*, std::unique_ptr<SupportModel>> _model;

		};

		class RemoveSupport : public Feature
		{
		public:
			RemoveSupport(SupportModel* target);
			virtual ~RemoveSupport();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;

		private:
			std::variant<SupportModel*, std::unique_ptr<SupportModel>> _model;
		};

		class AddRaft : public Feature
		{
		public: 
			AddRaft();
			virtual ~AddRaft();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;

		private:
			std::variant<RaftModel*, std::unique_ptr<RaftModel>> _model;
		};
		
		class RemoveRaft : public Feature
		{
		public: 
			RemoveRaft();
			virtual ~RemoveRaft();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;

		private:
			std::variant<RaftModel*, std::unique_ptr<RaftModel>> _model;
		};

		class SupportMode: public SelectFaceMode
		{
		public:
			SupportMode(const std::unordered_set<GLModel*>& selectedModels);
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

