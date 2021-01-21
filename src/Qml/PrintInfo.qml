import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.PrintInfoText {
	id: printInfo
	width :300
	height: 28
	onPrintsizeChanged: { 
		size.text = qsTr("<font color=\"#888888\">Size </font>") + printInfo.printsize; 
	}

	Rectangle {
		id: printinfo
		anchors.fill: parent
		radius: height / 2
		Text {
			id: size
			text: qsTr("<font color=\"#888888\">Size </font>") + printInfo.printsize
			font.family: openRegular.name
			font.pointSize: 10
			verticalAlignment: Text.AlignVCenter
			color: "#000000"
			anchors.left: parent.left
			anchors.leftMargin: 7
			anchors.verticalCenter: parent.verticalCenter
		}
		
	}
	DropShadow {
		anchors.fill: printinfo
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: printinfo
	}
}