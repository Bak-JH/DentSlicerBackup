#pragma once

#include <QObject>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>


#include "input/HitTestAble.h"
#include "glmodel.h"

namespace Hix
{
	namespace UI
	{
		class Widget3D;
		class CubeWidget :public Qt3DCore::QEntity, public Hix::Input::HitTestAble
		{
			Q_OBJECT

		public:
			CubeWidget(QEntity* parent);
			virtual ~CubeWidget();

		protected:
			void initHitTest()override;
			Qt3DExtras::QCuboidMesh _mesh;
			Qt3DCore::QTransform _transform;
			Qt3DExtras::QPhongMaterial _material;
		};
	}
}