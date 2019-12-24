import QtQuick 2.6
import hix.qml 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item {
	SlideBarShell {
		id: slidebarshell
		width: 64
		height: 600
		radius: width / 2
		color: "#ffffff"

		Text {
			id: modelheight
			text: control.to
			font.family: openRegular.name
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.bottom: control.top
			anchors.bottomMargin: 8
		}
		Text {
			id: zero
			text: qsTr("0")
			font.family: openRegular.name
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: control.bottom
			anchors.topMargin: 8
		}

		Slider {
			id: control
			from: parent.slideRect.from
			to: 154 //parent.slideRect.to
			value: (from + to) / 2
			orientation: Qt.Vertical
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter

			background: Rectangle {
				anchors.horizontalCenter: parent.horizontalCenter
				x: control.leftPadding
				y: control.topPadding + control.availableHeight / 2 - height / 2
				implicitWidth: 12
				implicitHeight: 500
				width: implicitWidth
				height: control.availableHeight
				//width: control.availableWidth
				//height: implicitHeight
				radius: width / 2
				color: "#e4e5e5"

				Rectangle {
					anchors.bottom: parent.bottom
					width: parent.width
					height: parent.height - control.visualPosition * parent.height
					color: "#19d6e0"
					radius: width / 2
				}
			
			}

			handle: Rectangle {
				id: handle
				//x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
				//y: control.topPadding + control.availableHeight / 2 - height / 2
				x: control.leftPadding + control.availableWidth / 2 - width / 2
				y: control.bottomPadding + control.visualPosition * (control.availableHeight - height)
				implicitWidth: 36
				implicitHeight: 22
				radius: 2
				color: control.pressed ? "#727d7d" : "#929d9d"
				//border.color: "#bdbebf"
			}
			Rectangle {	//current position
				width: 58
				height: 34
				//color: "transparent"
				anchors.right: handle.left
				//color: "transparent"
				//x: 0 - control.availableWidth * 1.2
				y: control.bottomPadding + control.visualPosition * (control.availableHeight - height)
				visible: control.pressed ? true : false
				
				Canvas {
					id: canvas
					anchors.fill: parent
					Text {
						//text: control.parent.slideRect.visualPosition
						anchors.verticalCenter: parent.verticalCenter
						anchors.left: parent.left
						anchors.leftMargin: (parent.width - 10 - width) / 2
						text: Math.floor(control.value)
						font.family: openSemiBold.name
						font.pointSize: 10.5
						color: "#ffffff"
					}
					onPaint: {
						var ctx = getContext("2d");
						ctx.reset();

						var windowRadius = 2;
						var startX = width - windowRadius - 10;	//triangle height = 10
						var startY = windowRadius;

						//48*34
						var w0 = 48
						var h0 = 34
						var w = w0 - windowRadius * 2;	//straight part
						var h = h0 - windowRadius * 2;	//straight part
						var s = 12;		//triangle part height
						var h1 = (h - s) / 2;

						ctx.beginPath();
						ctx.fillStyle = "#00b9c8";
						ctx.moveTo(startX, startY-windowRadius);
						ctx.arc(startX, startY, windowRadius, Math.PI * 1.5, Math.PI * 2, false);
						ctx.lineTo(startX+windowRadius, startY+h1);
						ctx.lineTo(startX+windowRadius+10, startY+h1+s/2);
						ctx.lineTo(startX+windowRadius, startY+h1+s);
						ctx.lineTo(startX+windowRadius, startY+h);
						ctx.arc(startX, startY+h, windowRadius, 0, Math.PI * 0.5, false);
						ctx.lineTo(startX-w, startY+h+windowRadius);
						ctx.arc(startX-w, startY+h, windowRadius, Math.PI * 0.5, Math.PI, false);
						ctx.lineTo(startX-w-windowRadius, startY);
						ctx.arc(startX-w, startY, windowRadius, Math.PI, Math.PI * 1.5, false);
						ctx.lineTo(startX, startY-windowRadius);
						ctx.fill();

					}
				}
	
			}
			/*
			Rectangle {
				id: currentPos
				width: 48
				height: 34
				color: "#00b9c8"
				x: 0 - control.availableWidth * 1.2
				y: control.bottomPadding + control.visualPosition * (control.availableHeight - height)
				visible: control.pressed ? true : false
				Text {
					//text: control.parent.slideRect.visualPosition
					anchors.verticalCenter: parent.verticalCenter
					anchors.horizontalCenter: parent.horizontalCenter
					text: Math.floor(control.value)
					font.family: openRegular.name
					color: "#ffffff"
				}
			}
			*/
		}
	}
	DropShadow {
		anchors.fill: slidebarshell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: slidebarshell
	}
}