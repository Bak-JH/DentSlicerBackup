import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1


LeftPopup {
	property var sidePadding: 16
	property var spaceOnTop: 68
	property var spaceBtwCtr: 56
	property var themeColor: "#00b9c8"
	
	featureName: qsTr("Rotate")
	leftPopupHeight: 284
	width: 256

	CtrSpinBox {
		id: rotateX
		fromNum: -360
		toNum: 360
		labelText: "X Offset(deg)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: parent.top
		anchors.topMargin: spaceOnTop
	}

	CtrSpinBox {
		id: rotateY
		fromNum: -360
		toNum: 360
		labelText: "Y Offset(deg)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: rotateX.bottom
		anchors.topMargin: spaceBtwCtr
	}

	CtrSpinBox {
		id: rotateZ
		fromNum: -360
		toNum: 360
		labelText: "Z Offset(deg)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: rotateY.bottom
		anchors.topMargin: spaceBtwCtr
	}
}