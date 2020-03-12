import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls"

FeaturePopup {
	popuptitle.text: qsTr("Slice")
	height: 360

	CtrDropdownBox {
		id: layerheight
		dropdownText: "Layer Height"
		width: parent.width	 - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: parent.top
		anchors.topMargin: 64
	}

	CtrDropdownBox {
		id: resintype
		dropdownText: "Resin Type"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: layerheight.bottom
		anchors.topMargin: 44
	}

	CtrDropdownBox {
		id: infilltype
		dropdownText: "infill Type"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: resintype.bottom
		anchors.topMargin: 44
	}
	CtrDropdownBox {
		id: silcingmode
		dropdownText: "Slicing Mode"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: infilltype.bottom
		anchors.topMargin: 44
	}
	CtrDropdownBox {
		id: imageinversion
		dropdownText: "Image Inversion"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: silcingmode.bottom
		anchors.topMargin: 44
	}
	CtrDropdownBox {
		id: contractionratio
		dropdownText: "Contractin Ratio"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: imageinversion.bottom
		anchors.topMargin: 44
	}
}