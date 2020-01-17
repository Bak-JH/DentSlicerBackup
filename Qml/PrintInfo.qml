import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item {
	Rectangle {
		id: printinfo
		width :400
		height: 72
		radius: height / 2
		Hix.Button {
			id: refresh
			width: parent.height - 16
			height: width
			radius: width / 2
			color: "#19d6e0"
			anchors.left: parent.left
			anchors.leftMargin: 8
			anchors.verticalCenter: parent.verticalCenter
			onEntered: { color = "#00b9c8" }	//themeColor
			onExited: { color = "#19d6e0" }
			Image {
				source: "qrc:/Resource/ic_refresh.png"
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}
		}
		Hix.PrintInfoText {
			width: 240
			height: parent.height * 0.6
			anchors.left: refresh.right
			anchors.leftMargin: printinfo.width * 0.05
			anchors.verticalCenter: parent.verticalCenter
			Text {
				id: time
				text: qsTr("<font color=\"#888888\">Time </font>") + parent.printtime
				color: "#000000"
				font.family: openRegular.name
				anchors.left: parent.left
					
			}
			Text {
				id: layer
				text: qsTr("<font color=\"#888888\">Layer </font>") + parent.printlayer
				font.family: openRegular.name
				color: "#000000"
				anchors.left: time.left
				anchors.leftMargin: printinfo.width * 0.32
				anchors.top: parent.top
			}
			Text {
				id: size
				text: qsTr("<font color=\"#888888\">Size </font>") + parent.printsize
				font.family: openRegular.name
				color: "#000000"
				anchors.left: parent.left
				anchors.bottom: parent.bottom
			}
			Text {
				id: volume
				text: qsTr("<font color=\"#888888\">Volume </font>") + parent.printvolume
				font.family: openRegular.name
				color: "#000000"
				anchors.left: size.left
				anchors.leftMargin: printinfo.width * 0.32
				anchors.bottom: parent.bottom
			}
		}
	}
	DropShadow {
		anchors.fill: printinfo
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: printinfo
	}
}