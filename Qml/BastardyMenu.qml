import QtQuick 2.4
import hix.qml 1.0

import QtGraphicalEffects 1.12

Item {
	Rectangle {
		width: 1365
		height: 90
		id: bastardymenu
		radius: 45
		color: "#FFFFFF"

		MenuButton {
			id:open
			anchors.left: parent.left
			anchors.leftMargin: parent.radius
			iconBasic: "qrc:/Resource/menu_open.png"
			/*
			Image {
				width: 70
				height: 50
				anchors.horizontalCenter: parent.horizontalCenter
				source: "qrc:/Resource/vs_icon.png"
			}
			*/
		}
		Rectangle {
			id:divider1
			anchors.left: open.right
			anchors.verticalCenter: parent.verticalCenter
			width: 1
			height: 80
			color: "#dddddd"
		}
		MenuButton {
			id:move
			anchors.left: divider1.right
			iconBasic: "qrc:/Resource/menu_move.png"
		}
		MenuButton {
			id:rotate
			anchors.left: move.right
			iconBasic: "qrc:/Resource/menu_rotate.png"
		}
		MenuButton {
			id:scale
			anchors.left: rotate.right
			iconBasic: "qrc:/Resource/menu_scale.png"
		}
		Rectangle {
			id:divider2
			anchors.left: scale.right
			anchors.verticalCenter: parent.verticalCenter
			width: 1
			height: 80
			color: "#dddddd"
		}

		MenuButton {
			id:cut
			anchors.left: divider2.right
			iconBasic: "qrc:/Resource/menu_cut.png"
		}
		MenuButton {
			id:shelloffset
			anchors.left: cut.right
			iconBasic: "qrc:/Resource/menu_shelloffset.png"
		}
		MenuButton {
			id:extend
			anchors.left: shelloffset.right
			iconBasic: "qrc:/Resource/menu_extend.png"
		}
		MenuButton {
			id:label
			anchors.left:extend.right
			iconBasic: "qrc:/Resource/menu_label.png"
		}
		MenuButton {
			id:layflat
			anchors.left: label.right
			iconBasic: "qrc:/Resource/menu_layflat.png"
		}
		MenuButton {
			id:orient
			anchors.left: layflat.right
			iconBasic: "qrc:/Resource/menu_orient.png"
		}
		MenuButton {
			id:arrange
			anchors.left: orient.right
			iconBasic: "qrc:/Resource/menu_arrange.png"
		}
		MenuButton {
			id:support
			anchors.left: arrange.right
			iconBasic: "qrc:/Resource/menu_support.png"
		}
		MenuButton {
			id:autorepair
			anchors.left: support.right
			iconBasic: "qrc:/Resource/menu_autorepair.png"
		}
		Rectangle {
			id:divider3
			anchors.left: autorepair.right
			anchors.verticalCenter: parent.verticalCenter
			width: 1
			height: 80
			color: "#dddddd"
		}
		MenuButton {
			id:save
			anchors.left: divider3.right
			iconBasic: "qrc:/Resource/menu_save.png"
		}
		/*
		MenuButton {
			id:export
			anchors.left: save.right
		}
		*/
	}

	DropShadow {
		anchors.fill: bastardymenu
		radius: 10.0
        samples: 21
        color: "#55000000"
		source: bastardymenu
	}

}

