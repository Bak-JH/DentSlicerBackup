import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1

LeftPopup {
	property var sidePadding: 16
	property var topPadding: 64
	property var spaceBtwCtr: 48
	property var themeColor: "#00b9c8"
	
	featureName: qsTr("Orient")
	leftPopupHeight: 284
	width: 256

	Text {
		id: popupbody
		width: parent.width - sidePadding * 2
		text: qsTr("Click Apply to rotate models.")
		font.pointSize: 10
		font.family: openRegular.name
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: topPadding
		anchors.leftMargin: sidePadding
	}
}
