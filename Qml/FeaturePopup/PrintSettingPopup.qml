import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12
import "../Controls"

FeaturePopup {
	title: qsTr("Settings")
	height: 180
	CtrDropdownBox {
		id: printerPreset
		label.text: "Printer Preset"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: parent.top
		anchors.topMargin: 55
	}
	CtrRectButton {
		id: logoutButton
		width: 224
		height: 32
		radius: 16
		color: "transparent"
		border.color: "#b2c4c6"

		anchors.top: printerPreset.bottom
		anchors.topMargin: 30
		anchors.horizontalCenter: parent.horizontalCenter

		Text 
		{
			color: "#859597"
			font.pointSize: 10
			text: "Logout"
			font.family: openSemiBold.name
				
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter
		}

		onEntered: { color = "#dde5e6" }
		onExited: { color = "transparent" }
	}


	// CtrButtonBase {
	// 	id: refreshButton
	// 	width: 26
	// 	height: width
	// 	radius: width / 2
	// 	color: "#ffffff"
	// 	onClicked: { }
	// 	onEntered: { color = "#eeeeee" }
	// 	onExited: { color = "#ffffff" }
	// 	anchors.verticalCenter: printerPreset.verticalCenter
	// 	anchors.left: printerPreset.right
	// 	anchors.leftMargin: 8
	// 	Image {
	// 		source: "qrc:/Resource/btn_setting_refresh.png"
	// 		//sourceSize.width: 18 * 1.2
	// 		//sourceSize.height: 16 * 1.2
	// 		anchors.horizontalCenter: parent.horizontalCenter
	// 		anchors.verticalCenter: parent.verticalCenter
	// 	}
	// }
}