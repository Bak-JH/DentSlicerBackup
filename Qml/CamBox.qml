import QtQuick 2.6
import hix.qml 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

CamBoxIcon {
	width: 58
	height: width
	radius: 6
	color: "#ffffff"

	MouseArea {
		hoverEnabled: true
		anchors.fill: parent
		onEntered: { parent.color = "#eeeeee" }
		onExited: { parent.color = "#ffffff" }
	}
}