import QtQuick 2.6
import hix.qml 1.0 as Hix

import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1


Hix.LeftPopupShell 
{
	id: leftpopupshell
	property var myPadding: 16
	property var themeColor: "#00b9c8"

	property var featureName
	property var leftPopupHeight

	Rectangle 
	{
		id: shell
		width: 256
		implicitWidth: width
		height: leftPopupHeight
		//height: popupHeight
		color: "#F6FEFF"
		radius: 8

		Text {
			id: popuptitle
			//text: featureName
			text: leftpopupshell.title
			font.pointSize: 11
			font.family: openRegular.name
			font.weight: Font.Bold
			anchors.top: shell.top
			anchors.left: shell.left
			anchors.topMargin: myPadding-2
			anchors.leftMargin: myPadding
		}

		Hix.CloseButton 
		{
			id: closeButton
			width: 16
			height: 16
			//color: "transparent"
			anchors.top: parent.top
			anchors.right: parent.right
			anchors.topMargin: myPadding
			anchors.rightMargin: myPadding
			image 
			{
				source: "qrc:/Resource/closebutton.png"
				anchors.top: closeButton.top
				anchors.right: closeButton.right
			}
		}

		Hix.RoundButton
		{
			id: roundButton
			width: 224
			height: 32
			radius: 16
			color: "#00b9c8"

			anchors.bottom: parent.bottom
			anchors.bottomMargin: 16
			anchors.horizontalCenter: parent.horizontalCenter

			labelText 
			{
				color: "#FFFFFF"
				font.pointSize: 10
				text: "Apply"
				font.family: openSemiBold.name
				
				anchors.horizontalCenter: roundButton.horizontalCenter
				anchors.verticalCenter: roundButton.verticalCenter
			}

			onClicked: { console.log("clicked"); }
			onEntered: { color = "#21959e" }
			onExited: { color = "#00b9c8" }
		}
	}
	
	DropShadow 
	{
		anchors.fill: leftpopupshell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: leftpopupshell
	}
}
