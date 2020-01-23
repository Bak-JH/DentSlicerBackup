import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls"

FeaturePopup {
	title: qsTr("Rotate")
	height: 284

	CtrSpinBox {
		id: rotateX
		fromNum: -360
		toNum: 360
		label.text: "X Offset(deg)"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: parent.top
		anchors.topMargin: 68
	}

	CtrSpinBox {
		id: rotateY
		fromNum: -360
		toNum: 360
		label.text: "Y Offset(deg)"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: rotateX.bottom
		anchors.topMargin: 56
	}

	CtrSpinBox {
		id: rotateZ
		fromNum: -360
		toNum: 360
		label.text: "Z Offset(deg)"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: rotateY.bottom
		anchors.topMargin: 56
	}
}