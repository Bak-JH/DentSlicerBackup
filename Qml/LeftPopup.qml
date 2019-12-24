import QtQuick 2.6
import hix.qml 1.0

import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1


Item {
	property var myPadding: 16
	property var themeColor: "#00b9c8"

	property var featureName
	property var leftPopupHeight

	LeftPopupShell {
		id: leftpopupshell
		width: 256
		implicitWidth: width
		height: leftPopupHeight
		//height: popupHeight
		color: "#F6FEFF"
		radius: 8

		Text {
			id: popuptitle
			text: featureName
			//text: parent.title
			font.pointSize: 11
			font.family: openRegular.name
			font.weight: Font.Bold
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.topMargin: myPadding
			anchors.leftMargin: myPadding
		}

		CloseButton {
			width: 16
			height: 16
			//color: "transparent"
			anchors.top: parent.top
			anchors.right: parent.right
			anchors.topMargin: myPadding
			anchors.rightMargin: myPadding
			Image {
				source: "qrc:/Resource/closebutton.png"
				anchors.top: parent.top
				anchors.right: parent.right
			}
		}
		/*
		Text {
			id: popupbody
			text: parent.body
			font.pointSize: 10
			font.family: openRegular.name
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.topMargin: 40
			anchors.leftMargin: myPadding
		}
		
		CtrInputBox {
			width: parent.width - myPadding * 2
			anchors.left: parent.left
			anchors.leftMargin: myPadding
			anchors.top: popuptitle.bottom
			anchors.topMargin: 48
		}
		
		CtrDropdownBox {
			width: parent.width - myPadding * 2
			anchors.left: parent.left
			anchors.leftMargin: myPadding
			anchors.top: parent.top
			anchors.topMargin: 120
		}

		CtrCutSwitch {
			width: parent.width - myPadding * 2
			anchors.left: parent.left
			anchors.leftMargin: myPadding
			anchors.top: parent.top
			anchors.topMargin: 144
		}

		CtrLabelTextBox {
			width: parent.width - myPadding * 2
			anchors.left: parent.left
			anchors.leftMargin: myPadding
			anchors.top: parent.top
			anchors.topMargin: 260
		}

		CtrScaleXYZ {
			width: parent.width - myPadding * 2
			anchors.left: parent.left
			anchors.leftMargin: myPadding
			anchors.top: parent.top
			anchors.topMargin: 300
		}
		*/


		RoundedButton {
			anchors.bottom: parent.bottom
			anchors.bottomMargin: 16
			anchors.horizontalCenter: parent.horizontalCenter
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
