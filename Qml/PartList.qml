import QtQuick 2.6
import hix.qml 1.0
import QtQuick.Controls 2.1 //1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item {
	property var myPadding: 16

	Rectangle {
		id: partlistshell
		width: 256
		height: 320
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
		/*
		ScrollView {
			width: 224
			height: 200
			anchors.horizontalCenter: parent.horizontalCenter
			clip: true
			PartListContentBox {
				anchors.horizontalCenter: parent.horizontalCenter
			}
			PartListContentBox {
				anchors.horizontalCenter: parent.horizontalCenter
			}
		}
		*/
	
		Component {//����Ʈ ���� Ʋ�� �����.
			id: contactDelegate
			Item {
				width: 224; height: 28
				Column {
					anchors.verticalCenter: parent.verticalCenter
					Text { 
						text: name
						font.family: openRegular.name
					}
					
				}
				Image {
					source: "qrc:/Resource/part_show.png"
					anchors.right: parent.right
					anchors.rightMargin: width /2
					anchors.verticalCenter: parent.verticalCenter
					width: sourceSize.width / 1.5
					height: sourceSize.height / 1.5
				}
			}
		}

		ListModel {		// ����Ʈ�信 ���� �����͵��� ����.
			id:model
			ListElement { name: "model1.stl" }
			ListElement { name: "model2.stl" }
			ListElement { name: "model3.stl" }
		}

		Rectangle {
			width: 240;
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

		PartDeleteButton {
			width: 14
			color: "yellow"
			anchors.right: parent.right
			anchors.rightMargin: myPadding
			anchors.bottom: parent.bottom
			anchors.bottomMargin: myPadding
			Image {
				anchors.right: parent.right
				anchors.bottom: parent.bottom
				source: "qrc:/Resource/part_remove.png"
				sourceSize.width: width/1.5
			}
		}

	}
	DropShadow {
		anchors.fill: partlistshell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: partlistshell
	}
}