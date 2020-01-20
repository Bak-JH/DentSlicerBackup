import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.ToastShell {
	id: root
	width: 500
	height: 60
	
	property string msgTypeImgSource: "qrc:/Resource/toast_check.png"
	property string msgTypeColor: "#00b9c8"

	Rectangle{
		id: shell
		width: parent.width
		height: parent.height
		radius: 8
		color: "#f6feff"
	
		Rectangle {
			id: msgtype
			width: 36
			height: width
			radius: width / 2
			color: root.msgTypeColor // themeColor
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: parent.left
			anchors.leftMargin: (root.height - height) / 2
			Image {
				id: img
				source: root.msgTypeImgSource
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}
		}
		Text {
			text: root.message
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: msgtype.right
			anchors.leftMargin: msgtype.width / 4
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

	onMessageTypeChanged: {
		console.log("messageTypeChanged")
		if(messageType == Hix.ToastShell.Done)
		{
			msgTypeImgSource = "qrc:/Resource/toast_check.png"
			msgTypeColor = "#00b9c8"
		}
		else if (messageType == Hix.ToastShell.ExpectedError)
		{
			msgTypeImgSource = "qrc:/Resource/closebutton.png"
			msgTypeColor = "#FFF000"
		}
		else if (messageType == Hix.ToastShell.UnExpectedError)
		{
			msgTypeImgSource = "qrc:/Resource/closebutton.png"
			msgTypeColor = "#FFF000"
		}
	}
}