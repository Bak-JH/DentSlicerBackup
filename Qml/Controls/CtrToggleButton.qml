import QtQuick 2.0
import QtQuick.Controls 2.1
import hix.qml 1.0 as Hix

Hix.ToggleSwitch {
	color: "transparent"
	id: root
	property alias mouseArea: mouseArea
	MouseArea {
		id: mouseArea
		anchors.fill: parent
		hoverEnabled: true
		// onEntered: {
		// }
		// onExited: {
		// }
		onClicked: {
			parent.isChecked = !parent.isChecked;
		}
		
	}
}