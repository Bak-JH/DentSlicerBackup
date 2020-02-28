import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls" 

FeaturePopup {
	title: qsTr("Extend")
	height: 186
	enableApply: false
	function settingToggle() {
		if (supportsettingpopup.visible == false) { supportsettingpopup.visible = true; }
		else { supportsettingpopup.visible = false; }
	}
	CtrTextButton {
		id: generatesupports
		width: 186
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 44
		anchors.leftMargin: 16
		buttonText: "Generate Supports"

	}

	Hix.Button {
		id: supportsettingbutton
		width: 32
		height: width
		radius: width / 2
		anchors.top: generatesupports.top
		anchors.right: parent.right
		anchors.rightMargin: 16
		Image {
			source: "qrc:/Resource/support_setting.png"
			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter
		}
		onClicked: { console.log("clicked"); settingToggle();}
		onEntered: { color = "#d2e0e1" }
		onExited: { color = "transparent" }
	}
	CtrTextButton {
		id: clearsupports
		anchors.top: generatesupports.bottom
		anchors.topMargin: 16
		anchors.horizontalCenter: parent.horizontalCenter
		buttonText: "Clear All Supports"
		// bgColor: "#00b9c8"
		// hoverColor: "#21959e"
		// textColor: "#FFFFFF"
	}

	CtrToggleSwitch {
		id: editsupports
		sizeMult: 1.2
		isChecked: false
		anchors.top: clearsupports.bottom
		anchors.topMargin: 16
		anchors.left: parent.left
		anchors.leftMargin: 30
		leftText
		{
			text: "Disable edit"
		}
		rightText
		{
			text: "Enable edit"
		}
		// width: parent.width / 2
		// anchors.horizontalCenter: parent.horizontalCenter
	}
	
	SidePopup {
		id: supportsettingpopup
		width: 296
		height: 224
		anchors.topMargin: 42
		// anchors.left: 42

		CtrDropdownBox {
			id: supporttype
			label.text: "Support Type"
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			height: 26
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: parent.top
			anchors.topMargin: 16
		}
		CtrDropdownBox {
			id: rafttype
			label.text: "Raft Type"
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			height: 26
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: supporttype.bottom
			anchors.topMargin: 16
		}
		CtrSpinBox {
			id: supportdensity
			label.text: "Support Density"
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			height: 26
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: rafttype.bottom
			anchors.topMargin: 16
		}
		CtrSpinBox {
			id: maxradius
			label.text: "Max.radius (mm)"
			height: 26
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: supportdensity.bottom
			anchors.topMargin: 16
		}
		CtrSpinBox {
			id: minradius
			label.text: "Min.radius (mm)"
			height: 26
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: maxradius.bottom
			anchors.topMargin: 16
		}


	}
}
