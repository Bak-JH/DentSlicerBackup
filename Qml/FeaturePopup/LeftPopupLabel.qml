import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import "../Controls"

LeftPopup {
	title: qsTr("Label")
	leftPopupHeight: 428
	width: 256

	Text {
		id: popupbody
		text: qsTr("Click to indicate the area on the part \n where label will be applied.")
		font.pointSize: 10
		font.family: openRegular.name
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 44
		anchors.leftMargin: 16
	}

	Image {
		id: labelimg
		source: "qrc:/Resource/img_label.png"
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: popupbody.bottom
		anchors.topMargin: 16
	}
	
	CtrTextBox {
		id: labeltext
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: labelimg.bottom
		anchors.topMargin: 32
	}
	
	CtrDropdownBox {
		id: labelfont
		labelText: "Font"
		dropList: Qt.fontFamilies()
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: labeltext.bottom
		anchors.topMargin: 44
	}

	CtrSpinBox {
		id: labelfontsize
		labelText: "Font Size"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: labelfont.bottom
		anchors.topMargin: 44
	}

	CtrSpinBox {
		id: labelheight
		labelText: "Label Height"
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: labelfontsize.bottom
		anchors.topMargin: 44
	}
	
	CtrToggleSwitch {
		id: toggleSwitch
		width: parent.width / 2
		anchors.top: labelheight.bottom
		anchors.topMargin: 16
		anchors.right: parent.right
		anchors.rightMargin: 16 - 2
	}
}