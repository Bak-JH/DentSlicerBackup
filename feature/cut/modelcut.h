#pragma once
#include "../interfaces/DialogedMode.h"
#include "DentEngine/src/ContourBuilder.h"
#include "DentEngine/src/Bounds3D.h"
#include "DrawingPlane.h"
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
		class ModelCut : public DialogedMode
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
			void getSliderSignal(double value);
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
			Hix::Features::Cut::DrawingPlane _cuttingPlane;
			Hix::Engine3D::Bounds3D _modelsBound;
			std::unordered_set<GLModel*> _models;
			Hix::QML::Controls::ToggleSwitch* _cutSwitch;
		};
	}
}
