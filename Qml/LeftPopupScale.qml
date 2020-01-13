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



	CtrScaleInputBox {
		id: scaleBox
	}

	CtrSpinBox {
		id: scale
		fromNum: 0
		toNum: 1000
		labelText: "Scale(%)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: scaleBox.bottom
		anchors.topMargin: spaceBtwCtr* 1.1
	}
}