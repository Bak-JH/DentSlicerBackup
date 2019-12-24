import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1


LeftPopup {
	property var sidePadding: 16
	property var spaceOnTop: 68
	property var spaceBtwCtr: 56
	property var themeColor: "#00b9c8"
	
	featureName: qsTr("Scale")
	leftPopupHeight: 336
	width: 256

	function lockToggle() {
		if (scalelock.source == "qrc:/Resource/img_scale_lock_revised.png") { scalelock.source = "qrc:/Resource/img_scale_unlock_revised.png" }
		else { scalelock.source = "qrc:/Resource/img_scale_lock_revised.png" }
	}

	CtrScaleInputBox {
		id: scaleX
		inputText: "X Size(mm)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: parent.top
		anchors.topMargin: spaceOnTop
	}

	CtrScaleInputBox {
		id: scaleY
		inputText: "Y Size(mm)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: scaleX.bottom
		anchors.topMargin: spaceBtwCtr
	}

	CtrScaleInputBox {
		id: scaleZ
		inputText: "Z Size(mm)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: scaleY.bottom
		anchors.topMargin: spaceBtwCtr
	}
	Hix.RoundButton {
		color: "transparent"
		width: 22
		height: 128
		anchors.verticalCenter: scaleY.verticalCenter
		anchors.right: scaleY.right
		Image {
			id: scalelock
			source: "qrc:/Resource/img_scale_lock_revised.png"
			anchors.left: parent.left
			anchors.leftMargin: 6
			//anchors.horizontalCenter: parent.horizontalCenter
		}
		onClicked: { lockToggle(); }
	}

	CtrInputBox {
		id: scale
		fromNum: 0
		toNum: 1000
		inputText: "Scale(%)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: scaleZ.bottom
		anchors.topMargin: spaceBtwCtr* 1.1
	}
}