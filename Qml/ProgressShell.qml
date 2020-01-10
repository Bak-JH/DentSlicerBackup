import QtQuick 2.6
import hix.qml 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12


Item {
	Rectangle {
		id: progressshell
		width: 720
		height: 100
		radius: width / 2
		color: "#ffffff"

		Text {
			text: "something"
			font.family: openRegular.name
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top
			anchors.topMargin: parent.height * 0.24
		}

		Rectangle {
			width: 600
			height: 10
			radius: height / 2
			color: "#eeeeee"
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.bottom: parent.bottom
			anchors.bottomMargin: 30
			Rectangle {
				width: 250
				height: 10
				radius: height / 2
				color: "#19d6e0"
				anchors.left: parent.left
				anchors.verticalCenter: parent.verticalCenter
			}
		}
	}
	
	DropShadow {
		anchors.fill: progressshell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: progressshell
	}
}
