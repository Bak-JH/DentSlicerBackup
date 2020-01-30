import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

ModalWindow {
	id: root
	Item {
		parent: contentArea
		height: 45
		anchors.verticalCenter: parent.verticalCenter
		Text {
			id: preset
			text: qsTr("Printer Preset")
			color: "#666666"
			font.family: openRegular.name
			font.pointSize: 10
			anchors.left: parent.left
		}

		ComboBox {
			id: control
			anchors.left: preset.left
			anchors.top: preset.bottom
			anchors.topMargin: 4
			model: parent.dropList
			width: 420 * 0.68   //0.56
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

		Hix.Button {
			id: refreshbtn
			width: 26
			height: width
			radius: width / 2
			color: "#ffffff"
			onClicked: { console.log(preset.height); root.setButtons([["Apply", "#00b9c8", "#21959e"], ["Cancel", "#abb3b3", "#8b9393"]]) }
			onEntered: { color = "#eeeeee" }
			onExited: { color = "#ffffff" }
			anchors.verticalCenter: control.verticalCenter
			anchors.left: control.right
			anchors.leftMargin: 8
			Image {
				source: "qrc:/Resource/btn_setting_refresh.png"
				//sourceSize.width: 18 * 1.2
				//sourceSize.height: 16 * 1.2
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
			}
		}
		
	}
}