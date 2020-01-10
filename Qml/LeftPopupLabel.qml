import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12

LeftPopup {
	property var sidePadding: 16
	property var spaceOnTop: 44
	property var spaceBtwCtr: 44
	property var themeColor: "#00b9c8"
	
	featureName: qsTr("Label")
	leftPopupHeight: 428 //400
	width: 256

	Text {
		id: popupbody
		text: qsTr("Click to indicate the area on the part \n where label will be applied.")
		font.pointSize: 10
		font.family: openRegular.name
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: spaceOnTop
		anchors.leftMargin: sidePadding
	}

	Image {
		id: labelimg
		source: "qrc:/Resource/img_label.png"
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: popupbody.bottom
		anchors.topMargin: 16
	}

	CtrLabelTextBox {
		id: labeltext
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: labelimg.bottom
		anchors.topMargin: 32
	}

	CtrDropdownBox {
		id: labelfont
		dropName: "Font"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: labeltext.bottom
		anchors.topMargin: spaceBtwCtr
	}

	CtrInputBox {
		id: labelfontsize
		inputText: "Font Size"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: labelfont.bottom
		anchors.topMargin: spaceBtwCtr
	}

	CtrInputBox {
		id: labelheight
		inputText: "Label Height"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: labelfontsize.bottom
		anchors.topMargin: spaceBtwCtr
	}
	
	CtrToggleSwitch {
		id: toggleSwitch
		width: parent.width / 2
		anchors.top: labelheight.bottom
		anchors.topMargin: 16
		anchors.right: parent.right
		anchors.rightMargin: sidePadding - 2
	}
}