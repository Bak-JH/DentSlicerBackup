import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item {
	property var buttonsWidth: 320
	property var buttonSpace: 32
	width: 420
	height: 240

	Hix.ModalShell {
		id: modalshell
		width: 420
		height: 240
		radius: 8
		color: "#ffffff"

		Hix.CloseButton {
			width: 16
			height: 16
			color: "transparent"
			anchors.top: parent.top
			anchors.right: parent.right
			anchors.topMargin: 16
			anchors.rightMargin: 16
			Image {
				id: closeimg
				source: "qrc:/Resource/closebutton.png"
				anchors.top: parent.top
				anchors.right: parent.right
			}
			onEntered: { closeimg.source = "qrc:/Resource/closebutton_hover.png" }
			onExited: { closeimg.source = "qrc:/Resource/closebutton.png" }
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

			Hix.RoundButton {
				id: leftbtn
				width: (buttonsWidth - buttonSpace) / 2
				height: 32
				radius: height / 2
				color: "#00b9c8"	//themecolor
				anchors.left: parent.left

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.verticalCenter: parent.verticalCenter
					color: "#FFFFFF"
					font.pointSize: 10
					text: "Apply"
					font.family: openSemiBold.name
				}
				onClicked: { console.log("clicked"); }
				onEntered: { color = "#21959e" }
				onExited: { color = "#00b9c8" }
			}

			Hix.RoundButton {
				id: rightbtn
				width: buttonsWidth - leftbtn.width - buttonSpace
				height: 32
				radius: height / 2
				color: "#abb3b3"	//themecolor
				anchors.left: leftbtn.right
				anchors.leftMargin: buttonSpace

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.verticalCenter: parent.verticalCenter
					color: "#FFFFFF"
					font.pointSize: 10
					text: "Apply"
					font.family: openSemiBold.name
				}
				onClicked: { console.log("clicked"); }
				onEntered: { color = "#8b9393" }
				onExited: { color = "#abb3b3" }
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