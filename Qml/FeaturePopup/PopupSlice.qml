import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls" 

FeaturePopup {
	title: qsTr("Slice")
	height: 220

	CtrDropdownBox {
		id: sliceType
		label.text: "Slicing Mode"
		width: parent.width - 16 * 2 - 16	// 16 = triangle height
		height: 26
		color: "transparent"
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 44
		anchors.leftMargin: 16
	}
	CtrSpinBox {
		id: layerHeight
		label.text: "Layer Height"
		width: parent.width - 16 * 2 - 16	// 16 = triangle height
		height: 26
		value: 0.1
		fromNum: -10
		toNum: 10
		increment: 0.01
		anchors.left: parent.left
		anchors.leftMargin: 32	// 16 = triangle height
		anchors.top: sliceType.bottom
		anchors.topMargin: 16
	}

	CtrToggleSwitch {
		id: invertX
		sizeMult: 1.2
		isChecked: false
		anchors.left: parent.left
		anchors.leftMargin: 32	// 16 = triangle height
		anchors.top: layerHeight.bottom
		anchors.topMargin: 16
		leftText
		{
			text: "No Inversion"
		}
		rightText
		{
			text: "Invert X"
		}
	}




}