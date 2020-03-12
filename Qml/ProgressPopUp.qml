import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12
import QtQuick.Layouts 1.3

Hix.ProgressPopupShell {
	id: root
	width: window.width
	height: window.height
	
	property string imgSource: "qrc:/Resource/progress_loading.png"

	// function reportError()
	// {
	// 	model.clear();
	// 	var newBtn = Qt.createComponent("ProgressException.qml");
	// 	var newBtnOption = newBtn.createObject(content);
	// }
	
	MouseArea {
		id: blockingArea
		anchors.fill: parent
		propagateComposedEvents: false
        hoverEnabled: true
        preventStealing: true
	}

	Rectangle {
		id: shell
		width: 216
		height: 216
		radius: 8
		color: "#ffffff"

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter

		Item {
			id: content
			width: parent.width
			height: parent.height

			Image {
				id: img
				source: imgSource
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter


				RotationAnimation on rotation {
					from: 0
					to: 360
					duration: 1200
					running: true
					loops: Animation.Infinite;
					easing.type: Easing.InOutQuad
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