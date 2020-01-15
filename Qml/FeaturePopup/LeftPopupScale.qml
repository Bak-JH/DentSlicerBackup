import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls"

LeftPopup {
	title: qsTr("Scale")
	leftPopupHeight: 336
	width: 256

	CtrScaleInputBox {
		id: scaleBox
	}

	CtrSpinBox {
		id: scale
		fromNum: 0
		toNum: 1000
		labelText: "Scale(%)"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: scaleBox.bottom
		anchors.topMargin: 56* 1.1
	}
}