import QtQuick 2.4
import hix.qml 1.0

RoundButton {
	width: 224
	height: 32
	radius: 16
	color: "#00B9C8"
	//anchors.bottom: parent.bottom
	//anchors.bottomMargin: 16
	//anchors.horizontalCenter: parent.horizontalCenter

	Text {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		color: "#FFFFFF"
		font.pointSize: 10
		font.weight: Font.Bold
		text: "round"
        //text: Roundbutton.btntext
	}
	MouseArea {
		anchors.fill: parent
		hoverEnabled: true
		onEntered: { parent.color="#21959E" }
		onExited: { parent.color="#00B9C8" }
	}
}