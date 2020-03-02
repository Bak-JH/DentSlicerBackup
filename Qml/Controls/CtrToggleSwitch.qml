import QtQuick 2.0
import QtQuick.Controls 2.1
import hix.qml 1.0 as Hix

Hix.ToggleSwitch {
	property var sizeMult: 1.0
	color: "transparent"
	height: 16 * sizeMult
	id: root
	property alias leftText: leftText
	property alias rightText: rightText
	Text {
		id: leftText
		font.family: openSemiBold.name
		font.pointSize: 8.5 * sizeMult
		color: isChecked ? "#aaaaaa" : "#1db2c4"
		anchors.verticalCenter: parent.verticalCenter
	}
	Switch {
		id: control
		anchors.left: leftText.right
		anchors.verticalCenter: parent.verticalCenter
		checked: parent.isChecked

		indicator: Rectangle {
			implicitWidth: 28 * sizeMult
			implicitHeight: 8 * sizeMult
			x: control.leftPadding * sizeMult
			y: (parent.height / 2 - height / 2) * sizeMult
			radius: implicitHeight / 2
			color: "#ffffff"
			border.color: "#cccccc"
			anchors.verticalCenter: parent.verticalCenter

			Rectangle {
				x: root.isChecked ? (parent.width - width) : 0
				width: 16* sizeMult
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
		anchors.leftMargin: 40* sizeMult
		font.family: openSemiBold.name
		font.pointSize: 8.5* sizeMult
		color: root.isChecked ? "#1db2c4" : "#aaaaaa"
		anchors.verticalCenter: parent.verticalCenter
	}
}