import QtQuick 2.6
import hix.qml 1.0 as Hix 
Hix.Button
{
	property string btnText;
	property string hoverColor;
	property string defaultColor;

	id: button
	height: 32
	radius: height / 2
	color: defaultColor

	Text {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		color: "#FFFFFF"
		font.pointSize: 10
		text: button.btnText
		font.family: openSemiBold.name
	}
	
	onClicked: { console.log("clicked"); }
	onEntered: { button.color = hoverColor }
	onExited: { button.color = defaultColor}
	
}