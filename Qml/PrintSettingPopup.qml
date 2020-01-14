import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item {
	property var themeColor: "#00b9c8"
	property var buttonsWidth: 320
	property var buttonSpace: 32

	Hix.ModalShell {
		id: modalshell
		width: 420
		height: 240
		radius: 8
		color: "#ffffff"

		Hix.CloseButton {
			width: 16
			height: 16
			color: transparent
			anchors.top: parent.top
			anchors.right: parent.right
			anchors.topMargin: 16
			anchors.rightMargin: 16
			Image {
				source: "qrc:/Resource/closebutton.png"
				anchors.top: parent.top
				anchors.right: parent.right
			}
		}
		
		Text {
			id: preset
			text: qsTr("Printer Preset")
			color: "#666666"
			font.family: openRegular.name
			font.pointSize: 10
			anchors.left: buttons.left
			anchors.top: parent.top
			anchors.topMargin: parent.height * 0.32
		}

		ComboBox {
			id: control
			anchors.left: buttons.left
			anchors.top: preset.bottom
			anchors.topMargin: 4
			model: parent.dropList
			width: parent.width * 0.68   //0.56
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

		Hix.RoundButton {
			id: refreshbtn
			width: 24
			height: width
			radius: width / 2
			color: "#ffffff"
			onEntered: { color = "#eeeeee" }
			onExited: { color = "#ffffff" }
			anchors.verticalCenter: control.verticalCenter
			anchors.left: control.right
			anchors.leftMargin: 8
			Image {
				source: "qrc:/Resource/btn_setting_refresh.png"
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
			}
		}

		Rectangle {
			id: buttons
			width: buttonsWidth
			height: 32
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.bottom: parent.bottom
			anchors.bottomMargin: parent.height * 0.2

			Hix.RoundButton {
				id: leftbtn
				width: (buttonsWidth - buttonSpace) / 2
				height: 32
				radius: height / 2
				color: "#00b9c8"	//themecolor
				anchors.left: parent.left
				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.verticalCenter: parent.verticalCenter
					color: "#FFFFFF"
					font.pointSize: 10
					text: "Save settings"
					font.family: openSemiBold.name
				}
				onClicked: { console.log("clicked"); }
				onEntered: { color = "#21959e" }
				onExited: { color = "#00b9c8" }
			}

			Hix.RoundButton {
				id: rightbtn
				width: buttonsWidth - leftbtn.width - buttonSpace
				height: 32
				radius: height / 2
				color: "#abb3b3"
				anchors.left: leftbtn.right
				anchors.leftMargin: buttonSpace

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.verticalCenter: parent.verticalCenter
					color: "#FFFFFF"
					font.pointSize: 10
					text: "Cancel"
					font.family: openSemiBold.name
				}
				onClicked: { console.log("clicked"); }
				onEntered: { color = "#8b9393" }
				onExited: { color = "#abb3b3" }
			}
		}
	}

	DropShadow {
		anchors.fill: modalshell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: modalshell
	}
}