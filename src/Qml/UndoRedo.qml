import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12
import "Controls"

Item {
	property var btnSize: 60
	width: 180
	height: btnSize

	CtrButtonBase {
		Rectangle
		{
			id: rectUndo
			anchors.fill: parent
			color: "#ffffff"
			radius: width / 2
			Image {
				source: "qrc:/Resource/undo_arrow.png"
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}
		}
		id: undo
		width: btnSize
		height: btnSize
		anchors.left: parent.left
		onEntered: { rectUndo.color = "#eeeeee" }
		onExited: { rectUndo.color = "#ffffff" }
	}
	CtrButtonBase {
		Rectangle
		{
			id: rectRedo
			anchors.fill: parent
			color: "#ffffff"
			radius: width / 2
			Image {
				source: "qrc:/Resource/redo_arrow.png"
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}
		}
		id: redo
		width: btnSize
		height: btnSize
		anchors.left: undo.right
		anchors.leftMargin: parent.width - btnSize * 2
		onEntered: { rectRedo.color = "#eeeeee" }
		onExited: { rectRedo.color = "#ffffff" }
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