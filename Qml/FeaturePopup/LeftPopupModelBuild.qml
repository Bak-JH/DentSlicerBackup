import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1

LeftPopup {
	id: leftpopupshell
	title: qsTr("Model Build")
	height: 168

	function helpToggle() {
		if (modelbuildhelp.visible == false) { modelbuildhelp.visible = true; }
		else { modelbuildhelp.visible = false; }
	}

	Text {
		id: popupbody
		width: 200 //182 //parent.width - sidePadding * 2
		//height: 26
		text: qsTr("Adjust the orientation of a model and positions of two planes if necessary.")
		wrapMode: Text.WordWrap //Text.WrapAnywhere
		font.pointSize: 10
		font.family: openRegular.name
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 44
		anchors.leftMargin: 16
	}

	Hix.Button {
		width: 30
		height: width
		radius: width / 2
		color: "#f6feff"
		anchors.top: popupbody.top
		anchors.topMargin: -2
		anchors.right: parent.right
		anchors.rightMargin: 10
		
		Image {
			source: "qrc:/Resource/btn_modelbuild_help.png"
			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter
		}

		onEntered: { color = "#d2e0e1" }
		onExited: { color = "#f6feff" }
		onClicked: { helpToggle() }
	}

	Rectangle {
		id: modelbuildhelp
		width: 296
		height: 192 //224
		color: "transparent"
		anchors.left: leftpopupshell.right
		anchors.leftMargin: 12
		anchors.top: parent.top
		anchors.topMargin: 32
		visible: false

		Canvas {
			id: canvas
			anchors.fill: parent
			width: 296
			height: 192 //224
			onPaint: {
				var ctx = getContext("2d");
				ctx.reset();

				var centreX = width / 2;
				var centreY = height / 2;

				var windowRadius = 8;
				var startX = width - windowRadius;
				var startY = windowRadius;

				//280*192
				var w0 = 280
				var h0 = 192 //224
				var w = w0 - windowRadius * 2;	//straight part
				var h = h0 - windowRadius * 2;	//straight part
				var s = 20;		//triangle part height
				var h1 = h - s - 8;
		

				ctx.beginPath();
				ctx.fillStyle = "#f6feff";
				ctx.moveTo(startX, startY-windowRadius);
				ctx.arc(startX, startY, windowRadius, Math.PI * 1.5, Math.PI * 2, false);
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

			Image {
				id: modelbuildimg
				source: "qrc:/Resource/img_modelbuild_popup.png"
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.top: parent.top
				anchors.topMargin: 22
			}
			Rectangle {
				width: 280 - 16 * 2
				anchors.top: modelbuildimg.bottom
				anchors.topMargin: 16
				anchors.right: parent.right
				anchors.rightMargin: 16
				Text {
					width: parent.width
					text: '<b><font color=purple>'+"Cutting plane "+'</font></b>'+'determines which part of the scan file to use without modification.<br>'+'<b><font color=purple>'+"Bottom plane "+'</font></b>'+'determines the position of the floor face of the model to be created.'
					font.pointSize: 10
					wrapMode: Text.WordWrap //Text.WrapAnywhere
					font.family: openRegular.name
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

}