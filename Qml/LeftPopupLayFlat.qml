import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1

Item {
	property var sidePadding: 16
	property var spaceOnTop: 44
	property var spaceBtwCtr: 48
	property var themeColor: "#00b9c8"

	width: 256

	LeftPopupShell {
		id: leftpopupshell
		width: 256
		height: 220
		//height: popupHeight
		color: "#F6FEFF"
		radius: 8

		Text {
			id: popuptitle
			text: "Lay Flat"
			//text: parent.title
			font.pointSize: 11
			font.family: openRegular.name
			font.weight: Font.Bold
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.topMargin: sidePadding-2
			anchors.leftMargin: sidePadding
		}

		CloseButton {
			width: 16
			height: 16
			//color: "transparent"
			anchors.top: parent.top
			anchors.right: parent.right
			anchors.topMargin: sidePadding
			anchors.rightMargin: sidePadding
			Image {
				source: "qrc:/Resource/closebutton.png"
				anchors.top: parent.top
				anchors.right: parent.right
			}
		}
		
		Text {
			id: popupbody
			text: "Click the surface to face it down."
			font.pointSize: 10
			font.family: openRegular.name
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.topMargin: spaceOnTop
			anchors.leftMargin: sidePadding
		}

		Image {
			source: "qrc:/Resource/img_layflat.png"
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: popupbody.bottom
			anchors.topMargin: 32
		}
	}
	
	DropShadow {
		anchors.fill: leftpopupshell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: leftpopupshell
	}
}
