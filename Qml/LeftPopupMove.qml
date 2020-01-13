import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1


LeftPopup {
	property var sidePadding: 16
	property var spaceOnTop: 68
	property var spaceBtwCtr: 56
	property var themeColor: "#00b9c8"

	title: qsTr("Move")
	leftPopupHeight: 268
	width: 256

	CtrSpinBox {
		id: moveX
		fromNum: -100
		toNum: 100
		labelText: "X Offset(mm)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: parent.top
		anchors.topMargin: spaceOnTop
	}
	
	CtrSpinBox {
		id: moveY
		fromNum: -100
		toNum: 100
		labelText: "Y Offset(mm)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: moveX.bottom
		anchors.topMargin: spaceBtwCtr
	}

	CtrSpinBox {
		id: moveZ
		fromNum: -100
		toNum: 100
		labelText: "Z Offset(mm)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: moveY.bottom
		anchors.topMargin: spaceBtwCtr
	}
}