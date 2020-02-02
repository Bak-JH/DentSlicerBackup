import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.ToastShell {
	id: root
	width: 520
	height: 72
	
	property string msgTypeImgSource: "qrc:/Resource/toast_check.png"
	property string msgTypeColor: "#00b9c8"
	property string textColor: "#008f9b"

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
			color: textColor
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: msgtype.right
			anchors.leftMargin: msgtype.width / 4
			font.family: openRegular.name
			font.pointSize: 10
		}
		/*
		Hix.Button {
			width: 16
			height: 16
			color: "transparent"
			anchors.verticalCenter: parent.verticalCenter
			anchors.right: parent.right
			anchors.rightMargin: (root.height - height) / 2
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
			textColor = "#008f9b"
			shell.color = "#f6feff"
		}
		else if (messageType == Hix.ToastShell.ExpectedError)
		{
			msgTypeImgSource = "qrc:/Resource/toast_x.png"
			msgTypeColor = "#ee5065"
			textColor = "#e64358"
			shell.color = "#fff6f7"
		}
		else if (messageType == Hix.ToastShell.UnExpectedError)
		{
			msgTypeImgSource = "qrc:/Resource/toast_exception.png"
			msgTypeColor = "#ffa545"
			textColor = "#f1820b"
			shell.color = "#fff9f4"
		}
	}
}