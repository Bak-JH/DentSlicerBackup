import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1

LeftPopup {
	title: qsTr("Arrange")
	height: 250

	Text {
		id: popupbody
		width: parent.width - 16 * 2
		height: 26
		text: qsTr("Click Apply to align models.")
		font.pointSize: 10
		font.family: openRegular.name
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 44
		anchors.leftMargin: 16
	}

	Image {
		source: "qrc:/Resource/img_arrange.png"
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: popupbody.bottom
		anchors.topMargin: 20
	}
}
