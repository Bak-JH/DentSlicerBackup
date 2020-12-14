import QtQuick 2.6
import hix.qml 1.0 as Hix

import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls"

Hix.FeaturePopupShell 
{
	id: root
	width: 256
	property var title
	property var myPadding: 16
	property var enableApply: true
    property alias applyButton: applyButton
	property var bottomMargin: applyButton.height + applyButton.anchors.bottomMargin
	MouseArea{
		id: blockingArea
		anchors.fill: parent
	}
	
	Rectangle 
	{
		id: shell
		width: parent.width
		height: parent.height
		color: "#F6FEFF"
		radius: 8

		Text {
			id: popuptitle
			text: root.title
			font.pointSize: 11
			font.family: openRegular.name
			font.weight: Font.Bold
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.topMargin: myPadding-2
			anchors.leftMargin: myPadding
		}

		CtrRectButton 
		{
			id: closeButton
			width: 16
			height: 16
			color: "#F6FEFF"
			anchors.top: parent.top
			anchors.right: parent.right
			anchors.topMargin: myPadding
			anchors.rightMargin: myPadding
			Image {
				id: closeimg
				source: "qrc:/Resource/closebutton.png"
				anchors.top: closeButton.top
				anchors.right: closeButton.right
			}
			onEntered: { closeimg.source = "qrc:/Resource/closebutton_hover.png" }
			onExited: { closeimg.source = "qrc:/Resource/closebutton.png" }
		}

		CtrRectButton
		{
			id: applyButton
			width: 224
			height: 32
			radius: 16
			color: "#00b9c8"
			visible: enableApply
			anchors.bottom: parent.bottom
			anchors.bottomMargin: 16
			anchors.horizontalCenter: parent.horizontalCenter

			Text 
			{
				color: "#FFFFFF"
				font.pointSize: 10
				text: "Apply"
				font.family: openSemiBold.name
				
				anchors.horizontalCenter: applyButton.horizontalCenter
				anchors.verticalCenter: applyButton.verticalCenter
			}

			onEntered: { color = "#21959e" }
			onExited: { color = "#00b9c8" }
		}
	}
	
	DropShadow 
	{
		anchors.fill: shell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: shell
	}
}
