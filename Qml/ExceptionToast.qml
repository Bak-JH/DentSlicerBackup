import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item {
	width: 520
	height: 72
	Hix.ToastShell {
		id: toastshell
		width: parent.width
		height: parent.height
		radius: 8
		color: "#fff9f4"
	
		Rectangle {
			id: circle
			width: 36
			height: width
			radius: width / 2
			color: "#ffa545"
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: parent.left
			anchors.leftMargin: (toastshell.height - height) / 2
			Image {
				source: "qrc:/Resource/toast_exception.png"
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}
		}
		Text {
			text: parent.toastmsg
			color: "#f1820b"
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: circle.right
			anchors.leftMargin: circle.width / 4
			font.family: openSemiBold.name
			font.pointSize: 10
		}
		/*
		Hix.CloseButton {
			width: 16
			height: 16
			color: "transparent"
			anchors.verticalCenter: parent.verticalCenter
			anchors.right: parent.right
			anchors.rightMargin: (toastshell.height - height) / 2
			Image {
				id: closeimg
				source: "qrc:/Resource/closebutton.png"
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}
			onEntered: { closeimg.source = "qrc:/Resource/closebutton_hover.png" }
			onExited: { closeimg.source = "qrc:/Resource/closebutton.png" }
		}
		*/
	}
	DropShadow {
		anchors.fill: toastshell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: toastshell
	}
}