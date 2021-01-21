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
			AddSupport(std::unique_ptr<SupportModel> model);
			virtual ~AddSupport();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;

		private:
			std::variant<SupportModel*, std::unique_ptr<SupportModel>> _model;

		};
		//class AddInterconnect : public Feature
		//{
		//public:
		//	AddInterconnect(std::array<SupportModel*,2> pts);
		//	virtual ~AddInterconnect();
		//protected:
		//	void undoImpl()override;
		//	void redoImpl()override;
		//	void runImpl()override;

		//private:
		//	std::array<SupportModel*, 2> _pts;
		//	std::variant<SupportModel*, std::unique_ptr<SupportModel>> _model;

		//};

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

		class ManualSupport : public FeatureContainer
		{
		public:
			ManualSupport(const OverhangDetect::Overhang& overhang);
			virtual ~ManualSupport();
		protected:
			void runImpl()override;

		private:
			OverhangDetect::Overhang _overhang;
		};

		class GenerateInterconnections : public FeatureContainer
		{
		public:
			GenerateInterconnections(std::array<SupportModel*, 2> models);
			virtual ~GenerateInterconnections();
		protected:
			void runImpl()override;
		private:
			std::array<SupportModel*, 2> _models;
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
			void interconnectSupports();
			void applyButtonClicked()override;

		private:
			Hix::Features::Feature* generateRaft();
			Hix::Features::Feature* removeRaft();
			static void applySupportSettings();
			void removeSupport(SupportModel* target);
			friend class Hix::Support::SupportModel;
			std::unordered_set<GLModel*> _targetModels;

			//UI
			Hix::QML::Controls::Button* _generateSupportsBttn;
			Hix::QML::Controls::Button* _generateRaftBttn;
			Hix::QML::Controls::Button* _clearSupportsBttn;
			Hix::QML::Controls::Button* _suppSettBttn;
			Hix::QML::Controls::Button* _raftSettBttn;
			Hix::QML::Controls::Button* _reconnectBttn;			
			Hix::QML::Controls::ToggleSwitch* _manualEditBttn;
			Hix::QML::Controls::ToggleSwitch* _thickenFeetBttn;

			//UI slicing options
			Hix::QML::Controls::DropdownBox* _suppTypeDrop;
			Hix::QML::Controls::DropdownBox* _raftTypeDrop;
			Hix::QML::Controls::InputSpinBox* _suppDensitySpin;
			Hix::QML::Controls::InputSpinBox* _maxRadSpin;
			Hix::QML::Controls::InputSpinBox* _minRadSpin;
			Hix::QML::Controls::InputSpinBox* _raftRadiusMultSpin;
			Hix::QML::Controls::InputSpinBox* _raftMinMaxRatioSpin;
			Hix::QML::Controls::InputSpinBox* _raftThickness;
			Hix::QML::Controls::DropdownBox* _interconnectTypeDrop;
			Hix::QML::Controls::InputSpinBox* _supportBaseHeightSpin;
			Hix::QML::Controls::InputSpinBox* _maxConnectDistanceSpin;

			




		};
	}
}

