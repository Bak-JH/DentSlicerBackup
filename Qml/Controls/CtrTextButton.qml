import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls" 


CtrRectButton {
	property var buttonText
	property var fontPoint: 10
	property var textColor: "#FFFFFF"
	property var bgColor: "#00b9c8"
	property var hoverColor: "#21959e"


	id: buttonRoot
	width: 224
	height: 32
	radius: height / 2
	color: bgColor
	Text {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		color: textColor
		font.pointSize: fontPoint
		text: buttonText
		font.family: openSemiBold.name
	}
	onClicked: { }
	onEntered: { color = hoverColor }
	onExited: { color = bgColor }
}

