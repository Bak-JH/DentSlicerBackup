import QtQuick 2.4
import hix.qml 1.0

import QtGraphicalEffects 1.12

Item {
	property string hoverColor: "#eeeeee"
	property string defaultColor: "#ffffff"
	property string activeNameColor: "#1db2c4"

	property var menus: [open, move, rotate, scale, cut, shelloffset, extend, label, layflat, orient, arrange, support, save, extract]
	property var menusStr: ["open", "move", "rotate", "scale", "cut", "shelloffset", "extend", "label", "layflat", "orient", "arrange", "support", "save", "extract"]

	function menuClick(id) {							// get every MenuItem inactive but selected one
		if (menus) {
			for( var i = 0; i < menus.length; i++ ) {
				menus[i].nameColor = "#9d9d9d"
				menus[i].iconBasic = "qrc:/Resource/menu_"+menusStr[i]+".png"
			}
			id.nameColor = activeNameColor				//change selected button's text color
		}
	}
	
	Rectangle {
		width: 1365
		height: 90
		id: featureMenu
		radius: 45
		color: "#FFFFFF"

		MenuItem {
			id:open
			anchors.left: parent.left
			anchors.leftMargin: parent.radius
			iconBasic: "qrc:/Resource/menu_open.png"
			featureName: "Open"

			onClicked: { menuClick(open); color = defaultColor; nameColor = activeNameColor; iconBasic="qrc:/Resource/menu_open_select_1.png" }
			onEntered: {
				if (nameColor == activeNameColor) {}	// hover on already selected button
				else color = hoverColor	// hover on unselected button
			}
			onExited: {
				if (nameColor == activeNameColor) {}	// exit from already selected button
				else color = defaultColor	// exit from unselected button
			}
		}
		Rectangle {
			id:divider1
			anchors.left: open.right
			anchors.verticalCenter: parent.verticalCenter
			width: 1
			height: 80
			color: "#dddddd"
		}
		MenuItem {
			id:move
			anchors.left: divider1.right
			iconBasic: "qrc:/Resource/menu_move.png"
			featureName: "Move"

			onClicked: { menuClick(move); color = defaultColor; nameColor = activeNameColor; iconBasic="qrc:/Resource/menu_move_select_1.png" }
			onEntered: {
				if (nameColor == activeNameColor) {}	// hover on already selected button
				else color = hoverColor	// hover on unselected button
			}
			onExited: {
				if (nameColor == activeNameColor) {}	// exit from already selected button
				else color = defaultColor	// exit from unselected button
			}
		}
		MenuItem {
			id:rotate
			anchors.left: move.right
			iconBasic: "qrc:/Resource/menu_rotate.png"
			featureName: "Rotate"

			onClicked: { menuClick(rotate); color = defaultColor; nameColor = activeNameColor; iconBasic="qrc:/Resource/menu_rotate_select_1.png" }
			onEntered: {
				if (nameColor == activeNameColor) {}	// hover on already selected button
				else color = hoverColor	// hover on unselected button
			}
			onExited: {
				if (nameColor == activeNameColor) {}	// exit from already selected button
				else color = defaultColor	// exit from unselected button
			}
		}
		MenuItem {
			id:scale
			anchors.left: rotate.right
			iconBasic: "qrc:/Resource/menu_scale.png"
			featureName: "Scale"

			onClicked: { menuClick(scale); color = defaultColor; nameColor = activeNameColor; iconBasic="qrc:/Resource/menu_scale_select_1.png" }
			onEntered: {
				if (nameColor == activeNameColor) {}	// hover on already selected button
				else color = hoverColor	// hover on unselected button
			}
			onExited: {
				if (nameColor == activeNameColor) {}	// exit from already selected button
				else color = defaultColor	// exit from unselected button
			}
		}
		Rectangle {
			id:divider2
			anchors.left: scale.right
			anchors.verticalCenter: parent.verticalCenter
			width: 1
			height: 80
			color: "#dddddd"
		}

		MenuItem {
			id:cut
			anchors.left: divider2.right
			iconBasic: "qrc:/Resource/menu_cut.png"
			featureName: "Cut"

			onClicked: { menuClick(cut); color = defaultColor; nameColor = activeNameColor; iconBasic="qrc:/Resource/menu_cut_select_1.png" }
			onEntered: {
				if (nameColor == activeNameColor) {}	// hover on already selected button
				else color = hoverColor	// hover on unselected button
			}
			onExited: {
				if (nameColor == activeNameColor) {}	// exit from already selected button
				else color = defaultColor	// exit from unselected button
			}
		}
		MenuItem {
			id:shelloffset
			anchors.left: cut.right
			iconBasic: "qrc:/Resource/menu_shelloffset.png"
			featureName: "Shell Offset"

			onClicked: { menuClick(shelloffset); color = defaultColor; nameColor = activeNameColor; iconBasic="qrc:/Resource/menu_shelloffset_select_1.png" }
			onEntered: {
				if (nameColor == activeNameColor) {}	// hover on already selected button
				else color = hoverColor	// hover on unselected button
			}
			onExited: {
				if (nameColor == activeNameColor) {}	// exit from already selected button
				else color = defaultColor	// exit from unselected button
			}
		}
		MenuItem {
			id:extend
			anchors.left: shelloffset.right
			iconBasic: "qrc:/Resource/menu_extend.png"
			featureName: "Extend"

			onClicked: { menuClick(extend); color = defaultColor; nameColor = activeNameColor; iconBasic="qrc:/Resource/menu_extend_select_1.png" }
			onEntered: {
				if (nameColor == activeNameColor) {}	// hover on already selected button
				else color = hoverColor	// hover on unselected button
			}
			onExited: {
				if (nameColor == activeNameColor) {}	// exit from already selected button
				else color = defaultColor	// exit from unselected button
			}
		}
		MenuItem {
			id:label
			anchors.left:extend.right
			iconBasic: "qrc:/Resource/menu_label.png"
			featureName: "Label"

			onClicked: { menuClick(label); color = defaultColor; nameColor = activeNameColor; iconBasic="qrc:/Resource/menu_label_select_1.png" }
			onEntered: {
				if (nameColor == activeNameColor) {}	// hover on already selected button
				else color = hoverColor	// hover on unselected button
			}
			onExited: {
				if (nameColor == activeNameColor) {}	// exit from already selected button
				else color = defaultColor	// exit from unselected button
			}
		}
		MenuItem {
			id:layflat
			anchors.left: label.right
			iconBasic: "qrc:/Resource/menu_layflat.png"
			featureName: "Lay Flat"

			onClicked: { menuClick(layflat); color = defaultColor; nameColor = activeNameColor; iconBasic="qrc:/Resource/menu_layflat_select_1.png" }
			onEntered: {
				if (nameColor == activeNameColor) {}	// hover on already selected button
				else color = hoverColor	// hover on unselected button
			}
			onExited: {
				if (nameColor == activeNameColor) {}	// exit from already selected button
				else color = defaultColor	// exit from unselected button
			}
		}
		MenuItem {
			id:orient
			anchors.left: layflat.right
			iconBasic: "qrc:/Resource/menu_orient.png"
			featureName: "Orient"

			onClicked: { menuClick(orient); color = defaultColor; nameColor = activeNameColor; iconBasic="qrc:/Resource/menu_orient_select_1.png" }
			onEntered: {
				if (nameColor == activeNameColor) {}	// hover on already selected button
				else color = hoverColor	// hover on unselected button
			}
			onExited: {
				if (nameColor == activeNameColor) {}	// exit from already selected button
				else color = defaultColor	// exit from unselected button
			}
		}
		MenuItem {
			id:arrange
			anchors.left: orient.right
			iconBasic: "qrc:/Resource/menu_arrange.png"
			featureName: "Arrange"

			onClicked: { menuClick(arrange); color = defaultColor; nameColor = activeNameColor; iconBasic="qrc:/Resource/menu_arrange_select_1.png" }
			onEntered: {
				if (nameColor == activeNameColor) {}	// hover on already selected button
				else color = hoverColor	// hover on unselected button
			}
			onExited: {
				if (nameColor == activeNameColor) {}	// exit from already selected button
				else color = defaultColor	// exit from unselected button
			}
		}
		MenuItem {
			id:support
			anchors.left: arrange.right
			iconBasic: "qrc:/Resource/menu_support.png"
			featureName: "Support"

			onClicked: { menuClick(support); color = defaultColor; nameColor = activeNameColor; iconBasic="qrc:/Resource/menu_support_select_1.png" }
			onEntered: {
				if (nameColor == activeNameColor) {}	// hover on already selected button
				else color = hoverColor	// hover on unselected button
			}
			onExited: {
				if (nameColor == activeNameColor) {}	// exit from already selected button
				else color = defaultColor	// exit from unselected button
			}
		}	
		Rectangle {
			id:divider3
			anchors.left: support.right
			anchors.verticalCenter: parent.verticalCenter
			width: 1
			height: 80
			color: "#dddddd"
		}
		MenuItem {
			id:save
			anchors.left: divider3.right
			iconBasic: "qrc:/Resource/menu_save.png"
			featureName: "Save"

			onClicked: { menuClick(save); color = defaultColor; nameColor = activeNameColor; iconBasic="qrc:/Resource/menu_save_select_1.png" }
			onEntered: {
				if (nameColor == activeNameColor) {}	// hover on already selected button
				else color = hoverColor	// hover on unselected button
			}
			onExited: {
				if (nameColor == activeNameColor) {}	// exit from already selected button
				else color = defaultColor	// exit from unselected button
			}
		}
		MenuItem {
			id: extract
			anchors.left: save.right
			iconBasic: "qrc:/Resource/menu_extract.png"
			featureName: "Export"

			onClicked: { menuClick(extract); color = defaultColor; nameColor = activeNameColor; iconBasic="qrc:/Resource/menu_extract_select_1.png" }
			onEntered: {
				if (nameColor == activeNameColor) {}	// hover on already selected button
				else color = hoverColor	// hover on unselected button
			}
			onExited: {
				if (nameColor == activeNameColor) {}	// exit from already selected button
				else color = defaultColor	// exit from unselected button
			}
		}
		
	}

	DropShadow {
		anchors.fill: featureMenu
		radius: 10.0
        samples: 21
        color: "#55000000"
		source: featureMenu
	}

}

