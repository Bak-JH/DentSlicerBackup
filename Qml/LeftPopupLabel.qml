import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1

LeftPopup {
	property var sidePadding: 16
	property var spaceOnTop: 44
	property var spaceBtwCtr: 44
	property var themeColor: "#00b9c8"
	
	featureName: qsTr("Label")
	leftPopupHeight: 428 //400
	width: 256

	Text {
		id: popupbody
		text: qsTr("Click to indicate the area on the part \n where label will be applied.")
		font.pointSize: 10
		font.family: openRegular.name
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: spaceOnTop
		anchors.leftMargin: sidePadding
	}

	Image {
		id: labelimg
		source: "qrc:/Resource/img_label.png"
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: popupbody.bottom
		anchors.topMargin: 16
	}

	CtrLabelTextBox {
		id: labeltext
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: labelimg.bottom
		anchors.topMargin: 32
	}

	CtrDropdownBox {
		id: labelfont
		dropdownText: "Font"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: labeltext.bottom
		anchors.topMargin: spaceBtwCtr
	}

	CtrInputBox {
		id: labelfontsize
		inputText: "Font Size"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: labelfont.bottom
		anchors.topMargin: spaceBtwCtr
	}

	CtrInputBox {
		id: labelheight
		inputText: "Label Height"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: labelfontsize.bottom
		anchors.topMargin: spaceBtwCtr
	}
	Rectangle {
		anchors.top: labelheight.bottom
		anchors.topMargin: 16
		anchors.right: parent.right
		anchors.rightMargin: sidePadding - 2
		color: "transparent"
		width: parent.width / 2
		height: 16
		Text {
			id: emboss
			text: "Emboss"
			font.family: openSemiBold.name
			font.pointSize: 8.5
			color: control.checked ? "#aaaaaa" : "#1db2c4"
			anchors.verticalCenter: parent.verticalCenter
		}
		Switch {
			id: control
			anchors.left: emboss.right
			anchors.verticalCenter: parent.verticalCenter
			//text: qsTr("Embossed / Engraved")

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
			/*
			contentItem: Text {
				text: "Embossed"
				opacity: enabled ? 1.0 : 0.3
				color: control.down ? "#17a81a" : "#21be2b"
				verticalAlignment: Text.AlignVCenter
				leftPadding: control.indicator.width + control.spacing
			}
			*/
		}
		Text {
			anchors.left: emboss.right
			anchors.leftMargin: 40
			text: "Engrave"
			font.family: openSemiBold.name
			font.pointSize: 8.5
			color: control.checked ? "#1db2c4" : "#aaaaaa"
			anchors.verticalCenter: parent.verticalCenter
		}
	}
	
}