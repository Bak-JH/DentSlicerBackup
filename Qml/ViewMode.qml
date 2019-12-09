import QtQuick 2.6
import hix.qml 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item {
	property var myPadding: 16
	function viewPopupToggle() {
		if (viewmodepopup.visible == false) { viewmodepopup.visible = true; }
		else { viewmodepopup.visible = false; }
	}
	ViewModeShell {
		id: viewmodeshell
		width: 256
		height: 50
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
			source: "qrc:/Resource/triangle_right.png"
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
	ViewModePopup {
		id: viewmodepopup
		width: 184
		height: 88
		color: "transparent"
		anchors.left: viewmodeshell.right
		anchors.leftMargin: 20
		anchors.top: parent.top

		Canvas {
			id: canvas
			anchors.fill: parent
			onPaint: {
				var ctx = getContext("2d");
				ctx.reset();

				var centreX = width / 2;
				var centreY = height / 2;

				var windowRadius = 8;
				var startX = width - windowRadius;
				var startY = windowRadius;

				//168*88
				var w0 = 168
				var h0 = 88
				var w = w0 - windowRadius * 2;	//straight part
				var h = h0 - windowRadius * 2;	//straight part
				var s = 20;		//triangle part height
				var h1 = h - s - 8;
			

				//ctx.shadowBlur = 9;
				//ctx.shadowColor = "#30000000";

				ctx.beginPath();
				ctx.fillStyle = "#ffffff";
				ctx.moveTo(startX, startY-windowRadius);
				ctx.arc(startX, startY, windowRadius, Math.PI * 1.5, Math.PI * 2, false);
				//ctx.moveTo(startX+windowRadius, startY);
				ctx.lineTo(startX+windowRadius, startY+h);
				ctx.arc(startX, startY+h, windowRadius, 0, Math.PI * 0.5, false);
				ctx.lineTo(startX-w, startY+h+windowRadius);
				ctx.arc(startX-w, startY+h, windowRadius, Math.PI * 0.5, Math.PI, false);
				ctx.lineTo(startX-w-windowRadius, startY+h-h1);
				ctx.lineTo(startX-w-windowRadius-16, startY+h-h1-s/2);
				ctx.lineTo(startX-w-windowRadius, startY+h-h1-s);
				ctx.lineTo(startX-w-windowRadius, startY);
				ctx.arc(startX-w, startY, windowRadius, Math.PI, Math.PI * 1.5, false);
				ctx.lineTo(startX, startY-windowRadius);
				ctx.fill();
			}

			ObjectViewButton {
				id: objectviewbutton
				width: 160
				height: 36
				anchors.top: parent.top
				anchors.topMargin: (parent.height - height * 2) / 2
				anchors.right: parent.right
				anchors.rightMargin: (parent.width - width - 16) / 2
				Text {
					id: objecttext
					text: qsTr("Object View")
					font.family: openRegular.name
					font.pointSize: 10
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.verticalCenter: parent.verticalCenter
				}
				MouseArea {
					anchors.fill: parent
					onClicked: {
						objecttext.color = "#1db2c4"
						objecttext.font.family = openSemiBold.name
						layertext.color = "#000000"
						layertext.font.family = openRegular.name
					}
				}
			}
			LayerViewButton {
				id: layerviewbutton
				width: 160
				height: 36
				anchors.top: objectviewbutton.bottom
				anchors.right: parent.right
				anchors.rightMargin: (parent.width - width - 16) / 2
				Text {
					id: layertext
					text: qsTr("Layer View")
					font.family: openRegular.name
					font.pointSize: 10
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.verticalCenter: parent.verticalCenter
				}
				MouseArea {
					anchors.fill: parent
					onClicked: {
						layertext.color = "#1db2c4"
						layertext.font.family = openSemiBold.name
						objecttext.color = "#000000"
						objecttext.font.family = openRegular.name
					}
				}
			}
		}
		DropShadow {
			anchors.fill: canvas
			radius: 10.0
			samples:21
			color: "#55000000"
			source: canvas
		}
	
	}

	DropShadow {
		anchors.fill: viewmodeshell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: viewmodeshell
	}
}