import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1 //1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.PartListItem {
	id: root
	width: 224
	height: 28

	onNameChanged: { modelnameText.text = root.modelName; }

	Column {
		anchors.verticalCenter: parent.verticalCenter
		Hix.Button{
			id: selectButton
			width: 224; height: 28
			Text { 
				id: modelnameText
				text: root.modelName
				font.family: openRegular.name
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: 8
			}
			function setSelected(){
				color="#f5f5f5"
				modelnameText.color="#1db2c4"
				if(hideButton.isChecked)
					showhideimg.source = "qrc:/Resource/part_hide_select_1.png"
				else
					showhideimg.source = "qrc:/Resource/part_show_select_1.png"
			}
			function setUnselected(){
				color="white"
				modelnameText.color="black"
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

