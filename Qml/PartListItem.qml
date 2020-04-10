import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1 //1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12
import "Controls"
Hix.PartListItem {
	id: root
	width: 224
	height: 28
	property bool isSelected: false
	property var labelObject
	property var mousePos
	onNameChanged: { modelnameText.text = root.modelName; }

	Column {
		anchors.verticalCenter: parent.verticalCenter
		Hix.Button{
			id: selectButton
			width: 224
			height: 28
			Text { 
				id: modelnameText
				text: root.modelName
				width: 190
				layer.enabled: true
				font.family: openRegular.name
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.leftMargin: 8
			}
			function setSelected(){
				isSelected = true
				color="#f5f5f5"
				modelnameText.color="#1db2c4"
				if(hideButton.isChecked)
					showhideimg.source = "qrc:/Resource/part_hide_select_1.png"
				else
					showhideimg.source = "qrc:/Resource/part_show_select_1.png"
			}
			function setUnselected(){
				isSelected = false
				color="white"
				modelnameText.color="black"
				if(hideButton.isChecked)
					showhideimg.source = "qrc:/Resource/part_hide_1.png"
				else
					showhideimg.source = "qrc:/Resource/part_show_1.png"
			}

			function generateFullnameBox(){
				var component = Qt.createComponent("PartListFullnameBox.qml")
				var mousePoint = root.mapToItem(null, root.mousePos.x, root.mousePos.y);
				root.labelObject = component.createObject(window, 
							{ modelName: root.modelName, x: mousePoint.x, y: mousePoint.y})
			}

			function deleteFullnameBox(){
				if(root.labelObject)
					root.labelObject.destroy()
			}
			
			
			onEntered: {
				holdTimer.start()
			}
			onExited: {
				holdTimer.stop()
				deleteFullnameBox();
			}
			
			onPositionChanged:{
				root.mousePos = Qt.point(mouse.x, mouse.y)
			}
			
			Timer {
			id:  holdTimer
			interval: 500
			running: false
			repeat: false
				onTriggered: {
					parent.generateFullnameBox();
				}
			}
		}
	}
	CtrToggleButton{
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

		onCheckedChanged: {
			if(isSelected)
				showhideimg.source = "qrc:/Resource/part_show_select_1.png"
			else
				showhideimg.source = "qrc:/Resource/part_show_1.png"
		}
		onChecked: {
			if(isSelected)
				showhideimg.source = "qrc:/Resource/part_hide_select_1.png"
			else
				showhideimg.source = "qrc:/Resource/part_hide_1.png"
		}
	}
}

