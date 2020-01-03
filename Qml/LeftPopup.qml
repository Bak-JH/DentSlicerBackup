import QtQuick 2.6
import hix.qml 1.0

import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1


LeftPopupShell 
{
	id: leftpopupshell
	property var myPadding: 16
	property var themeColor: "#00b9c8"
	property int leftPopupHeight
	content
	{
		width: 256
		height: 300
		color: "#F6FEFF"
		radius: 8

		title
		{
			id: popuptitle
			text: leftpopupshell.title
			font.pointSize: 11
			font.family: openRegular.name
			font.weight: Font.Bold
			anchors.top: content.top
			anchors.left: content.left
			anchors.topMargin: myPadding-2
			anchors.leftMargin: myPadding
		}

		closeButton
		{
			id: closeButton
			width: 16
			height: 16
			anchors.top: content.top
			anchors.right: content.right
			anchors.topMargin: myPadding
			anchors.rightMargin: myPadding
			image 
			{
				source: "qrc:/Resource/closebutton.png"
				anchors.top: closeButton.top
				anchors.right: closeButton.right
			}
		}

		roundButton
		{
			id: roundButton
			width: 224
			height: 32
			radius: 16
			color: "#00b9c8"

			anchors.bottom: content.bottom
			anchors.bottomMargin: 16
			anchors.horizontalCenter: content.horizontalCenter

			labelText {
				color: "#FFFFFF"
				font.pointSize: 10
				text: "Apply"
				font.family: openSemiBold.name
				
				anchors.horizontalCenter: roundButton.horizontalCenter
				anchors.verticalCenter: roundButton.verticalCenter
			}

			onClicked: { console.log("clicked"); }
			onEntered: { roundButton.color = "#21959e" }
			onExited: { roundButton.color = "#00b9c8" }
		}
	}

	DropShadow {
		anchors.fill: content
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: content
	}
}
