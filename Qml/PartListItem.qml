import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1 //1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.PartListItem {
	width: 224; height: 28
	Column {
		anchors.verticalCenter: parent.verticalCenter
		Hix.ToggleSwitch{
			id: selectButton
			width: 224; height: 28
			Text { 
				id: modelname
				text: name
				font.family: openRegular.name
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: 8
			}
			onChecked: {
				color="#f5f5f5"
				modelname.color="#1db2c4"
				if(hideButton.isChecked)
					showhideimg.source = "qrc:/Resource/part_hide_select_1.png"
				else
					showhideimg.source = "qrc:/Resource/part_show_select_1.png"
			}
			onUnchecked: {
				color="white"
				modelname.color="black"
				if(hideButton.isChecked)
					showhideimg.source = "qrc:/Resource/part_hide_1.png"
				else
					showhideimg.source = "qrc:/Resource/part_show_1.png"
			}


		}
	}
	Hix.ToggleSwitch{
		id: hideButton
		width: 16
		height: width
		anchors.right: parent.right
		anchors.rightMargin: 6
		anchors.verticalCenter: parent.verticalCenter
		color: "transparent"
		Image {
			id: showhideimg
			source: "qrc:/Resource/part_show_1.png"
			anchors.verticalCenter: parent.verticalCenter
		}
		onChecked: {
			if(selectButton.isChecked)
				showhideimg.source = "qrc:/Resource/part_show_select_1.png"
			else
				showhideimg.source = "qrc:/Resource/part_show_1.png"
		}
		onUnchecked: {
			if(selectButton.isChecked)
				showhideimg.source = "qrc:/Resource/part_hide_select_1.png"
			else
				showhideimg.source = "qrc:/Resource/part_hide_1.png"
		}
	}
}

