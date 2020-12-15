#pragma once
#include "../interfaces/DialogedMode.h"
#include "../interfaces/WidgetMode.h"
#include "../interfaces/RangeSliderMode.h"

#include "DentEngine/src/Bounds3D.h"
#include "../../render/PlaneMeshEntity.h"
#include "../rotate.h"
#include "../../Qml/components/ControlForwardInclude.h"
#include "SelectionPlane.h"
#include "../CSG/CSG.h"
#include "../interfaces/PPShaderMode.h"

class GLModel;

namespace Hix
{

	namespace Features
	{
		enum class MBEditMode
		{
			Rotation=0,
			FaceSelection,
			PlaneCut

		};
		enum class SelectionState
		{
			None = 0,
			Selecting,
			Selected
		};
		class TwoManifoldBuilder;
		class FaceSelector
		{
		public:
			FaceSelector(const Hix::Render::SceneEntity& subject);
			std::unordered_set<Hix::Engine3D::VertexConstItr> doSelection(const Hix::Plane3D::PDPlane& selectionPlane, const std::vector<QVector3D>& polyline);

		private:
			//cached subject mesh
			const Hix::Engine3D::Mesh& _subjectOrig;
			CorkTriMesh _subject;

		};
		class ModelBuilderMode : public Hix::Features::DialogedMode, public PPShaderMode, public Hix::Features::WidgetMode, public Hix::Features::RangeSliderMode
		{
		public:
			ModelBuilderMode();
			virtual ~ModelBuilderMode();
			void build();
			void applyButtonClicked()override;
			void featureStarted()override;
			void featureEnded()override;
			QVector3D getWidgetPosition()override;
			void updatePosition()override;


		private:
			void setMode(MBEditMode mode);
			void setModeTxts(MBEditMode mode);
			void setSelectionState(SelectionState state);
			bool select();
			void erase();
			MBEditMode _edit;
			SelectionState _selectionState = SelectionState::None;
			Hix::Features::FeatureContainerFlushSupport* _rotateContainer;
			Hix::Render::PlaneMeshEntity _topPlane;
			Hix::Render::PlaneMeshEntity _bottPlane;
			std::unique_ptr<GLModel> _model;

			Hix::QML::Controls::Button* _button0;
			Hix::QML::Controls::Button* _button1;
			Hix::QML::Controls::Button* _prevButton;
			Hix::QML::Controls::Button* _nextButton;
			//Hix::QML::Controls::ToggleSwitch* _faceSelectionSwtch;
			//Hix::QML::Controls::ToggleSwitch* _rotationSwtch;
			Hix::Features::SelectionPlane _selectionPlane;
			//Hix::QML::Controls::DropdownBox* _editTypeCmb;
			std::optional<FaceSelector> _faceSelector;
			std::unordered_set<Hix::Engine3D::FaceConstItr> _selectedFaces;
			friend class MBPrep;
		};



		class MBPrep : public Hix::Tasking::Task
		{
		public:
			MBPrep(ModelBuilderMode* mode, QUrl fileUrl);
			virtual ~MBPrep();
			void run()override;
		private:
			ModelBuilderMode* _mode;
			QUrl _fileUrl;
		};

	}
}


