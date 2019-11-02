#pragma once
#include <Qt3DCore>
#include <Qt3DExtras>
#include <array>
#include "input/Clickable.h"
#include "input/HitTestAble.h"
#include "../../common/HetUniquePtr.h"
#include <unordered_set>
class GLModel;


namespace Hix
{
	namespace Features
	{
		namespace Cut
		{
			class FreeCutPtWidget;
			class DrawingPlane : public Qt3DCore::QEntity, public Hix::Input::HitTestAble, public Hix::Input::Clickable
			{
			public:
				DrawingPlane(Qt3DCore::QEntity* owner);
				virtual ~DrawingPlane();
				void enablePlane(bool isEnable);
				void enableDrawing(bool isEnable);
				Qt3DCore::QTransform& transform();
				void clicked(Hix::Input::MouseEventData&, const Qt3DRender::QRayCasterHit&)override;
				void removePt(FreeCutPtWidget* pt);
				std::vector<QVector3D> contour()const;
			protected:
				void initHitTest()override;
			private:
				std::unordered_map<Qt3DCore::QEntity*, Qt3DCore::QTransform*> _meshTransformMap;
				bool _drawingEnabled = false;
				std::array<Qt3DExtras::QPlaneMesh, 2> _planeMeshes;
				Qt3DCore::QTransform _transform;
				Qt3DCore::QEntity* _owner = nullptr;
				std::unordered_set<Hix::Memory::HetUniquePtr<FreeCutPtWidget>> _ptWidgets;
				FreeCutPtWidget* _lastPt = nullptr;
			};


		}
	}
}
