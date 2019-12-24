import QtQuick 2.6
import hix.qml 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12


DropdownBox {
	Text {
		text: parent.dropName
		anchors.verticalCenter: parent.verticalCenter
		font.family: openRegular.name
		color: "#666666"
	}
	ComboBox {
		id: control
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		model: parent.dropList
		width: parent.width * 0.6
		height: 26
		font.family: openRegular.name

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
				//anchors.fill: parent.highlighted
				//height: control.height
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
			/*
			onPaint: {
				context.reset();
				context.moveTo(0, 0);
				context.lineTo(width, 0);
				context.lineTo(width / 2, height);
				context.closePath();
				context.fillStyle = control.pressed ? "#17a81a" : "#21be2b";
				context.fill();
			}
			*/
		}

		contentItem: Text {	// selected value on top, not in popup
			//leftPadding: 0
			leftPadding: 12
			//rightPadding: control.indicator.width + control.spacing
			text: control.displayText
			font: control.font
			//color: control.pressed ? "#17a81a" : "#000000"
			color: "#000000"
			verticalAlignment: Text.AlignVCenter
			elide: Text.ElideRight
		}

		background: Rectangle {
			id: bgrect
			color: control.hovered ? "#ffffff" : "#f6feff"
			border.color: control.hovered ? "#b3bfc0": "#d3dfe0"
			//border.width: control.visualFocus ? 2 : 1
			radius: 2
		}
		
		DropShadow {
			visible: false
			//visible: control.hovered ? true : false
			anchors.fill: bgrect
			radius: 4.0
			samples: 9
			color: "#30000000"
			source: bgrect
			Image {
				//source: control.popup.visible ? "qrc:/Resource/drop_arrow_up.png" : "qrc:/Resource/drop_arrow_down.png"
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
				//implicitHeight: contentHeight
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
						//visible: MouseArea.pressed ? true : false
					}
				}
			}

			background: Rectangle {	//popup rectangle
				id: drpopuprect
				border.color: "#b3bfc0"
				radius: 2
				/*
				layer.enabled: true
				layer.effect: DropShadow {
					//transparentBorder: false
					radius: 4.0
					samples: 9
					color: "#30000000"
				}
				*/
			}
			/*
			DropShadow {
				anchors.fill: drpopuprect
				radius: 4.0
				samples: 9
				color: "#30000000"
				source: drpopuprect
			}
			*/
		}
	}
}

