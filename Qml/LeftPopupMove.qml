import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1


LeftPopup {
	title: qsTr("Move")
	leftPopupHeight: 320
	width: 256
	height: leftPopupHeight

	CtrSpinBox {
		id: moveX
		fromNum: -100
		toNum: 100
		labelText: "X Offset(mm)"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: parent.top
		anchors.topMargin: 68
	}
	
	CtrSpinBox {
		id: moveY
		fromNum: -100
		toNum: 100
		labelText: "Y Offset(mm)"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: moveX.bottom
		anchors.topMargin: 56
	}

	CtrSpinBox {
		id: moveZ
		fromNum: -100
		toNum: 100
		labelText: "Z Offset(mm)"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: moveY.bottom
		anchors.topMargin: 56
	}

	Hix.RoundButton {
		id: snapButton
		width: 224
		height: 32
		radius: 16
		color: "transparent"
		border.color: "#b2c4c6"

		anchors.bottom: parent.bottom
		anchors.bottomMargin: 64
		anchors.horizontalCenter: parent.horizontalCenter

		Text 
		{
			color: "#859597"
			font.pointSize: 10
			text: "Snap to ground"
			font.family: openSemiBold.name
				
			anchors.horizontalCenter: snapButton.horizontalCenter
			anchors.verticalCenter: snapButton.verticalCenter
		}

		onClicked: { console.log("clicked"); }
		onEntered: { color = "#dde5e6" }
		onExited: { color = "transparent" }
	}
}