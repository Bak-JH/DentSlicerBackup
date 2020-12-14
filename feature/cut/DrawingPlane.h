#pragma once
#include <Qt3DCore>
#include <Qt3DExtras>
#include <array>
#include "../../render/PlaneMeshEntity.h"
#include "input/Clickable.h"
#include "input/HitTestAble.h"
#include "../../common/HetUniquePtr.h"
#include <unordered_set>
#include "../../application/ApplicationManager.h"

class GLModel;


namespace Hix
{
	namespace Features
	{
		namespace Cut
		{
			class FreeCutPtWidget;
			class DrawingPlane : 
				public Hix::Render::PlaneMeshEntity, public Hix::Input::HitTestAble, public Hix::Input::Clickable
			{
			public:
				DrawingPlane(Qt3DCore::QEntity* owner,
					float width =	Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedBound.lengthX(),
					float height = Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedBound.lengthY(),
					QColor color = QColor(140, 140, 140), float alpha = 0.1f);

				virtual ~DrawingPlane();
				void enablePlane(bool isEnable);
				void enableDrawing(bool isEnable);
				void clicked(Hix::Input::MouseEventData&, const Qt3DRender::QRayCasterHit&)override;
				virtual void removePt(FreeCutPtWidget* pt);
				virtual void clearPt();
				std::vector<QVector3D> contour()const;
			protected:
				void initHitTest()override;
				bool _drawingEnabled = false;
				std::unordered_set<Hix::Memory::HetUniquePtr<FreeCutPtWidget>> _ptWidgets;
				FreeCutPtWidget* _lastPt = nullptr;

			};


		}
	}
}
