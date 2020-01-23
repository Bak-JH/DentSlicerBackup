import QtQuick 2.6
import hix.qml 1.0 as Hix 
Hix.Button 
{
	property string btnText;

	id: button
	height: 32
	radius: height / 2

	Text {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		color: "#FFFFFF"
		font.pointSize: 10
		text: button.btnText
		font.family: openSemiBold.name
	}
	onClicked: { console.log("clicked"); }
	onEntered: { button.color = "#21959e" }
	onExited: { button.color = "#00b9c8" }
}