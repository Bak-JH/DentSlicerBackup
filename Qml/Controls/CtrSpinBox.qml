import QtQuick 2.12
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.InputSpinBox {
	id: root
	property alias control: control
	property alias label: label
	property double  fromNum
	property double  toNum
	
	onValueChanged:{ console.log(value); control.text = root.value.toFixed(2); }

	Text 
	{
		id: label
		text: ""
		font.family: openRegular.name
		anchors.left: parent.left
		color: "#666666"
		anchors.verticalCenter: control.verticalCenter
	}
	
	SpinBox {
		id: control
		font.family: openRegular.name
		value: parent.value
		from: fromNum
		to: toNum
		width: parent.width * 0.46//0.5
		height: 26
		anchors.right: parent.right
		anchors.rightMargin: parent.width * 0.1
		anchors.verticalCenter: parent.verticalCenter
		editable: true

		validator: DoubleValidator {
			bottom: fromNum
			top: toNum
		}

		contentItem: TextInput {
			font: control.font
			selectionColor: "#21be2b"
			selectedTextColor: "#ffffff"
			anchors.verticalCenter: control.verticalCenter
			verticalAlignment: Qt.AlignVCenter

			readOnly: !control.editable
			validator: control.validator
			inputMethodHints: Qt.ImhFormattedNumbersOnly
			layer.enabled: true
			Component.onCompleted:
			{
				text = root.value.toFixed(2);
			}
			onTextEdited:{
				root.value = parseFloat(text);
				console.log(root.value)
			}
		}
		
		up.indicator: Rectangle {
			id: uparrow
			anchors.left: parent.right
			anchors.leftMargin: parent.width * 0.09 // this value + implicitWidth = parent.width * 0.2
			anchors.top: parent.top
			
			implicitWidth: parent.width * 0.11 // this value + anchors.leftMargin = parent.width * 0.2
			implicitHeight: implicitWidth * 0.9			
		
			Image {
				id: upindicator
				anchors.fill: parent
				source: "qrc:/Resource/triangle_up.png"
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
			}
			
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onReleased: { root.value += root.increment }
				onEntered: { upindicator.source = "qrc:/Resource/triangle_up_hover.png" }
				onExited: { upindicator.source = "qrc:/Resource/triangle_up.png" }
			}
		}

		down.indicator: Rectangle {
			id: downarrow
			anchors.left: parent.right
			anchors.leftMargin: parent.width * 0.09 // this value + implicitWidth = parent.width * 0.2
			anchors.bottom: parent.bottom

			implicitWidth: parent.width * 0.11 // this value + anchors.leftMargin = parent.width * 0.2
			implicitHeight: implicitWidth * 0.9

			Image {
				id: downindicator
				anchors.fill: parent
				source: "qrc:/Resource/triangle_down.png"
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
			}
			MouseArea {
				anchors.fill: parent
				hoverEnabled: true
				onReleased: { root.value -= root.increment }
				onEntered: { downindicator.source = "qrc:/Resource/triangle_down_hover.png" }
				onExited: { downindicator.source = "qrc:/Resource/triangle_down.png" }
			}
		}
		
		background: Rectangle {
			id: rect
			color: control.hovered ? "#ffffff" : "#f6feff"
			border.color: control.hovered ? "#b3bfc0": "#d3dfe0"
			radius: 2
		}
		DropShadow {
			visible: false
			anchors.fill: rect
			radius: 4.0
			samples: 9
			color: "#30000000"
			source: rect
		}
		
	}
}
