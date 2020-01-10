import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1

LeftPopup {
	property var sidePadding: 16
	property var spaceOnTop: 44
	property var spaceBtwCtr: 48
	property var themeColor: "#00b9c8"
	
	title: qsTr("Move")
	leftPopupHeight: 220
	width: 256
		
	Text {
		id: popupbody
		text: "Click the surface to face it down."
		font.pointSize: 10
		font.family: openRegular.name
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: spaceOnTop
		anchors.leftMargin: sidePadding
	}

	Image {
		source: "qrc:/Resource/img_layflat.png"
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: popupbody.bottom
		anchors.topMargin: 32
	}
}
