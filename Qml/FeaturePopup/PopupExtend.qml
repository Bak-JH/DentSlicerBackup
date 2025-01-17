import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls"

FeaturePopup {
	title: qsTr("Extend")
	height: 220

	Text {
		id: popupbody
		text: qsTr("Click Apply to fix the model.")
		font.pointSize: 10
		font.family: openRegular.name
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 44
		anchors.leftMargin: 16
	}

	CtrSpinBox {
		id: extendvalue
		label.text: "Extend Value \n (mm)"
		fromNum: -100
		toNum: 100
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: popupbody.bottom
		anchors.topMargin: 52
	}
}
