import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12


Hix.ProgressPopupShell {
	id:root
	width: window.width
	height: window.height
	property string errorMessage;

	MouseArea {
		id: blockingArea
		anchors.fill: parent
		propagateComposedEvents: false
        hoverEnabled: true
        preventStealing: true

		onClicked: { console.log(errorMessage); }
	}

	Rectangle {
		id: popup
		width: 216
		height: 216
		radius: 8
		color: "#ffffff"
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter

		property bool running: true

		Image {
			id: img
			source: "qrc:/Resource/progress_loading_yellow.png"
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top
			anchors.topMargin: 40

			RotationAnimation on rotation {
				from: 0
				to: 360
				duration: 1500
				running: popup.running
				loops: Animation.Infinite;
				easing.type: Easing.InOutQuad
				//easing.amplitude: 5
			}
		}

		Text {
			id: errorMsg
			text: root.errorMessage
			font.family: openSemiBold.name
			font.pointSize: 12
			color: "#f1820b"
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: img.bottom
			anchors.topMargin: 24
			//anchors.verticalCenter: img.verticalCenter
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