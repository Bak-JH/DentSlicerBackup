#ifndef RAYCASTCONTROLLER_H
#define RAYCASTCONTROLLER_H

#include <QAbstractRayCaster>
namespace Qt3DCore
{
	class QEntity;
}
namespace Qt3DRender
{
	class QScreenRayCaster;
	class QAbstractRayCaster;
}
namespace Qt3DInput
{
	class QMouseHandler;
	class QMouseEvent;
}
class RayCastController: public QObject
{
	Q_OBJECT
public:
    RayCastController();
	void initialize(Qt3DCore::QEntity* camera);
	//void initialize(Qt3DRender::QScreenRayCaster* rayCaster, Qt3DInput::QMouseHandler* mouseHandler);

private:
	Qt3DRender::QScreenRayCaster* _rayCaster = nullptr;
	Qt3DInput::QMouseHandler* _mouseHandler = nullptr;
public slots:
	void mouseReleased(Qt3DInput::QMouseEvent* mouse);
	void hitsChanged(const Qt3DRender::QAbstractRayCaster::Hits& hits);

signals:

};

#endif // RAYCASTCONTROLLER_H
