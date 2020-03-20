import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12
import "FeaturePopup"
import "Controls" 

Item {
	id: root
	width: 256
	// width: 235

	height: 50
	Rectangle {
		id: shell
		width: parent.width
		height: parent.height
		radius: 8
		CtrToggleSwitch {
			id: toggleView
			sizeMult: 1.3
			isChecked: false
			anchors.top: parent.top
			anchors.topMargin: 16
			anchors.left: parent.left
			anchors.leftMargin: 20
			leftText
			{
				text: "Object view"
			}
			rightText
			{
				text: "Layer view"
			}
			// width: parent.width / 2
			// anchors.horizontalCenter: parent.horizontalCenter
		}
	}


	// property var myPadding: 16
	// function viewPopupToggle() {
	// 	if (viewmodepopup.visible == false) { viewmodepopup.visible = true; }
	// 	else { viewmodepopup.visible = false; }
	// }
	// Rectangle {
	// 	id: shell
	// 	width: parent.width
	// 	height: parent.height
	// 	radius: 8
	// 	Text {
	// 		text: qsTr("View Mode")
	// 		font.family: openSemiBold.name
	// 		font.pointSize: 11
	// 		color: "#888888"
	// 		anchors.verticalCenter: parent.verticalCenter
	// 		anchors.left: parent.left
	// 		anchors.leftMargin: 16
	// 	}
	// 	Text {
	// 		text: "text"//parent.viewtype
	// 		font.family: openRegular.name
	// 		font.pointSize: 10
	// 		color: "#1db2c4"
	// 		anchors.verticalCenter: parent.verticalCenter
	// 		anchors.right: parent.right
	// 		anchors.rightMargin: 16 * 2 + viewtriangle.width
	// 	}
	// 	Image {
	// 		id: viewtriangle
	// 		source: "qrc:/Resource/arrow_right.png"
	// 		anchors.verticalCenter: parent.verticalCenter
	// 		anchors.right: parent.right
	// 		anchors.rightMargin: 16
	// 	}
	// 	MouseArea {
	// 		anchors.fill: parent
	// 		hoverEnabled: true
	// 		onClicked: { viewPopupToggle(); }
	// 	}
	// }
	// SidePopup {
	// 	id: viewmodepopup
	// 	width: 184
	// 	height: 88
	// 	popupColor: "#FFFFFF"

	// 	Hix.Button {
	// 		id: objectviewbutton
	// 		width: 160
	// 		height: 36
	// 		anchors.top: parent.top
	// 		anchors.topMargin: (parent.height - height * 2) / 2
	// 		anchors.right: parent.right
	// 		anchors.rightMargin: (parent.width - width - 16) / 2
	// 		Text {
	// 			id: objecttext
	// 			anchors.horizontalCenter: parent.horizontalCenter
	// 			anchors.verticalCenter: parent.verticalCenter
	// 			font.pointSize: 10
	// 			text: qsTr("Object View")
	// 			font.family: openRegular.name
	// 		}
	// 		onClicked: {
	// 			root.setViewType(Hix.ViewModeShell.ObjectView)
	// 			objecttext.color = "#1db2c4"
	// 			objecttext.font.family = openSemiBold.name
	// 			layertext.color = "#000000"
	// 			layertext.font.family = openRegular.name
	// 		}
	// 	}

	// 	Hix.Button {
	// 		id: layerviewbutton
	// 		width: 160
	// 		height: 36
	// 		anchors.top: objectviewbutton.bottom
	// 		anchors.right: parent.right
	// 		anchors.rightMargin: (parent.width - width - 16) / 2
	// 		Text {
	// 			id: layertext
	// 			anchors.horizontalCenter: parent.horizontalCenter
	// 			anchors.verticalCenter: parent.verticalCenter
	// 			font.pointSize: 10
	// 			text: qsTr("Layer View")
	// 			font.family: openRegular.name
	// 		}
	// 		onClicked: {
	// 			root.setViewType(Hix.ViewModeShell.LayerView)
	// 			layertext.color = "#1db2c4"
	// 			layertext.font.family = openSemiBold.name
	// 			objecttext.color = "#000000"
	// 			objecttext.font.family = openRegular.name
	// 		}
	// 	}

	// }

	DropShadow {
		anchors.fill: shell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: shell
	}
}