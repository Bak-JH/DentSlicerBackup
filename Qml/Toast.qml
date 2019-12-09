import QtQuick 2.6
import hix.qml 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item {
	property var themeColor: "#00b9c8"
	ToastShell {
		id: toastshell
		width: 500
		height: 60
		radius: 8
		color: "#f6feff"
	
		Rectangle {
			id: circle
			width: 36
			height: width
			radius: width / 2
			color: themeColor
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: parent.left
			anchors.leftMargin: (toastshell.height - height) / 2
			Image {
				source: "qrc:/Resource/toast_check.png"
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}
		}
		Text {
			text: parent.toastmsg
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: circle.right
			anchors.leftMargin: circle.width / 4
			font.family: openRegular.name
		}
		CloseButton {
			width: 16
			height: 16
			anchors.verticalCenter: parent.verticalCenter
			anchors.right: parent.right
			anchors.rightMargin: (toastshell.height - height) / 2
			Image {
				source: "qrc:/Resource/closebutton.png"
				anchors.fill: parent
			}
		}
	}
	DropShadow {
		anchors.fill: toastshell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: toastshell
	}
}