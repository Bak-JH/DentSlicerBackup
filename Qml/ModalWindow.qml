import QtQuick 2.6
import hix.qml 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item {
	property var themeColor: "#00b9c8"
	property var buttonsWidth: 320
	property var buttonSpace: 32

	ModalShell {
		id: modalshell
		width: 420
		height: 240
		radius: 8
		color: "#ffffff"

		ModalCloseButton {
			width: 16
			height: 16
			color: transparent
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
		
		Text {
			text: parent.modalmsg
			font.family: openRegular.name
			font.pointSize: 10
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top
			anchors.topMargin: parent.height * 0.4
		}

		Rectangle {
			width: buttonsWidth
			height: 32
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.bottom: parent.bottom
			anchors.bottomMargin: parent.height * 0.2

			RoundedButton {
				id: leftbtn
				width: (buttonsWidth - buttonSpace) / 2
				color: themeColor
				anchors.left: parent.left
			}
			RoundedButton {
				id: rightbtn
				width: buttonsWidth - leftbtn.width - buttonSpace
				color: themeColor
				anchors.left: leftbtn.right
				anchors.leftMargin: buttonSpace
			}
		}
	}

	DropShadow {
		anchors.fill: modalshell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: modalshell
	}
}