import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12
import QtQuick.Layouts 1.12
import "Controls"
Hix.ModalShell {
	id: root
	width: window.width
	height: window.height

	MouseArea {
		id: blockingArea
		width: parent.width
		height: parent.height
		//anchors.fill: window
		propagateComposedEvents: false
        hoverEnabled: true
        preventStealing: true
	}

	Rectangle {
		id: shell
		width: 420
		height: 240
		radius: 8
		color: "#ffffff"

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter

		CtrRectButton {
			id: closeButton
			width: 16
			height: 16
			color: "transparent"
			anchors.top: parent.top
			anchors.right: parent.right
			anchors.topMargin: 16
			anchors.rightMargin: 16
			Image {
				id: closeimg
				source: "qrc:/Resource/closebutton.png"
				anchors.top: parent.top
				anchors.right: parent.right
			}
			onEntered: { closeimg.source = "qrc:/Resource/closebutton_hover.png" }
			onExited: { closeimg.source = "qrc:/Resource/closebutton.png" }
		}
		
		Item {
			id: contentArea
			anchors.top: closeButton.bottom
			anchors.topMargin: closeButton.height * 0.4
			anchors.bottom: buttonarea.top
			anchors.bottomMargin: closeButton.height * 0.4
			anchors.left: buttonarea.left
			anchors.right: buttonarea.right

			Text {
				id:contentText
				text: root.modalMessage
				font.family: openRegular.name
				font.pointSize: 10
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
			}
		}
		
		RowLayout {
			id: buttonarea
			width: 320
			height: 32
			spacing: 32
			// Layout.preferredWidth: 32
            Layout.fillWidth: true
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.bottom: parent.bottom
			anchors.bottomMargin: parent.height * 0.2
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