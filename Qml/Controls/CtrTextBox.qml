import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.TextInputBox {
	property alias label: label
	Text {
		id: label
		text: qsTr("Text")
		font.family: openRegular.name
		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter
		color: "#666666"
	}

	TextField {
		id: control
		width: parent.width * 0.56 //0.6
		height: 26
		placeholderText: qsTr("Enter text here")
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		background: Rectangle {
			id: bgrect
			color: control.hovered ? "#ffffff" : "#f6feff"
			border.color: control.hovered ? "#b3bfc0": "#d3dfe0"
			radius: 2
		}

		onAccepted: {
			parent.inputText = text
		}
	}
}