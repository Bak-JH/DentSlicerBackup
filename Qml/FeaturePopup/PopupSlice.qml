import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls" 

FeaturePopup {
	title: qsTr("Slice")
	height: 270

	CtrDropdownBox {
		id: sliceType
		label.text: "Slicing Mode"
		width: parent.width - 16 * 2 - 16	// 16 = triangle height
		height: 26
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 44
		anchors.leftMargin: 16
	}
	CtrSpinBoxInteger {
		id: layerHeight
		label.text: "Layer Height(µm)"
		width: parent.width - 13 * 2	// 16 = triangle height
		height: 26
		value: 100
		fromNum: 10
		toNum: 200
		increment: 10.0
		anchors.left: parent.left
		anchors.leftMargin: 16	// 16 = triangle height
		anchors.top: sliceType.bottom
		anchors.topMargin: 16
	}
	CtrSpinBox {
		id: aaxy
		label.text: "Antialiasing XY"
		width: parent.width - 13 * 2	// 16 = triangle height
		height: 26
		value: 1
		fromNum: 1
		toNum: 8
		increment: 1
		anchors.left: parent.left
		anchors.leftMargin: 16	// 16 = triangle height
		anchors.top: layerHeight.bottom
		anchors.topMargin: 16
	}
	CtrSpinBox {
		id: aaz
		label.text: "Antialiasing Z"
		width: parent.width - 13 * 2	// 16 = triangle height
		height: 26
		value: 1
		fromNum: 1
		toNum: 4
		increment: 1
		anchors.left: parent.left
		anchors.leftMargin: 16	// 16 = triangle height
		anchors.top: aaxy.bottom
		anchors.topMargin: 16
	}

}
