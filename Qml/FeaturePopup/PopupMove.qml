import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls"

FeaturePopup {
	title: qsTr("Move")
	height: 268

	CtrSpinBox {
		id: moveX
		fromNum: -100
		toNum: 100
		label.text: "X Offset(mm)"
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
		label.text: "Y Offset(mm)"
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
		label.text: "Z Offset(mm)"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: moveY.bottom
		anchors.topMargin: 56
	}
}