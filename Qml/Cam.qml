import QtQuick 2.6
import hix.qml 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item{
	property var camHoverColor: "#eeeeee"
	function camToggle() {
		if (campopupshell.visible == false) { campopupshell.visible = true; }
		else { campopupshell.visible = false; }
	}

	CamButtonShell {
		id: cambuttonshell
		width: 90
		height: width
		radius: width / 2
		color: "#ffffff"
		//color: hovered ? "#eeeeee" : "#ffffff"

		Text {
			text: qsTr("CAM")
			font.family: openSemiBold.name
			font.pointSize: 10
			color: "#aaaaaa"
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top
			anchors.topMargin: parent.height * 0.1
		}
		Image {
			id: camicon
			source: "qrc:/Resource/cam.png"
			anchors.left: parent.left
			anchors.leftMargin: parent.width * 0.2
			anchors.bottom: parent.bottom
			anchors.bottomMargin: parent.height * 0.3
		}
		Image {
			source: "qrc:/Resource/triangle_down.png"
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.bottom: parent.bottom
			anchors.bottomMargin: parent.height * 0.1
			height: width * 0.6
		}

		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			onEntered: { cambuttonshell.color = camHoverColor; camicon.source = "qrc:/Resource/cam_hover.png" }
			onExited: { cambuttonshell.color = "#ffffff"; camicon.source = "qrc:/Resource/cam.png" }
			onClicked: { camToggle(); }
		}
	}
	
	DropShadow {
		anchors.fill: cambuttonshell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: cambuttonshell
	}

	CamPopupShell {
		id:campopupshell
		visible: false
		anchors.top: cambuttonshell.bottom
		anchors.topMargin: cambuttonshell.width * 0.1
		anchors.right: cambuttonshell.right
		anchors.rightMargin: 6

		width: 218
		height: 234
		color: "transparent"

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
				var startY = windowRadius + 16;

				//282*224
				var w0 = 218
				var h0 = 218
				var w = w0 - windowRadius * 2;	//straight part
				var h = h0 - windowRadius * 2;	//straight part
				var s = 20;		//triangle part height
				//var h1 = h - s - 24;
				var w1 = 22;
			

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
				//ctx.lineTo(startX-w-windowRadius, startY+h-h1);
				//ctx.lineTo(startX-w-windowRadius-16, startY+h-h1-s/2);
				//ctx.lineTo(startX-w-windowRadius, startY+h-h1-s);
				ctx.lineTo(startX-w-windowRadius, startY);
				ctx.arc(startX-w, startY, windowRadius, Math.PI, Math.PI * 1.5, false);
				ctx.lineTo(startX-w1-s, startY-windowRadius);
				ctx.lineTo(startX-w1-s/2, 0);
				ctx.lineTo(startX-w1, startY-windowRadius);
				ctx.lineTo(startX, startY-windowRadius);
				ctx.fill();

			}

			Rectangle {
				width: 194
				height: 194
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.bottom: parent.bottom
				anchors.bottomMargin: (parent.height - height - 16) / 2
				CamBox {
					id: up
					anchors.top: parent.top
					anchors.horizontalCenter: parent.horizontalCenter
					Image {
						source: "qrc:/Resource/cam_top_1.5.png"
						width: 36
						height: width
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
					}
				}
				CamBox {
					id: back
					anchors.top: parent.top
					anchors.right: parent.right
					Image {
						source: "qrc:/Resource/cam_back_1.5.png"
						width: 36
						height: width
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
					}
				}
				CamBox {
					id: left
					anchors.verticalCenter: parent.verticalCenter
					anchors.left: parent.left
					Image {
						source: "qrc:/Resource/cam_left_1.5.png"
						width: 36
						height: width
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
					}
				}
				CamBox {
					id: front
					anchors.verticalCenter: parent.verticalCenter
					anchors.horizontalCenter: parent.horizontalCenter
					Image {
						source: "qrc:/Resource/cam_front_1.5.png"
						width: 36
						height: width
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
					}
				}
				CamBox {
					id: right
					anchors.verticalCenter: parent.verticalCenter
					anchors.right: parent.right
					Image {
						source: "qrc:/Resource/cam_right_1.5.png"
						width: 36
						height: width
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
					}
				}
				CamBox {
					id: bottom
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.bottom: parent.bottom
					Image {
						source: "qrc:/Resource/cam_bottom_1.5.png"
						width: 36
						height: width
						anchors.verticalCenter: parent.verticalCenter
						anchors.horizontalCenter: parent.horizontalCenter
					}
				}
			}
			/*
			CamBoxIcon {
				width: 58
				height: width
				radius: 6
				color: "#eeeeee"
				Image {
					source: "qrc:/Resource/triangle_down.png"
				}
			}
			*/
		}
		
	
		DropShadow {
			anchors.fill: canvas
			radius: 10.0
			samples:21
			color: "#55000000"
			source: canvas
		}
	}
}