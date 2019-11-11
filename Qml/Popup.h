#pragma once
#include <5.12.3/QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/5.12.3/QtQuick/private/qquickmousearea_p.h>
#include <QtQuick/5.12.3/QtQuick/private/qquickevents_p_p.h>
#include <Qt3DQuickExtras/Qt3DQuickWindow>

class TestRect : public QQuickRectangle
{
	Q_OBJECT
public:
	TestRect();
public slots:
	void onClick();
private:
	QQuickMouseArea* mouseArea;
	Qt3DExtras::Quick::Qt3DQuickWindow* window;
};