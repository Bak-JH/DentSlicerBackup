import QtQuick 2.6
import hix.qml 1.0 as Hix
import "Controls"

//MenuBtnShell {
CtrToggleButton {
	id: menuButton
	property string iconBasic
	property string iconSelected
	property string featureName
	property string nameColor: "#9d9d9d"
	property string activeNameColor: "#1db2c4"
	property string hoverColor: "#eeeeee"
	property string defaultColor: "#ffffff"
	property bool isActive: false

	width: 80
	height: 90
	Rectangle
	{
		anchors.fill: parent
		id: rect
		color: defaultColor
	}
	Image {
		id: iconImg
		sourceSize.width: width
		anchors.top: parent.top;
		anchors.topMargin: (parent.height - height) * 0.5 - parent.height * 0.08
		anchors.horizontalCenter: parent.horizontalCenter;
		source: iconBasic
	}
	Text {
		id: text
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 10
		text: featureName
		color: nameColor
		font.pointSize: 10
		font.family: openRegular.name
	}

	onChecked: { 
		text.color = activeNameColor;
		iconImg.source = iconSelected;
	}
	
	onUnchecked: {
		text.color = nameColor;
		iconImg.source = iconBasic;
	}

	mouseArea {
		onEntered: {
			if (nameColor == activeNameColor) {}	// hover on already selected button
			else rect.color = hoverColor	// hover on unselected button
		}
		onExited: {
			if (nameColor == activeNameColor) {}	// exit from already selected button
			else rect.color = defaultColor	// exit from unselected button
		}
	}


}