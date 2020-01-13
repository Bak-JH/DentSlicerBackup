import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item {
	width: 256
	height: 180

	function lockToggle() {
		if (scalelock.source == "qrc:/Resource/img_scale_lock_revised.png") { scalelock.source = "qrc:/Resource/img_scale_unlock_revised.png" }
		else { scalelock.source = "qrc:/Resource/img_scale_lock_revised.png" }
	}

	CtrSpinBox {
		id: scaleX
		labelText: "X Size(mm)"
		width: parent.width - sidePadding * 2
		control.anchors.rightMargin: width * 0.2
		control.width: width * 0.46 * 0.8
		control.up.indicator.anchors.leftMargin: control.width * 0.09 / 0.8
		control.up.indicator.implicitWidth: control.width * 0.11 / 0.8
		control.down.indicator.anchors.leftMargin: control.width * 0.09 / 0.8
		control.down.indicator.implicitWidth: control.width * 0.11 / 0.8
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: parent.top
		anchors.topMargin: spaceOnTop
	}

	CtrSpinBox {
		id: scaleY
		labelText: "Y Size(mm)"
		width: parent.width - sidePadding * 2
		control.anchors.rightMargin: width * 0.2
		control.width: width * 0.46 * 0.8
		control.up.indicator.anchors.leftMargin: control.width * 0.09 / 0.8
		control.up.indicator.implicitWidth: control.width * 0.11 / 0.8
		control.down.indicator.anchors.leftMargin: control.width * 0.09 / 0.8
		control.down.indicator.implicitWidth: control.width * 0.11 / 0.8
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: scaleX.bottom
		anchors.topMargin: spaceBtwCtr
	}

	CtrSpinBox {
		id: scaleZ
		labelText: "Z Size(mm)"
		width: parent.width - sidePadding * 2
		control.anchors.rightMargin: width * 0.2
		control.width: width * 0.46 * 0.8
		control.up.indicator.anchors.leftMargin: control.width * 0.09 / 0.8
		control.up.indicator.implicitWidth: control.width * 0.11 / 0.8
		control.down.indicator.anchors.leftMargin: control.width * 0.09 / 0.8
		control.down.indicator.implicitWidth: control.width * 0.11 / 0.8
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
}
