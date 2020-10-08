import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls" 

FeaturePopup {
	title: qsTr("Printer")
	height: 200

	CtrDropdownBox {
		id: printerDrop
		label.text: "Printers"
		width: parent.width - 10	// 16 = triangle height
		comboBox.width: parent.width - label.width - 14
		height: 26
		color: "transparent"
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 44
		anchors.leftMargin: 4
	}

		Hix.Button {
		id: refreshButton
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
			text: "Refresh"
			font.family: openSemiBold.name
				
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter
		}

		onEntered: { color = "#dde5e6" }
		onExited: { color = "transparent" }
	}

}
