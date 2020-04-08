import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.RangeSlideBarShell {
		id: root
		width: 64
		height: 600
		onLowerValueChanged: {
			control.first.value = lowerValue;
		}
		onUpperValueChanged: {
			control.second.value = upperValue;

		}
	Rectangle {
		id: shell
		width: parent.width
		height: parent.height
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
			text: control.from
			font.family: openRegular.name
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: control.bottom
			anchors.topMargin: 8
		}
		
		RangeSlider {
			id: control
			from: root.min.toFixed(2)
			to: root.max.toFixed(2)
			first.value:(from + to) / 2
			second.value: (from + to) / 2
			orientation: Qt.Vertical
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter

			first.onMoved:
			{
				root.lowerValue = first.value
			}
			second.onMoved:
			{
				//top
				root.upperValue = second.value
			}

			background: Rectangle {
				anchors.horizontalCenter: parent.horizontalCenter
				x: control.leftPadding// + control.availableWidth / 2  - width / 2
				y: control.topPadding
				implicitWidth: 12
				implicitHeight: 500
				width: implicitWidth
				height: control.availableHeight
				radius: width / 2
				color: "#e4e5e5"

				Rectangle {
					//x: control.first.visualPosition * parent.width
					y: control.second.visualPosition * parent.height
					width: parent.width//control.second.visualPosition * parent.width - x
					height: control.first.visualPosition * parent.height - y //(control.first.visualPosition - control.second.visualPosition) * parent.height
					color: "#19d6e0"
					radius: width / 2
				}
			}
			// control.leftPadding = 6
			// control.availableWidth = 188

			first.handle: Rectangle {
				x: control.leftPadding
				y: control.bottomPadding + control.first.visualPosition * (control.availableHeight - height) //control.first.visualPosition * parent.height - implicitHeight / 2
				implicitWidth: 36
				implicitHeight: 22
				radius: 2
				color: control.first.pressed ? "#727d7d" : "#929d9d"

				Rectangle {	//current position
					width: 106
					height: 34
					anchors.right: control.first.handle.left
					anchors.verticalCenter: parent.verticalCenter
					//y: control.topPadding + control.first.visualPosition * (control.availableHeight - height)
					visible: control.first.pressed ? true : false
				
					Canvas {
						id: canvas
						anchors.fill: parent
						Text {
							anchors.verticalCenter: parent.verticalCenter
							anchors.left: parent.left
							anchors.leftMargin: (parent.width - 10 - width) / 2
							//text: Math.floor(control.value)
							text: "Bottom plane"
							font.family: openSemiBold.name
							font.pointSize: 10
							color: "#ffffff"
						}
						onPaint: {
							var ctx = getContext("2d");
							ctx.reset();

							var windowRadius = 2;
							var startX = width - windowRadius - 10;	//triangle height = 10
							var startY = windowRadius;

							//96*34
							var w0 = 96
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
				
			}

			second.handle: Rectangle {
				x: control.leftPadding
				y: control.topPadding + control.second.visualPosition * (control.availableHeight - height)
				implicitWidth: 36
				implicitHeight: 22
				radius: 2
				//border.color: "red"
				//color: "transparent"
				color: control.second.pressed ? "#727d7d" : "#929d9d"

				Rectangle {	//current position
					width: 106 //58
					height: 34
					anchors.right: control.second.handle.left
					anchors.verticalCenter: parent.verticalCenter
					//y: control.topPadding + control.first.visualPosition * (control.availableHeight - height)
					visible: control.second.pressed ? true : false
				
					Canvas {
						anchors.fill: parent
						Text {
							anchors.verticalCenter: parent.verticalCenter
							anchors.left: parent.left
							anchors.leftMargin: (parent.width - 10 - width) / 2
							//text: Math.floor(control.value)
							text: "Cutting plane"
							font.family: openSemiBold.name
							font.pointSize: 10
							color: "#ffffff"
						}
						onPaint: {
							var ctx = getContext("2d");
							ctx.reset();

							var windowRadius = 2;
							var startX = width - windowRadius - 10;	//triangle height = 10
							var startY = windowRadius;

							//96*34	//48*34
							var w0 = 96 //48
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