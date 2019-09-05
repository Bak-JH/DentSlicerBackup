#pragma once
#include <Qt3DCore>
#include <Qt3DExtras>
#include <array>
#include "input/Clickable.h"
#include "input/HitTestAble.h"

class GLModel;


namespace Hix
{
	namespace Cut
	{
		class DrawingPlane : public Qt3DCore::QEntity, public Hix::Input::HitTestAble, public Hix::Input::Clickable
		{
			Q_OBJECT
		public:
			DrawingPlane(GLModel* owner);
			void enablePlane(bool isEnable);
			void enableDrawing(bool isEnable);
			Qt3DCore::QTransform& transform();
			void clicked(Hix::Input::MouseEventData&, const Qt3DRender::QRayCasterHit&)override;

		protected:
			void initHitTest()override;
		private:
			std::vector<QVector2D> _contour;
			bool _drawingEnabled = false;
			std::array<Qt3DExtras::QPlaneMesh, 2> _planeMeshes;
			Qt3DCore::QTransform _transform;
			Qt3DExtras::QPhongAlphaMaterial _material;
			GLModel* _owner = nullptr;
		};


	}
}