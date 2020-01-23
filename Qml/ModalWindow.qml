import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.ModalShell {
	id: root
	width: 420
	height: 240
	property var buttonsWidth: 320
	property var buttonSpace: 32

	function setButtons(textList)
	{
		var count = 1, prevBtnOption;
		for(var text in textList)
		{
			var newBtn = Qt.createComponent("ModalWindowButton.qml");
			var newBtnOption = 
							newBtn.createObject(buttonarea,
							{
								"id" : "button" + count,
								"btnText" : textList[text],
								"width" : (320 - (32 * (textList.length - 1))) / textList.length,
								"color" : "#00b9c8",
								"anchors.left" : count == 1 ? buttonarea.left : prevBtnOption.right,
								"anchors.leftMargin" : count == 1 ? 0 : 32
							});
			prevBtnOption = newBtnOption;
			count++;
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
			width: 16
			height: 16
			color: transparent
			anchors.top: parent.top
			anchors.right: parent.right
			anchors.topMargin: 16
			anchors.rightMargin: 16
			Image {
				source: "qrc:/Resource/closebutton.png"
				anchors.top: parent.top
				anchors.right: parent.right
			}
		}
		
		Text {
			text: root.modalMessage
			font.family: openRegular.name
			font.pointSize: 10
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top
			anchors.topMargin: parent.height * 0.4
		}
		
		Rectangle {
			id: buttonarea
			width: buttonsWidth
			height: 32
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.bottom: parent.bottom
			anchors.bottomMargin: parent.height * 0.2
			
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
				color: "#00b9c8"	//themecolor
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
				onClicked: { console.log("clicked");}
				onEntered: { color = "#21959e" }
				onExited: { color = "#00b9c8" }
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