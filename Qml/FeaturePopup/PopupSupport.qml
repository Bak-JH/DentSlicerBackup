import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls" 

FeaturePopup {
	title: qsTr("Support")
	height: 260
	enableApply: false
	function supportSettingToggle() {
		if (supportsettingpopup.visible == false) {
			supportsettingpopup.visible = true; 
			raftsettingpopup.visible = false;

		}
		else { 
			supportsettingpopup.visible = false;
		}
	}
	function raftSettingToggle() {
		if (raftsettingpopup.visible == false) {
			raftsettingpopup.visible = true;
			supportsettingpopup.visible = false;
	
		}
		else {
			raftsettingpopup.visible = false;
		}
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

	CtrTextButton {
		id: generateraft
		width: 186
		anchors.top: generatesupports.bottom
		anchors.left: parent.left
		anchors.topMargin: 16
		anchors.leftMargin: 16
		buttonText: "Generate Raft"
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
		onClicked: { supportSettingToggle();}
		onEntered: { color = "#d2e0e1" }
		onExited: { color = "transparent" }
	}
	Hix.Button {
		id: raftsettingbutton
		width: 32
		height: width
		radius: width / 2
		anchors.top: generateraft.top
		anchors.right: parent.right
		anchors.rightMargin: 16
		Image {
			source: "qrc:/Resource/support_setting.png"
			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter
		}
		onClicked: { raftSettingToggle();}
		onEntered: { color = "#d2e0e1" }
		onExited: { color = "transparent" }
	}
	CtrTextButton {
		id: clearsupports
		anchors.top: generateraft.bottom
		anchors.topMargin: 16
		anchors.horizontalCenter: parent.horizontalCenter
		buttonText: "Clear All Supports"
		// bgColor: "#00b9c8"
		// hoverColor: "#21959e"
		// textColor: "#FFFFFF"
	}

	CtrTextButton {
		id: reconnect
		anchors.top: clearsupports.bottom
		anchors.topMargin: 16
		anchors.horizontalCenter: parent.horizontalCenter
		buttonText: "Generate Interconnects"
	}

	CtrToggleSwitch {
		id: editsupports
		sizeMult: 1.2
		isChecked: true
		anchors.top: reconnect.bottom
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
		height: 305
		anchors.topMargin: 38
		anchors.left: generatesupports.right
		anchors.leftMargin: 30

		CtrDropdownBox {
			id: supporttype
			label.text: "Support Type"
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			height: 26
			color: "transparent"
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: parent.top
			anchors.topMargin: 16
		}
		CtrDropdownBox {
			id: interconnecttype
			label.text: "Interconnect"
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			height: 26
			color: "transparent"
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: supporttype.bottom
			anchors.topMargin: 16
		}
		CtrSpinBox {
			id: supportdensity
			label.text: "Support Density(%)"
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			height: 26
			increment: 5
			fromNum: 5
			toNum: 100
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: interconnecttype.bottom
			anchors.topMargin: 16
		}
		CtrSpinBox {
			id: maxradius
			label.text: "Max radius (mm)"
			height: 26
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			increment: 0.05
			fromNum: 0.1
			toNum: 5
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: supportdensity.bottom
			anchors.topMargin: 16
		}
		CtrSpinBox {
			id: minradius
			label.text: "Min radius (mm)"
			height: 26
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			increment: 0.05
			fromNum: 0.1
			toNum: 5
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: maxradius.bottom
			anchors.topMargin: 16
		}
		CtrSpinBox {
			id: supportBaseHeight
			label.text: "Min height (mm)"
			height: 26
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			increment: 0.1
			fromNum: 1.0
			toNum: 20
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: minradius.bottom
			anchors.topMargin: 16
		}
		CtrSpinBox {
			id: maxConnectDistance
			label.text: "Max connect dist (mm)"
			height: 26
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			increment: 0.1
			fromNum: 0.0
			toNum: 10000
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: supportBaseHeight.bottom
			anchors.topMargin: 16
		}
	}
	SidePopup {
		id: raftsettingpopup
		width: 296
		height: 250
		anchors.topMargin: 72
		anchors.left: generateraft.right
		anchors.leftMargin: 30
		CtrDropdownBox {
			id: rafttype
			label.text: "Raft Type"
			height: 26
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			color: "transparent"
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: parent.top
			anchors.topMargin: 16
		}
		CtrSpinBox {
			id: raftRadiusMult
			label.text: "Radius mult(support.max)"
			height: 26
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			increment: 0.1
			fromNum: 0.5
			toNum: 100.0
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: rafttype.bottom
			anchors.topMargin: 16
		}
		CtrSpinBox {
			id: raftMinMaxRatio
			label.text: "Vertical ratio"
			height: 26
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			increment: 0.05
			fromNum: 1.0
			toNum: 10.0
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: raftRadiusMult.bottom
			anchors.topMargin: 16
		}
		CtrSpinBox {
			id: raftThickness
			label.text: "Thickness"
			height: 26
			width: parent.width - 16 * 2 - 16	// 16 = triangle height
			increment: 0.05
			fromNum: 0.05
			toNum: 10.0
			anchors.left: parent.left
			anchors.leftMargin: 32	// 16 = triangle height
			anchors.top: raftMinMaxRatio.bottom
			anchors.topMargin: 16
		}
	}
}
