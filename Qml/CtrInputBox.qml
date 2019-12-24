import QtQuick 2.6
import hix.qml 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

InputBox {
	//width: 224
	//height: 26

	property string inputText

	Text {
		//text: parent.propName
		text: inputText
		font.family: openRegular.name
		anchors.left: parent.left
		//anchors.top: parent.top
		//anchors.topMargin: height / 2
		color: "#666666"
		anchors.verticalCenter: parent.verticalCenter
		//verticalAlignment: Text.AlignVCenter
	}

	SpinBox {
		id: control
		font.family: openRegular.name
		value: 0
		width: parent.width * 0.5
		height: 26
		anchors.right: parent.right
		anchors.rightMargin: parent.width * 0.1
		anchors.verticalCenter: parent.verticalCenter
		editable: true
		from: parent.inputRect.from
		to: parent.inputRect.to

		contentItem: TextInput {
			z: 2
			text: control.textFromValue(control.value, control.locale)
			font: control.font
			selectionColor: "#21be2b"
			selectedTextColor: "#ffffff"
			//horizontalAlignment: Qt.AlignHCenter
			verticalAlignment: Qt.AlignVCenter

			readOnly: !control.editable
			validator: control.validator
			inputMethodHints: Qt.ImhFormattedNumbersOnly
		}

		up.indicator: Rectangle {
			id: uparrow
			//x: control.mirrored ? 0 : parent.width - width
			anchors.left: parent.right
			anchors.leftMargin: parent.width * 0.1 // this value + implicitWidth = parent.width * 0.2
			anchors.top: parent.top
			//y: 0
			
			implicitWidth: parent.width * 0.1 // this value + anchors.leftMargin = parent.width * 0.2
			//implicitHeight: parent.height / 2
			implicitHeight: implicitWidth * 0.9

			//color: control.up.pressed ? "#e4e4e4" : "#f6f6f6"
			//border.color: enabled ? "#21be2b" : "#bdbebf"
			
			Image {
				anchors.fill: parent
				source: "qrc:/Resource/triangle_up_1.5.png"
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
			}
			
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onReleased: { control.increase(); }
			}
		}

		down.indicator: Rectangle {
			//x: control.mirrored ? parent.width - width : 0
			anchors.left: parent.right
			anchors.leftMargin: parent.width * 0.1
			anchors.bottom: parent.bottom

			implicitWidth: parent.width * 0.1
			implicitHeight: implicitWidth * 0.9

			Image {
				anchors.fill: parent
				source: "qrc:/Resource/triangle_down_1.5.png"
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
			}
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onEntered: {}
				onExited: {}
				onReleased: { control.decrease(); }
			}
		}

		background: Rectangle {
			id: rect
			color: control.hovered ? "#ffffff" : "#f6feff"
			//border.color: "#d3dfe0"
			border.color: control.hovered ? "#b3bfc0": "#d3dfe0"
			radius: 2
		}
		DropShadow {
			//visible: control.hovered ? true : false
			visible: false
			anchors.fill: rect
			radius: 4.0
			samples: 9
			color: "#30000000"
			source: rect
		}
	}
}
