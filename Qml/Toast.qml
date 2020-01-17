import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.ToastShell {
	id: root
	width: 500
	height: 60
		Rectangle{
		id: shell
		width: parent.width
		height: parent.height
		radius: 8
		color: "#f6feff"
	
		Rectangle {
			id: circle
			width: 36
			height: width
			radius: width / 2
			color: "#00b9c8" // themeColor
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: parent.left
			anchors.leftMargin: (root.height - height) / 2
			Image {
				source: "qrc:/Resource/toast_check.png"
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}
		}
		Text {
			text: root.toastmsg
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: circle.right
			anchors.leftMargin: circle.width / 4
			font.family: openRegular.name
		}
		Hix.CloseButton {
			width: 16
			height: 16
			anchors.verticalCenter: parent.verticalCenter
			anchors.right: parent.right
			anchors.rightMargin: (root.height - height) / 2
			Image {
				source: "qrc:/Resource/closebutton.png"
				anchors.fill: parent
			}
		}
	}
	DropShadow {
		anchors.fill: shell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: shell
	}
}