import QtQuick 2.6
import hix.qml 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12


Item {
	Rectangle {
		id: progressshell
		width: 720
		height: 100
		radius: width / 2
		color: "#ffffff"

		Text {
			text: "something"
			font.family: openRegular.name
			anchors.horizontalCenter: parent.horizontalCenter
		}

	}
	
	DropShadow {
		anchors.fill: progressshell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: progressshell
	}
	
}
