import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1


LeftPopup {
	property var sidePadding: 16
	property var topPadding: 64
	property var spaceBtwCtr: 48
	property var themeColor: "#00b9c8"
	
	featureName: qsTr("Rotate")
	leftPopupHeight: 284
	width: 256

	CtrInputBox {
		id: rotateX
		inputText: "X Offset(deg)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: parent.top
		anchors.topMargin: topPadding
	}

	CtrInputBox {
		id: rotateY
		inputText: "Y Offset(deg)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: rotateX.bottom
		anchors.topMargin: spaceBtwCtr
	}

	CtrInputBox {
		id: rotateZ
		inputText: "Z Offset(deg)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: rotateY.bottom
		anchors.topMargin: spaceBtwCtr
	}
}