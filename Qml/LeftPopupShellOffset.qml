import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1

LeftPopup {
	property var sidePadding: 16
	property var spaceOnTop: 44
	property var spaceBtwCtr: 44
	property var themeColor: "#00b9c8"
	
	featureName: qsTr("Shell Offset")
	leftPopupHeight: 220
	width: 256

	Text {
		id: popupbody
		text: qsTr("Click Apply to create a shell inside the \nmodel.")
		font.pointSize: 10
		font.family: openRegular.name
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: spaceOnTop
		anchors.leftMargin: sidePadding
	}

	CtrSpinBox {
		id: extendvalue
		labelText: "Tickness(mm)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: popupbody.bottom
		anchors.topMargin: spaceBtwCtr
	}
	
}