import QtQuick 2.4
import hix.qml 1.0

import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1

Item {
	LeftPopupShell {

		id: leftpopupshell
		width: 256
		height: popupHeight
		color: "#F6FEFF"
		radius: 8

		Text {
			id: popuptitle
			text: parent.title
			font.pointSize: 10
			font.weight: Font.Bold
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.topMargin: 16
			anchors.leftMargin: 16
		}

		CloseButton {
			width: 16
			height: 16
			anchors.top: parent.top
			anchors.right: parent.right
			anchors.topMargin: 16
			anchors.rightMargin: 16
			Image {
				source: "qrc:/Resource/closebutton.png"
				anchors.top: parent.top
				anchors.right: parent.right
			}
		}
		
		CtrInputBox {
			width: 224
			anchors.left: parent.left
			anchors.leftMargin: 16
			anchors.top: popuptitle.bottom
			anchors.topMargin: 16
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
			/*
			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
				color: "#FFFFFF"
				font.pointSize: 10
				font.weight: Font.Bold
                text: parent.btntext
			}*/
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
