import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1

LeftPopup {
	property var sidePadding: 16
	property var spaceOnTop: 44
	property var themeColor: "#00b9c8"
	
	featureName: qsTr("Cut")
	leftPopupHeight: 250
	width: 256

	Text {
		id: popupbody
		//width: parent.width - sidePadding * 2
		//height: 26
		text: qsTr("Select how to cut the model.")
		font.pointSize: 10
		font.family: openRegular.name
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: spaceOnTop
		anchors.leftMargin: sidePadding
	}

	CtrCutSwitch {
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: popupbody.bottom
		anchors.topMargin: 20
	}

}
