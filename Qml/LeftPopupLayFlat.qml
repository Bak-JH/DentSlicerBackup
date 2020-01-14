import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1

LeftPopup {
	title: qsTr("Lay Flat")
	leftPopupHeight: 220
	width: 256
		
	Text {
		id: popupbody
		text: "Click the surface to face it down."
		font.pointSize: 10
		font.family: openRegular.name
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 44
		anchors.leftMargin: 16
	}

	Image {
		source: "qrc:/Resource/img_layflat.png"
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: popupbody.bottom
		anchors.topMargin: 32
	}
}
