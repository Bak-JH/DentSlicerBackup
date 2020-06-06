#pragma once
#include "interfaces/SelectFaceMode.h"
#include "interfaces/DialogedMode.h"
#include "support/SupportModel.h"
#include "support/SupportRaftManager.h"
#include "../Qml/components/ControlForwardInclude.h"

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

		//vertical suppport only for now....
		class AutoSupport : public FeatureContainer
		{
		public:
			AutoSupport(GLModel * model);
			virtual ~AutoSupport();
		protected:
			void runImpl()override;

		private:
			GLModel* _model;
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

		class SupportMode: public SelectFaceMode, public DialogedMode
		{
		public:
			SupportMode();
			virtual ~SupportMode();
			void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;
			void generateAutoSupport(std::unordered_set<GLModel*> models);
			static void clearSupport(const std::unordered_set<GLModel*> models);
			void regenerateRaft();
			void applyButtonClicked()override;

		private:
			Hix::Features::Feature* generateRaft();
			Hix::Features::Feature* removeRaft();
			void applySupportSettings();
			void removeSupport(SupportModel* target);
			friend class Hix::Support::SupportModel;
			std::unordered_set<GLModel*> _targetModels;

			//UI
			Hix::QML::Controls::Button* _generateSupportsBttn;
			Hix::QML::Controls::Button* _generateRaftBttn;
			Hix::QML::Controls::Button* _clearSupportsBttn;
			Hix::QML::Controls::ToggleSwitch* _manualEditBttn;
			//UI slicing options
			Hix::QML::Controls::DropdownBox* _suppTypeDrop;
			Hix::QML::Controls::DropdownBox* _raftTypeDrop;
			Hix::QML::Controls::InputSpinBox* _suppDensitySpin;
			Hix::QML::Controls::InputSpinBox* _maxRadSpin;
			Hix::QML::Controls::InputSpinBox* _minRadSpin;
			Hix::QML::Controls::InputSpinBox* _raftRadiusMultSpin;
			Hix::QML::Controls::InputSpinBox* _raftMinMaxRatioSpin;
			Hix::QML::Controls::InputSpinBox* _raftThickness;

			



		};
	}
}

