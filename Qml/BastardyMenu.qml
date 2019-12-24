import QtQuick 2.4
import hix.qml 1.0

import QtGraphicalEffects 1.12

Item {
	property string hoverColor: "#eeeeee"
	property string defaultColor: "#ffffff"
	property string activeNameColor: "#1db2c4"

	property var menus: [open, move, rotate, scale, cut, shelloffset, extend, label, layflat, orient, arrange, support, save, extract]
	property var menusStr: ["open", "move", "rotate", "scale", "cut", "shelloffset", "extend", "label", "layflat", "orient", "arrange", "support", "save", "extract"]

	function menuClick(id) {							// get every MenuButton inactive but selected one
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
		id: bastardymenu
		radius: 45
		color: "#FFFFFF"

		MenuButton {
			id:open
			anchors.left: parent.left
			anchors.leftMargin: parent.radius
			iconBasic: "qrc:/Resource/menu_open.png"
			featureName: "Open"
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onClicked: { menuClick(open); parent.color = defaultColor; parent.nameColor = activeNameColor; parent.iconBasic="qrc:/Resource/menu_open_select_1.png" }
				onEntered: {
					if (parent.nameColor == activeNameColor) {}	// hover on already selected button
					else parent.color = hoverColor	// hover on unselected button
				}
				onExited: {
					if (parent.nameColor == activeNameColor) {}	// exit from already selected button
					else parent.color = defaultColor	// exit from unselected button
				}
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
		MenuButton {
			id:move
			anchors.left: divider1.right
			iconBasic: "qrc:/Resource/menu_move.png"
			featureName: "Move"
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onClicked: { menuClick(move); parent.color = defaultColor; parent.nameColor = activeNameColor; parent.iconBasic="qrc:/Resource/menu_move_select_1.png" }
				onEntered: {
					if (parent.nameColor == activeNameColor) {}	// hover on already selected button
					else parent.color = hoverColor	// hover on unselected button
				}
				onExited: {
					if (parent.nameColor == activeNameColor) {}	// exit from already selected button
					else parent.color = defaultColor	// exit from unselected button
				}
			}
		}
		MenuButton {
			id:rotate
			anchors.left: move.right
			iconBasic: "qrc:/Resource/menu_rotate.png"
			featureName: "Rotate"
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onClicked: { menuClick(rotate); parent.color = defaultColor; parent.nameColor = activeNameColor; parent.iconBasic="qrc:/Resource/menu_rotate_select_1.png" }
				onEntered: {
					if (parent.nameColor == activeNameColor) {}	// hover on already selected button
					else parent.color = hoverColor	// hover on unselected button
				}
				onExited: {
					if (parent.nameColor == activeNameColor) {}	// exit from already selected button
					else parent.color = defaultColor	// exit from unselected button
				}
			}
		}
		MenuButton {
			id:scale
			anchors.left: rotate.right
			iconBasic: "qrc:/Resource/menu_scale.png"
			featureName: "Scale"
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onClicked: { menuClick(scale); parent.color = defaultColor; parent.nameColor = activeNameColor; parent.iconBasic="qrc:/Resource/menu_scale_select_1.png" }
				onEntered: {
					if (parent.nameColor == activeNameColor) {}	// hover on already selected button
					else parent.color = hoverColor	// hover on unselected button
				}
				onExited: {
					if (parent.nameColor == activeNameColor) {}	// exit from already selected button
					else parent.color = defaultColor	// exit from unselected button
				}
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

		MenuButton {
			id:cut
			anchors.left: divider2.right
			iconBasic: "qrc:/Resource/menu_cut.png"
			featureName: "Cut"
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onClicked: { menuClick(cut); parent.color = defaultColor; parent.nameColor = activeNameColor; parent.iconBasic="qrc:/Resource/menu_cut_select_1.png" }
				onEntered: {
					if (parent.nameColor == activeNameColor) {}	// hover on already selected button
					else parent.color = hoverColor	// hover on unselected button
				}
				onExited: {
					if (parent.nameColor == activeNameColor) {}	// exit from already selected button
					else parent.color = defaultColor	// exit from unselected button
				}
			}
		}
		MenuButton {
			id:shelloffset
			anchors.left: cut.right
			iconBasic: "qrc:/Resource/menu_shelloffset.png"
			featureName: "Shell Offset"
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onClicked: { menuClick(shelloffset); parent.color = defaultColor; parent.nameColor = activeNameColor; parent.iconBasic="qrc:/Resource/menu_shelloffset_select_1.png" }
				onEntered: {
					if (parent.nameColor == activeNameColor) {}	// hover on already selected button
					else parent.color = hoverColor	// hover on unselected button
				}
				onExited: {
					if (parent.nameColor == activeNameColor) {}	// exit from already selected button
					else parent.color = defaultColor	// exit from unselected button
				}
			}
		}
		MenuButton {
			id:extend
			anchors.left: shelloffset.right
			iconBasic: "qrc:/Resource/menu_extend.png"
			featureName: "Extend"
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onClicked: { menuClick(extend); parent.color = defaultColor; parent.nameColor = activeNameColor; parent.iconBasic="qrc:/Resource/menu_extend_select_1.png" }
				onEntered: {
					if (parent.nameColor == activeNameColor) {}	// hover on already selected button
					else parent.color = hoverColor	// hover on unselected button
				}
				onExited: {
					if (parent.nameColor == activeNameColor) {}	// exit from already selected button
					else parent.color = defaultColor	// exit from unselected button
				}
			}
		}
		MenuButton {
			id:label
			anchors.left:extend.right
			iconBasic: "qrc:/Resource/menu_label.png"
			featureName: "Label"
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onClicked: { menuClick(label); parent.color = defaultColor; parent.nameColor = activeNameColor; parent.iconBasic="qrc:/Resource/menu_label_select_1.png" }
				onEntered: {
					if (parent.nameColor == activeNameColor) {}	// hover on already selected button
					else parent.color = hoverColor	// hover on unselected button
				}
				onExited: {
					if (parent.nameColor == activeNameColor) {}	// exit from already selected button
					else parent.color = defaultColor	// exit from unselected button
				}
			}
		}
		MenuButton {
			id:layflat
			anchors.left: label.right
			iconBasic: "qrc:/Resource/menu_layflat.png"
			featureName: "Lay Flat"
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onClicked: { menuClick(layflat); parent.color = defaultColor; parent.nameColor = activeNameColor; parent.iconBasic="qrc:/Resource/menu_layflat_select_1.png" }
				onEntered: {
					if (parent.nameColor == activeNameColor) {}	// hover on already selected button
					else parent.color = hoverColor	// hover on unselected button
				}
				onExited: {
					if (parent.nameColor == activeNameColor) {}	// exit from already selected button
					else parent.color = defaultColor	// exit from unselected button
				}
			}
		}
		MenuButton {
			id:orient
			anchors.left: layflat.right
			iconBasic: "qrc:/Resource/menu_orient.png"
			featureName: "Orient"
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onClicked: { menuClick(orient); parent.color = defaultColor; parent.nameColor = activeNameColor; parent.iconBasic="qrc:/Resource/menu_orient_select_1.png" }
				onEntered: {
					if (parent.nameColor == activeNameColor) {}	// hover on already selected button
					else parent.color = hoverColor	// hover on unselected button
				}
				onExited: {
					if (parent.nameColor == activeNameColor) {}	// exit from already selected button
					else parent.color = defaultColor	// exit from unselected button
				}
			}
		}
		MenuButton {
			id:arrange
			anchors.left: orient.right
			iconBasic: "qrc:/Resource/menu_arrange.png"
			featureName: "Arrange"
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onClicked: { menuClick(arrange); parent.color = defaultColor; parent.nameColor = activeNameColor; parent.iconBasic="qrc:/Resource/menu_arrange_select_1.png" }
				onEntered: {
					if (parent.nameColor == activeNameColor) {}	// hover on already selected button
					else parent.color = hoverColor	// hover on unselected button
				}
				onExited: {
					if (parent.nameColor == activeNameColor) {}	// exit from already selected button
					else parent.color = defaultColor	// exit from unselected button
				}
			}
		}
		MenuButton {
			id:support
			anchors.left: arrange.right
			iconBasic: "qrc:/Resource/menu_support.png"
			featureName: "Support"
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onClicked: { menuClick(support); parent.color = defaultColor; parent.nameColor = activeNameColor; parent.iconBasic="qrc:/Resource/menu_support_select_1.png" }
				onEntered: {
					if (parent.nameColor == activeNameColor) {}	// hover on already selected button
					else parent.color = hoverColor	// hover on unselected button
				}
				onExited: {
					if (parent.nameColor == activeNameColor) {}	// exit from already selected button
					else parent.color = defaultColor	// exit from unselected button
				}
			}
		}
		/*
		MenuButton {
			id:autorepair
			anchors.left: support.right
			iconBasic: "qrc:/Resource/menu_autorepair.png"
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onClicked: { menuClick(autorepair); parent.color = defaultColor; parent.nameColor = activeNameColor; parent.iconBasic="qrc:/Resource/menu_autorepair_select_1.png" }
				onEntered: {
					if (parent.nameColor == activeNameColor) {}	// hover on already selected button
					else parent.color = hoverColor	// hover on unselected button
				}
				onExited: {
					if (parent.nameColor == activeNameColor) {}	// exit from already selected button
					else parent.color = defaultColor	// exit from unselected button
				}
			}
		}
		*/
		Rectangle {
			id:divider3
			anchors.left: support.right
			anchors.verticalCenter: parent.verticalCenter
			width: 1
			height: 80
			color: "#dddddd"
		}
		MenuButton {
			id:save
			anchors.left: divider3.right
			iconBasic: "qrc:/Resource/menu_save.png"
			featureName: "Save"
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onClicked: { menuClick(save); parent.color = defaultColor; parent.nameColor = activeNameColor; parent.iconBasic="qrc:/Resource/menu_save_select_1.png" }
				onEntered: {
					if (parent.nameColor == activeNameColor) {}	// hover on already selected button
					else parent.color = hoverColor	// hover on unselected button
				}
				onExited: {
					if (parent.nameColor == activeNameColor) {}	// exit from already selected button
					else parent.color = defaultColor	// exit from unselected button
				}
			}
		}
		MenuButton {
			id: extract
			anchors.left: save.right
			iconBasic: "qrc:/Resource/menu_extract.png"
			featureName: "Export"
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onClicked: { menuClick(extract); parent.color = defaultColor; parent.nameColor = activeNameColor; parent.iconBasic="qrc:/Resource/menu_extract_select_1.png" }
				onEntered: {
					if (parent.nameColor == activeNameColor) {}	// hover on already selected button
					else parent.color = hoverColor	// hover on unselected button
				}
				onExited: {
					if (parent.nameColor == activeNameColor) {}	// exit from already selected button
					else parent.color = defaultColor	// exit from unselected button
				}
			}
		}
		
	}

	DropShadow {
		anchors.fill: bastardymenu
		radius: 10.0
        samples: 21
        color: "#55000000"
		source: bastardymenu
	}

}

