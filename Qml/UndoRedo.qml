import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item {
	property var btnSize: 60
	width: 180
	height: btnSize

	Hix.Button {
		id: undo
		color: "#ffffff"
		width: btnSize
		height: btnSize
		radius: width / 2
		anchors.left: parent.left
		Image {
			source: "qrc:/Resource/undo_arrow.png"
			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter
		}
		onEntered: { color = "#eeeeee" }
		onExited: { color = "#ffffff" }
	}
	Hix.Button {
		id: redo
		color: "#ffffff"
		width: btnSize
		height: btnSize
		radius: width / 2
		anchors.left: undo.right
		anchors.leftMargin: parent.width - btnSize * 2
		Image {
			source: "qrc:/Resource/redo_arrow.png"
			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter
		}
		onEntered: { color = "#eeeeee" }
		onExited: { color = "#ffffff" }
	}
	DropShadow {
		anchors.fill: undo
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: undo
	}
	DropShadow {
		anchors.fill: redo
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: redo
	}
}