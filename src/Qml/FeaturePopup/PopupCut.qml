import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls"

FeaturePopup {
	title: qsTr("Cut")
	height: 250

	Text {
		id: popupbody
		text: qsTr("Select how to cut the model.")
		font.pointSize: 10
		font.family: openRegular.name
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 44
		anchors.leftMargin: 16
	}

	CtrToggleImgSwitch {
		id: cutswitch
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: popupbody.bottom
		anchors.topMargin: 20
		
		leftBtnImgSource: "qrc:/Resource/cut_flat"
		leftBtnText: "Flat Cut"

		rightBtnImgSource: "qrc:/Resource/cut_free"
		rightBtnText: "Free Cut"
		
	}

}
