import QtQuick 2.6
import hix.qml 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12


DropdownBox {
	property alias label: label
	Rectangle{
		color: "#F6FEFF"
		Text {
			id: label
			anchors.verticalCenter: parent.verticalCenter
			font.family: openRegular.name
			color: "#666666"
		}
	}
	ComboBox {
		id: control
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		model: parent.dropList
		width: parent.width * 0.56 //0.6
		height: 26
		font.family: openRegular.name
		currentIndex: parent.index
		onActivated: { parent.index = currentIndex }

		delegate: ItemDelegate {	// popup
			width: control.width
			contentItem: Text {
				text: modelData
				color: hovered ? "#ffffff" : "#000000"
				font: control.font
				elide: Text.ElideRight
				verticalAlignment: Text.AlignVCenter
			}
			highlighted: control.highlightedIndex === index
			background: Rectangle {
				color: hovered ? "#43becd" : "#ffffff"
			}
		}

		indicator: Canvas {
			id: canvas
			x: control.width - width - control.rightPadding
			y: control.topPadding + (control.availableHeight - height) / 2
			width: 12
			height: 8
			contextType: "2d"
			
			Connections {
				target: control
				onPressedChanged: canvas.requestPaint()
			}

			Image {
				source: control.popup.visible ? "qrc:/Resource/drop_arrow_up.png" : "qrc:/Resource/drop_arrow_down.png"
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
			}
		}

		contentItem: Text {	// selected value on top, not in popup
			leftPadding: 12
 			text: control.displayText
			font: control.font
			color: "#000000"
			verticalAlignment: Text.AlignVCenter
			elide: Text.ElideRight
		}

		background: Rectangle {
			id: bgrect
			color: control.hovered ? "#ffffff" : "#f6feff"
			border.color: control.hovered ? "#b3bfc0": "#d3dfe0"
			radius: 2
		}
		
		DropShadow {
			visible: false
			anchors.fill: bgrect
			radius: 4.0
			samples: 9
			color: "#30000000"
			source: bgrect
			Image {
				source: "qrc:/Resource/drop_arrow_up.png"
				anchors.right: parent.right
				anchors.rightMargin: control.rightPadding
				anchors.verticalCenter: parent.verticalCenter
			}
		}

		popup: Popup {
			y: control.height * 1.2
			width: control.width
			implicitHeight: contentItem.implicitHeight
			padding: 1

			contentItem: ListView {
				clip: true
				implicitHeight: contentHeight * 0.7
				model: control.popup.visible ? control.delegateModel : null
				currentIndex: control.highlightedIndex

				ScrollIndicator.vertical: ScrollIndicator {
					id: verticalIndicator
					//active: MouseArea.pressed
					orientation: Qt.Vertical
					size: frame.height / content.height
					position: -content.y / content.height
					anchors { top: parent.top; right: parent.right; bottom: parent.bottom }
					contentItem: Rectangle {
						implicitWidth: 6
						color: "#aaaaaaaa"
						radius: 1
					}
				}
			}

			background: Rectangle {	//popup rectangle
				id: drpopuprect
				border.color: "#b3bfc0"
				radius: 2
			}
		}
	}
}

