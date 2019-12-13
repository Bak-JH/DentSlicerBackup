import QtQuick 2.4
import hix.qml 1.0 as Hix

Hix.RoundButton {
	width: 224
	height: 32
	radius: 16
	color: "#00b9c8"

	Text {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		color: "#FFFFFF"
		font.pointSize: 10
		text: "Apply"
		font.family: openSemiBold.name
	}

	onClicked: { console.log("clicked"); }
	onEntered: { color = "#21959e" }
	onExited: { color = "#00b9c8" }

}