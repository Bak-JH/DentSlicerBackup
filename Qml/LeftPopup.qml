import QtQuick 2.6
import hix.qml 1.0

import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1


Item {
	property var myPadding: 16
	property var themeColor: "#00b9c8"

	LeftPopupShell {
		id: leftpopupshell
		width: 256
		height: popupHeight
		color: "#F6FEFF"
		radius: 8

		Text {
			id: popuptitle
			text: parent.title
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
		
		CtrInputBox {
			width: parent.width - myPadding * 2
			anchors.left: parent.left
			anchors.leftMargin: myPadding
			anchors.top: popuptitle.bottom
			anchors.topMargin: 16
		}
		
		CtrDropdownBox {
			width: parent.width - myPadding * 2
			anchors.left: parent.left
			anchors.leftMargin: myPadding
			anchors.top: parent.top
			anchors.topMargin: 100
		}

		CtrCutSwitch {
			width: parent.width - myPadding * 2
			anchors.left: parent.left
			anchors.leftMargin: myPadding
			anchors.top: parent.top
			anchors.topMargin: 124
		}

		CtrLabelTextBox {
			width: parent.width - myPadding * 2
			anchors.left: parent.left
			anchors.leftMargin: myPadding
			anchors.top: parent.top
			anchors.topMargin: 240
		}
		
		/*
		InputBox {
			anchors.left: parent.left
			anchors.leftMargin: 16
			anchors.top: popuptitle.bottom
			anchors.topMargin: 16
			Text {
				text: parent.propName
			}
			SpinBox{
				from: inputRect.from
				to: inputRect.to
		    }
		}*/

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
