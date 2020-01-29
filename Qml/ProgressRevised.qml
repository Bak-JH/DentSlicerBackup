import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12
//import QtMultimedia 5.14

Item {
	width: 216
	height: 300
	Rectangle {
		id: popup
		width: parent.width
		height: parent.height
		radius: 8
		color: "#ffffff"

		property bool running: true

		Image {
			id: img
			source: "qrc:/Resource/progress_loading.png"
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top
			anchors.topMargin: 40

			RotationAnimation on rotation {
				from: 0
				to: 360
				duration: 1500
				running: root.runing
				loops: Animation.Infinite;
				easing.type: Easing.InOutQuad
				//easing.amplitude: 5
			}
		}
		Text {
			text: qsTr("35") + "%"
			font.family: openSemiBold.name
			font.pointSize: 18
			color: "#1db2c4"
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: img.verticalCenter
		}

		Component { // 리스트 뷰의 틀을 만든다.
			id: contactDelegate
			Rectangle {
				width: parent.width * 0.9
				height: 28
				anchors.horizontalCenter: parent.horizontalCenter
				//color: "#eeeeee"
				radius: 4
				Column {
					anchors.verticalCenter: parent.verticalCenter
					Text { 
						id: modelname
						text: name
						color: "#666666"
						font.family: openRegular.name
						font.pointSize: 10
						anchors.verticalCenter: parent.verticalCenter
						anchors.left: parent.left
						anchors.leftMargin: 4
					}
	
				}
				Rectangle{
					id: check
					width: 22
					height: width
					anchors.right: parent.right
					anchors.rightMargin: 4
					anchors.verticalCenter: parent.verticalCenter
					color: "transparent"
					Image {
						id: checkimg
						source: "qrc:/Resource/progress_check.png"
						//anchors.fill: parent
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
					}
				}
			}
		}

		ListModel {		// 리스트뷰에 담을 데이터들을 선언.
			id:model
			ListElement { name: "Model import" }
			ListElement { name: "Mesh repair" }
			ListElement { name: "Auto arrange" }
		}

		Rectangle {
			width: parent.width
			height: parent.height * 0.3
			anchors.right: parent.right
			anchors.top: popup.top
			anchors.topMargin: 168
			ListView {
				anchors.fill: parent
				clip: true
				width: parent.width
				height: parent.height
				model: model	// 선언된 데이터들을 리스트뷰에 넣는다.
				delegate: contactDelegate	// delegate: 리스트 한개의 틀
				//highlight: Rectangle { color: "lightsteelblue"; radius: 2 }
				focus: true
			}
		}
	}
	
	DropShadow 
	{
		anchors.fill: popup
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: popup
	}
	
}