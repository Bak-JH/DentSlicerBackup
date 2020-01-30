import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12
//import QtMultimedia 5.14

Hix.ProgressPopupShell {
	id: shell
	width: window.width
	height: window.height
	color: "#00000000"

	ListModel // listview data
	{		
		id:model
		objectName: "featueList"

		function appendFeature(featureName)
		{
			model.append({"name" : featureName});
		}
	}
	
	MouseArea {
		id: blockingArea
		objectName: "blockingArea"
		anchors.fill: parent
		propagateComposedEvents: false
        hoverEnabled: true
        preventStealing: true
	}

	Rectangle {
		id: popup
		width: 216
		height: 300
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
				duration: 1200
				// running: root.runing
				loops: Animation.Infinite;
				easing.type: Easing.InOutQuad
			}
		}

		Component { // listview item style
			id: contactDelegate
			Item {
				width: parent.width * 0.92
				height: 28
				anchors.horizontalCenter: parent.horizontalCenter
				Column {
					anchors.verticalCenter: parent.verticalCenter
					Text { 
						id: modelname
						text: name
						font.family: openRegular.name
						font.pointSize: 10
						anchors.verticalCenter: parent.verticalCenter
						anchors.left: parent.left
						anchors.leftMargin: 8
					}
	
				}
				Rectangle{
					id: showhide
					width: 20
					height: width
					anchors.right: parent.right
					anchors.rightMargin: 8
					anchors.verticalCenter: parent.verticalCenter
					color: "yellow"
					Image {
						id: showhideimg
						source: "qrc:/Resource/progress_check.png"
						anchors.verticalCenter: parent.verticalCenter
					}
				}
			}
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
				model: model
				delegate: contactDelegate
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