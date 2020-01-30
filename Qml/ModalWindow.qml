import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12
import QtQuick.Layouts 1.12

Hix.ModalShell {
	id: root
	width: 420
	height: 240
	property var buttonsWidth: 320
	property var buttonSpace: 32
	property alias contentArea: contentArea

	function setButtons(buttonSet)
	{
		var prevBtnOption;
		for(var button in buttonSet)
		{
			console.log(buttonSet[button])
			var newBtn = Qt.createComponent("ModalWindowButton.qml");
			var newBtnOption = 
							newBtn.createObject(buttonarea,
							{
								"Layout.fillWidth" : true,
								"id" : "button" + (button+1),
								"btnText" : buttonSet[button][0],
								"width" : (320 - (32 * (buttonSet.length - 1))) / buttonSet.length,
								"defaultColor" : buttonSet[button][1],
								"hoverColor" : buttonSet[button][2]
								//"anchors.left" : button == 0 ? buttonarea.left : prevBtnOption.right,
								//"anchors.leftMargin" : button == 0 ? 0 : 32
							});
			prevBtnOption = newBtnOption;
		}
	}

	MouseArea {
		id: blockingArea
		width: window.width
		height: window.height
		anchors.fill: window
		propagateComposedEvents: false
        hoverEnabled: true
        preventStealing: true
	}

	Rectangle {
		id: shell
		width: parent.width
		height: parent.height
		radius: 8
		color: "#ffffff"

		Hix.CloseButton {
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
		}
		
		RowLayout {
			id: buttonarea
			width: 320
			height: 32
			spacing: 32
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.bottom: parent.bottom
			anchors.bottomMargin: parent.height * 0.2
			/*
			Hix.Button {
				id: leftbtn
				width: (buttonsWidth - buttonSpace) / 2
				height: 32
				radius: height / 2
				color: "#00b9c8"	//themecolor
				anchors.left: parent.left

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.verticalCenter: parent.verticalCenter
					color: "#FFFFFF"
					font.pointSize: 10
					text: "Apply"
					font.family: openSemiBold.name
				}
				onClicked: { console.log("clicked"); }
				onEntered: { color = "#21959e" }
				onExited: { color = "#00b9c8" }
			}
			
			Hix.Button {
				id: rightbtn
				width: buttonsWidth - leftbtn.width - buttonSpace
				height: 32
				radius: height / 2
				color: "#abb3b3"	//themecolor
				anchors.left: leftbtn.right
				anchors.leftMargin: buttonSpace

				Text {
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.verticalCenter: parent.verticalCenter
					color: "#FFFFFF"
					font.pointSize: 10
					text: "Apply"
					font.family: openSemiBold.name
				}
				onClicked: { console.log(contentArea.height); root.setButtons([["Apply", "#00b9c8", "#21959e"], ["Cancel", "#abb3b3", "#8b9393"]]); }
				onEntered: { color = "#8b9393" }
				onExited: { color = "#abb3b3" }
			}
			*/
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