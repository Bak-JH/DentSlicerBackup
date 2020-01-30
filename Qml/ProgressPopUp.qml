import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.ProgressPopupShell {
	id: root
	width: 216
	height: 300

	property string imgSource: "qrc:/Resource/progress_loading.png"

	function reportError()
	{
		model.clear();
		var newBtn = Qt.createComponent("ProgressException.qml");
		var newBtnOption = newBtn.createObject(content);
	}

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
		width: window.width
		height: window.height
		anchors.fill: window
		propagateComposedEvents: false
        hoverEnabled: true
        preventStealing: true
	}

	Rectangle {
		id: shell
		width: parent.width
		height: parent.height
		radius: 8
		color: "#ffffff"

		Item {
			id: content
			width: parent.width
			height: parent.height

			Image {
				id: img
				source: imgSource
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.top: parent.top
				anchors.topMargin: 40

				RotationAnimation on rotation {
					from: 0
					to: 360
					duration: 1200
					running: true
					loops: Animation.Infinite;
					easing.type: Easing.InOutQuad
				}
			}

			Text {
				id: percentage
				text: root.percent + "%"
				font.family: openSemiBold.name
				font.pointSize: 18
				color: "#1db2c4"
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: img.verticalCenter
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
				anchors.top: img.bottom
				anchors.topMargin: 10
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
	}
	
	DropShadow 
	{
		anchors.fill: shell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: shell
	}
	
}