import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1

LeftPopup {
	title: qsTr("Orient")
	leftPopupHeight: 250
	width: 256

	Text {
		id: popupbody
		text: qsTr("Click Apply to rotate models.")
		font.pointSize: 10
		font.family: openRegular.name
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 44
		anchors.leftMargin: 16
	}

	Image {
		source: "qrc:/Resource/img_orient.png"
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: popupbody.bottom
		anchors.topMargin: 32
	}
}
