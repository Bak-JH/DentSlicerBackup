import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.TextInputBox {
	id: root
	property alias label: label
	Text {
		id: label
		text: qsTr("Text")
		font.family: openRegular.name
		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter
		color: "#666666"
	}

	Rectangle {
		id: control
		width: parent.width * 0.56 //0.6
		height: 26
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		color: "#f6feff"
		border.color: "#d3dfe0"
		radius: 2

		MouseArea
		{
			anchors.fill: parent
			hoverEnabled: true
			onEntered: {
				control.color = "#ffffff"
				control.border.color = "#b3bfc0"
			}
			onExited: {
				control.color = "#f6feff"
				control.border.color = "#d3dfe0"
			}
		}

		TextInput{
			width: parent.width - 3
			height: parent.height
			font.family: openRegular.name
			verticalAlignment :Qt.AlignVCenter
			leftPadding : 10
			layer.enabled: true
			
			onTextChanged: {
				root.inputText = text
			}
			
		}
	}
}