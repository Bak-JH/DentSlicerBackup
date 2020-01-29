import QtQuick 2.4
import hix.qml 1.0

import QtGraphicalEffects 1.12

Item {

	property string activeNameColor: "#1db2c4"
	
	Rectangle {
		width: 1365
		height: 90
		id: featureMenu
		radius: 45
		color: "#FFFFFF"
		RowLayout {
            id: featureItems
			spacing: parent.radius
			MenuItem {
				id:open
				iconBasic: "qrc:/Resource/menu_open.png"
				iconSelected: "qrc:/Resource/menu_open_select_1.png" 
				featureName: "Open"
			}
			Rectangle {
				id:divider1
				width: 1
				height: 80
				color: "#dddddd"
			}
			MenuItem {
				id:move
				iconBasic: "qrc:/Resource/menu_move.png"
				iconSelected: "qrc:/Resource/menu_move_select_1.png" 
				featureName: "Move"
			}
			MenuItem {
				id:rotate
				iconBasic: "qrc:/Resource/menu_rotate.png"
				iconSelected: "qrc:/Resource/menu_rotate_select_1.png" 
				featureName: "Rotate"
			}
			MenuItem {
				id:scale
				iconBasic: "qrc:/Resource/menu_scale.png"
				iconSelected: "qrc:/Resource/menu_scale_select_1.png" 
				featureName: "Scale"
			}
			Rectangle {
				id:divider2
				width: 1
				height: 80
				color: "#dddddd"
			}

			MenuItem {
				id:cut
				iconBasic: "qrc:/Resource/menu_cut.png"
				iconSelected: "qrc:/Resource/menu_cut_select_1.png" 
				featureName: "Cut"
			}
			MenuItem {
				id:shelloffset
				iconBasic: "qrc:/Resource/menu_shelloffset.png"
				iconSelected: "qrc:/Resource/menu_shelloffset_select_1.png"
				featureName: "Shell Offset"
			}
			MenuItem {
				id:extend
				iconBasic: "qrc:/Resource/menu_extend.png"
				iconSelected: "qrc:/Resource/menu_extend_select_1.png"
				featureName: "Extend"
			}
			MenuItem {
				id:label
				iconBasic: "qrc:/Resource/menu_label.png"
				iconSelected: "qrc:/Resource/menu_label_select_1.png"
				featureName: "Label"
			}
			MenuItem {
				id:layflat
				iconBasic: "qrc:/Resource/menu_layflat.png"
				iconSelected: "qrc:/Resource/menu_layflat_select_1.png"
				featureName: "Lay Flat"
			}
			MenuItem {
				id:orient
				iconBasic: "qrc:/Resource/menu_orient.png"
				iconSelected: "qrc:/Resource/menu_orient_select_1.png"
				featureName: "Orient"
			}
			MenuItem {
				id:arrange
				iconBasic: "qrc:/Resource/menu_arrange.png"
				iconSelected: "qrc:/Resource/menu_arrange_select_1.png"
				featureName: "Arrange"
			}
			MenuItem {
				id:support
				iconBasic: "qrc:/Resource/menu_support.png"
				iconSelected: "qrc:/Resource/menu_support_select_1.png"
				featureName: "Support"
			}	
			Rectangle {
				id:divider3
				width: 1
				height: 80
				color: "#dddddd"
			}
			MenuItem {
				id:save
				iconBasic: "qrc:/Resource/menu_save.png"
				iconSelected: "qrc:/Resource/menu_save_select_1.png"
				featureName: "Save"
			}
			MenuItem {
				id: extract
				iconBasic: "qrc:/Resource/menu_extract.png"
				iconSelected: "qrc:/Resource/menu_extract_select_1.png"
				featureName: "Export"
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

