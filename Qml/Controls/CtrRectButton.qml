import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls" 


CtrButtonBase {

	id: buttonRoot
    property alias rect: rect
    property alias radius: rect.radius
    property alias color: rect.color
    property alias border: rect.border

    Rectangle
    {
        id: rect
        color: "transparent"
        anchors.fill: parent
    }
}

