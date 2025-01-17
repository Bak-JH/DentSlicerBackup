import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12
import "../Controls"
Item {
	width: 256
	height: 180

	function lockToggle() {
		if (scalelock.source == "qrc:/Resource/img_scale_lock_revised.png") { scalelock.source = "qrc:/Resource/img_scale_unlock_revised.png" }
		else { scalelock.source = "qrc:/Resource/img_scale_lock_revised.png" }
	}

	CtrSpinBox {
		id: scaleX
		label.text: "X Size(mm)"
		width: parent.width - 16 * 2
		control.anchors.rightMargin: width * 0.2
		control.width: width * 0.46 * 0.8
		control.up.indicator.anchors.leftMargin: control.width * 0.09 / 0.8
		control.up.indicator.implicitWidth: control.width * 0.11 / 0.8
		control.down.indicator.anchors.leftMargin: control.width * 0.09 / 0.8
		control.down.indicator.implicitWidth: control.width * 0.11 / 0.8
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: parent.top
		anchors.topMargin: 68
	}

	CtrSpinBox {
		id: scaleY
		label.text: "Y Size(mm)"
		width: parent.width - 16 * 2
		control.anchors.rightMargin: width * 0.2
		control.width: width * 0.46 * 0.8
		control.up.indicator.anchors.leftMargin: control.width * 0.09 / 0.8
		control.up.indicator.implicitWidth: control.width * 0.11 / 0.8
		control.down.indicator.anchors.leftMargin: control.width * 0.09 / 0.8
		control.down.indicator.implicitWidth: control.width * 0.11 / 0.8
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: scaleX.bottom
		anchors.topMargin: 56
	}

	CtrSpinBox {
		id: scaleZ
		label.text: "Z Size(mm)"
		width: parent.width - 16 * 2
		control.anchors.rightMargin: width * 0.2
		control.width: width * 0.46 * 0.8
		control.up.indicator.anchors.leftMargin: control.width * 0.09 / 0.8
		control.up.indicator.implicitWidth: control.width * 0.11 / 0.8
		control.down.indicator.anchors.leftMargin: control.width * 0.09 / 0.8
		control.down.indicator.implicitWidth: control.width * 0.11 / 0.8
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: scaleY.bottom
		anchors.topMargin: 56
	}

	CtrRectButton {
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
