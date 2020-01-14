import QtQuick 2.6
import hix.qml 1.0 as Hix

//MenuBtnShell {
Hix.MenuButton {
	id: menuButton
	property string iconBasic
	property string nameColor: "#9d9d9d"
	property string featureName

	width: 85
	height: 90
	color: "#FFFFFF"
	
	Image {
		sourceSize.width: width
		anchors.top: parent.top;
		anchors.topMargin: (parent.height - height) * 0.5 - parent.height * 0.08
		anchors.horizontalCenter: parent.horizontalCenter;
		source: iconBasic
	}
	Text {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 10
		text: featureName
		color: nameColor
		font.pointSize: 10
		font.family: openRegular.name
	}

	
}