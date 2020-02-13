import QtQuick 2.0
import QtQuick.Controls 2.1
import hix.qml 1.0 as Hix

Hix.ToggleSwitch {
	color: "transparent"
	height: 16
	property alias leftText: leftText
	property alias rightText: rightText
	Text {
		id: leftText
		font.family: openSemiBold.name
		font.pointSize: 8.5
		color: control.checked ? "#aaaaaa" : "#1db2c4"
		anchors.verticalCenter: parent.verticalCenter
	}
	Switch {
		id: control
		anchors.left: leftText.right
		anchors.verticalCenter: parent.verticalCenter
		checked: parent.isLeft

		indicator: Rectangle {
			implicitWidth: 28
			implicitHeight: 8
			x: control.leftPadding
			y: parent.height / 2 - height / 2
			radius: implicitHeight / 2
			color: "#ffffff"
			border.color: "#cccccc"
			anchors.verticalCenter: parent.verticalCenter

			Rectangle {
				x: control.checked ? parent.width - width : 0
				width: 16
				height: width
				radius: width / 2
				color: control.down ? "#cccccc" : "#ffffff"
				border.color: "#999999"
				anchors.verticalCenter: parent.verticalCenter
			}
		}

		onToggled: {
			isChecked = checked;
		}
	}
	Text {
		id: rightText
		anchors.left: leftText.right
		anchors.leftMargin: 40
		font.family: openSemiBold.name
		font.pointSize: 8.5
		color: control.checked ? "#1db2c4" : "#aaaaaa"
		anchors.verticalCenter: parent.verticalCenter
	}
}