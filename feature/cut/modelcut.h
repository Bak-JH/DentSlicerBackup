#pragma once
#include "../interfaces/DialogedMode.h"
#include "../interfaces/SliderMode.h"
#include "../slice/ContourBuilder.h"
#include "render/Bounds3D.h"
#include "DrawingPlane.h"
#include <optional>
namespace Hix
{
	namespace QML
	{
		namespace Controls
		{
			class ToggleSwitch;
		}
	}

	namespace Features
	{
		class ModelCut : public DialogedMode, public SliderMode
		{
		public:
			enum CutType
			{
				ZAxial,
				Polyline
			};
			ModelCut();
			virtual ~ModelCut();
			void cutModeSelected();
			void applyButtonClicked()override;
				
			// polyline
			void addCuttingPoint(QVector3D v);
			void removeCuttingPoint(int idx);
			void removeCuttingPoints();
			void drawLine(QVector3D endpoint);

		public slots:
			void onChecked();
			void onUnchecked();

		private:
			//cutting
			CutType _cutType = ZAxial;
			const Engine3D::Mesh* _origMesh;
			std::optional<Hix::Features::Cut::DrawingPlane> _cuttingPlane;
			Hix::Engine3D::Bounds3D _modelsBound;
			std::unordered_set<GLModel*> _models;
			Hix::QML::Controls::ToggleSwitch* _cutSwitch;
		};
	}
}
