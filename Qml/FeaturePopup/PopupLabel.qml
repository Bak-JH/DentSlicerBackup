import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import "../Controls"

FeaturePopup {
	title: qsTr("Label")
	height: 428

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
		label.text: "Font"
		dropList: Qt.fontFamilies()
		index: 0
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: labeltext.bottom
		anchors.topMargin: 44
	}

	CtrSpinBox {
		id: labelfontsize
		label.text: "Font Size"
		fromNum: 0
		toNum: 256
		value: 40
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: labelfont.bottom
		anchors.topMargin: 44
	}

	CtrSpinBox {
		id: labelheight
		label.text: "Label Height"
		fromNum: 0.1
		toNum: 100
		value: 4
		width: parent.width - 16 * 2
		anchors.left: parent.left
		anchors.leftMargin: 16
		anchors.top: labelfontsize.bottom
		anchors.topMargin: 44
	}
	
	CtrToggleSwitch {
		id: labeltype
		leftText
		{
			text: "Engrave"
		}
		rightText
		{
			text: "Emboss"
		}
		width: parent.width / 2
		anchors.top: labelheight.bottom
		anchors.topMargin: 16
		anchors.right: parent.right
		anchors.rightMargin: 16 - 2
	}
}