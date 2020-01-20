import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12
import "FeaturePopup"

Hix.ViewModeShell {
	id: root
	width: 256
	height: 50
	
	property var myPadding: 16
	function viewPopupToggle() {
		if (viewmodepopup.visible == false) { viewmodepopup.visible = true; }
		else { viewmodepopup.visible = false; }
	}
	Rectangle {
		id: shell
		width: parent.width
		height: parent.height
		radius: 8
		Text {
			text: qsTr("View Mode")
			font.family: openSemiBold.name
			font.pointSize: 11
			color: "#888888"
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: parent.left
			anchors.leftMargin: myPadding
		}
		Text {
			text: parent.viewtype
			font.family: openRegular.name
			font.pointSize: 10
			color: "#1db2c4"
			anchors.verticalCenter: parent.verticalCenter
			anchors.right: parent.right
			anchors.rightMargin: myPadding * 2 + viewtriangle.width
		}
		Image {
			id: viewtriangle
			source: "qrc:/Resource/arrow_right.png"
			anchors.verticalCenter: parent.verticalCenter
			anchors.right: parent.right
			anchors.rightMargin: myPadding
		}
		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			onClicked: { viewPopupToggle(); }
		}
	}
	SidePopup {
		id: viewmodepopup
		width: 184
		height: 88

		Hix.Button {
			id: objectviewbutton
			width: 160
			height: 36
			anchors.top: parent.top
			anchors.topMargin: (parent.height - height * 2) / 2
			anchors.right: parent.right
			anchors.rightMargin: (parent.width - width - 16) / 2
			Text {
				id: objecttext
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
				font.pointSize: 10
				text: qsTr("Object View")
				font.family: openRegular.name
			}
			onClicked: {
				console.log("clicked")
				objecttext.color = "#1db2c4"
				objecttext.font.family = openSemiBold.name
				layertext.color = "#000000"
				layertext.font.family = openRegular.name
			}
		}

		Hix.Button {
			id: layerviewbutton
			width: 160
			height: 36
			anchors.top: objectviewbutton.bottom
			anchors.right: parent.right
			anchors.rightMargin: (parent.width - width - 16) / 2
			Text {
				id: layertext
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
				font.pointSize: 10
				text: qsTr("Layer View")
				font.family: openRegular.name
			}
			onClicked: {
				console.log("clicked")
				layertext.color = "#1db2c4"
				layertext.font.family = openSemiBold.name
				objecttext.color = "#000000"
				objecttext.font.family = openRegular.name
			}
		}

	}

	DropShadow {
		anchors.fill: shell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: shell
	}
}