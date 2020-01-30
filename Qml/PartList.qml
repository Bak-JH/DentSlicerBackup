import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1 //1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.PartList {
	id: root
	objectName: "partList"
	width: 256
	height: 320
	property var myPadding: 16
	
	function showHideToggle() {
		if(showhideimg.source == "qrc:/Resource/part_show_1.png") showhide.source = "qrc:/Resource/part_hide_1.png"
		else showhideimg.source = "qrc:/Resource/part_show_1.png"
	}

	ListModel {		// ����Ʈ�信 ���� �����͵��� ����.
		id:model
		objectName: "modelList"
		function appendModel(modelName, modelPointer)
		{
			model.append({"name" : modelName, "modelPointer" : modelPointer});
		}

		function deleteModel(modelPointer)
		{
			for(var n = 0; n < model.count; ++n)
			{
				if(model.get(n).modelPointer == modelPointer)
					model.remove(n)
			}
		}
	}

	Rectangle {
		id: shell
		width: root.width
		height: root.height
		radius: 8
		Text {
			id: listtitle
			text: qsTr("Part List")
			font.family: openSemiBold.name
			font.pointSize: 11
			anchors.top: parent.top
			anchors.topMargin: myPadding
			anchors.left: parent.left
			anchors.leftMargin: myPadding
		}
	
		Component { // ����Ʈ ���� Ʋ�� �����.
			id: contactDelegate
			Item {
				width: 224; height: 28
				Column {
					anchors.verticalCenter: parent.verticalCenter
					Hix.Button{
						width: 224; height: 28
						//color: "#f5f5f5"
						Text { 
							id: modelname
							text: name
							font.family: openRegular.name
							anchors.verticalCenter: parent.verticalCenter
							anchors.left: parent.left
							anchors.leftMargin: 8
						}
						onClicked: {
							color="#f5f5f5"
							modelname.color="#1db2c4"
							if(showhideimg.source == "qrc:/Resource/part_show_1.png") showhideimg.source = "qrc:/Resource/part_show_select_1.png"
							else if(showhideimg.source == "qrc:/Resource/part_hide_1.png") showhideimg.source = "qrc:/Resource/part_hide_select_1.png"
						}
					}
				}
				Hix.Button{
					id: showhide
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
					onClicked: {
						console.log("showhide clicked")
						if(showhideimg.source == "qrc:/Resource/part_show_1.png") showhideimg.source = "qrc:/Resource/part_hide_1.png"
						else if(showhideimg.source == "qrc:/Resource/part_hide_1.png") showhideimg.source = "qrc:/Resource/part_show_1.png"
						else if(showhideimg.source == "qrc:/Resource/part_show_select_1.png") showhideimg.source = "qrc:/Resource/part_hide_select_1.png"
						else if(showhideimg.source == "qrc:/Resource/part_hide_select_1.png") showhideimg.source = "qrc:/Resource/part_show_select_1.png"
					}
				}
			}
		}

		Rectangle {
			width: 256 - 16;
			height: 212;
			//anchors.horizontalCenter: parent.horizontalCenter
			anchors.right: parent.right
			anchors.top: listtitle.bottom
			anchors.topMargin: 8
			ListView {
				anchors.fill: parent
				clip: true
				width: 240;
				height: 212;
				model: model	// ����� �����͵��� ����Ʈ�信 �ִ´�.
				delegate: contactDelegate	// delegate: ����Ʈ �Ѱ��� Ʋ
				//highlight: Rectangle { color: "lightsteelblue"; radius: 2 }
				focus: true

				ScrollBar.vertical: ScrollBar{
					id: control
					//width: 12
					policy: ScrollBar.AlwaysOn
					contentItem: Rectangle {
						implicitWidth: 8
						//implicitHeight: 100
						radius: 2
						//color: "#aaaaaaaa"
						color: control.pressed ? "#eeaaaaaa" : "#aaaaaaaa"
					}
                }
			}
		}

		Hix.Button {
			width: 14
			height: width
			anchors.right: parent.right
			anchors.rightMargin: myPadding
			anchors.bottom: parent.bottom
			anchors.bottomMargin: myPadding
			Image {
				anchors.right: parent.right
				anchors.bottom: parent.bottom
				source: "qrc:/Resource/part_remove_1.png"
				//sourceSize.width: width
			}
			onClicked: { root.deleteModels() }//TODO: delete later
		}

	}
	DropShadow {
		anchors.fill: shell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: shell
	}
}