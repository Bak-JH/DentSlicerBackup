import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls"

FeaturePopup {
	title: qsTr("Shell Offset")
	height: 220

	Text {
		id: popupbody
		text: qsTr("Click Apply to create a shell inside the \nmodel.")
		font.pointSize: 10
		font.family: openRegular.name
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 44
		anchors.leftMargin: 16
	}

	CtrSpinBoxInteger {
		id: offsetValue
		fromNum: 2
		toNum: 100
		value: 3
		label.text: "Thickness(mm)"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: popupbody.bottom
		anchors.topMargin: 44
	}
	
}