import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1

LeftPopup {
	property var sidePadding: 16
	property var spaceOnTop: 64
	property var spaceBtwCtr: 44
	property var themeColor: "#00b9c8"
	
	featureName: qsTr("Export")
	leftPopupHeight: 360
	width: 256

	CtrDropdownBox {
		id: layerheight
		dropdownText: "Layer Height"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: parent.top
		anchors.topMargin: spaceOnTop
	}

	CtrDropdownBox {
		id: resintype
		dropdownText: "Resin Type"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: layerheight.bottom
		anchors.topMargin: spaceBtwCtr
	}

	CtrDropdownBox {
		id: infilltype
		dropdownText: "infill Type"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: resintype.bottom
		anchors.topMargin: spaceBtwCtr
	}
	CtrDropdownBox {
		id: silcingmode
		dropdownText: "Slicing Mode"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: infilltype.bottom
		anchors.topMargin: spaceBtwCtr
	}
	CtrDropdownBox {
		id: imageinversion
		dropdownText: "Image Inversion"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: silcingmode.bottom
		anchors.topMargin: spaceBtwCtr
	}
	CtrDropdownBox {
		id: contractionratio
		dropdownText: "Contractin Ratio"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: imageinversion.bottom
		anchors.topMargin: spaceBtwCtr
	}
}