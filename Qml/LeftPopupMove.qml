import QtQuick 2.6
import hix.qml 1.0
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1


LeftPopup {
	property var sidePadding: 16
	property var spaceOnTop: 68
	property var spaceBtwCtr: 56
	property var themeColor: "#00b9c8"

	title: qsTr("Move")
	leftPopupHeight: 268
	width: 256

	CtrInputBox {
		id: moveX
		fromNum: -100
		toNum: 100
		label.text: "X Offset(mm)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: parent.top
		anchors.topMargin: spaceOnTop
	}
	/*
	CtrInputBox {
		id: moveY
		fromNum: -100
		toNum: 100
		label.text: "Y Offset(mm)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: moveX.bottom
		anchors.topMargin: spaceBtwCtr
	}

	CtrInputBox {
		id: moveZ
		fromNum: -100
		toNum: 100
		label.text: "Z Offset(mm)"
		width: parent.width - sidePadding * 2
		anchors.left: parent.left
		anchors.leftMargin: sidePadding
		anchors.top: moveY.bottom
		anchors.topMargin: spaceBtwCtr
	}*/
}
/*
Item {
	property var myPadding: 16
	property var themeColor: "#00b9c8"

	LeftPopupShell {
		id: leftpopupshell
		width: 256
		height: 284
		//height: popupHeight
		color: "#F6FEFF"
		radius: 8

		Text {
			id: popuptitle
			text: qsTr("Move")
			font.pointSize: 11
			font.family: openRegular.name
			font.weight: Font.Bold
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.topMargin: myPadding
			anchors.leftMargin: myPadding
		}

		CloseButton {
			width: 16
			height: 16
			//color: "transparent"
			anchors.top: parent.top
			anchors.right: parent.right
			anchors.topMargin: myPadding
			anchors.rightMargin: myPadding
			Image {
				source: "qrc:/Resource/closebutton.png"
				anchors.top: parent.top
				anchors.right: parent.right
			}
		}

		CtrInputBox {
			id: moveX
			inputText: "X Offset"
			width: parent.width - myPadding * 2
			anchors.left: parent.left
			anchors.leftMargin: myPadding
			anchors.top: popuptitle.bottom
			anchors.topMargin: 32
		}

		CtrInputBox {
			id: moveY
			inputText: "Y Offset"
			width: parent.width - myPadding * 2
			anchors.left: parent.left
			anchors.leftMargin: myPadding
			anchors.top: moveX.bottom
			anchors.topMargin: 48
		}

		CtrInputBox {
			id: moveZ
			inputText: "Z Offset"
			width: parent.width - myPadding * 2
			anchors.left: parent.left
			anchors.leftMargin: myPadding
			anchors.top: moveY.bottom
			anchors.topMargin: 48
		}

		RoundedButton {
			anchors.bottom: parent.bottom
			anchors.bottomMargin: 16
			anchors.horizontalCenter: parent.horizontalCenter
		}
	}
	
	DropShadow {
		anchors.fill: leftpopupshell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: leftpopupshell
	}
}
*/