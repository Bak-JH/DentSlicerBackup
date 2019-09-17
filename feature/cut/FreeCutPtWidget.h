#pragma once
#include "../../ui/CubeWidget.h"
#include "../../input/Clickable.h"
#include "../../input/Hoverable.h"
#include <memory>
namespace Hix
{
	namespace Render
	{
		class LineMeshEntity;
	}
	namespace Features
	{
		namespace Cut
		{
			class DrawingPlane;
			class FreeCutPtWidget : public Hix::UI::CubeWidget, public Hix::Input::Clickable, public Hix::Input::Hoverable
			{
			public:
				FreeCutPtWidget(DrawingPlane* drawingPlane);
				virtual ~FreeCutPtWidget();
				void onEntered()override;
				void onExited() override;
				void clicked(Hix::Input::MouseEventData&, const Qt3DRender::QRayCasterHit&)override;

				void updateLineTo();
				void setTranslation(const QVector3D& trans);
				QVector3D translation()const;
				FreeCutPtWidget* next = nullptr;
				FreeCutPtWidget* prev = nullptr;

			private:
				DrawingPlane* _drawingPlane;
				std::unique_ptr<Render::LineMeshEntity> _line;

			};
		}
	}
}
