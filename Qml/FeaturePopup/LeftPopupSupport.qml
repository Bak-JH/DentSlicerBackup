import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls" 

Hix.LeftPopupShell {
	id: leftpopupshell
	height: 196

	function settingToggle() {
		if (supportsettingpopup.visible == false) { supportsettingpopup.visible = true; }
		else { supportsettingpopup.visible = false; }
	}
	Rectangle
	 {
		id: shell
		width: 256
		height: 196
		color: "#F6FEFF"
		radius: 8

		Text {
			id: popuptitle
			text: "Support"
			font.pointSize: 11
			font.family: openRegular.name
			font.weight: Font.Bold
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.topMargin: 14
			anchors.leftMargin: 16
		}

		Hix.CloseButton {
			width: 16
			height: 16
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
		Hix.RoundButton {
			id: generatesupports
			width: 186
			height: 32
			radius: 16
			//color: enabled ? "#00b9c8" : "#cccccc"
			color: "#00b9c8"
			anchors.bottom: editsupports.top
			anchors.bottomMargin: 16
			anchors.left: parent.left
			anchors.leftMargin: 16
			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
				color: "#FFFFFF"
				font.pointSize: 10
				text: "Generate Supports"
				font.family: openSemiBold.name
			}
			onClicked: { console.log("clicked"); supportbutton1.enabled = false; console.log(supportbutton1.enabled); }
			onEntered: { color = "#21959e" }
			onExited: { color = "#00b9c8" }
		}

		Hix.RoundButton {
			id: supportsettingbutton
			width: 32
			height: width
			radius: width / 2
			anchors.bottom: generatesupports.bottom
			anchors.right: parent.right
			anchors.rightMargin: 16
			Image {
				source: "qrc:/Resource/support_setting.png"
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
			}
			onClicked: { console.log("clicked"); settingToggle();}
			onEntered: { color = "#d2e0e1" }
			onExited: { color = "transparent" }
		}

		Hix.RoundButton {
			id: editsupports
			width: 224
			height: 32
			radius: height / 2
			color: "#00b9c8"
			anchors.bottom: clearsupports.top
			anchors.bottomMargin: 16
			anchors.horizontalCenter: parent.horizontalCenter
			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
				color: "#FFFFFF"
				font.pointSize: 10
				text: "Edit Supports"
				font.family: openSemiBold.name
			}
			onClicked: { console.log("clicked"); editsupports.visible=false; editbuttons.visible=true; }
			onEntered: { color = "#21959e" }
			onExited: { color = "#00b9c8" }
		}

		Rectangle {
			id: editbuttons
			width: 224
			height: 32
			anchors.bottom: clearsupports.top
			anchors.bottomMargin: sidePadding
			anchors.horizontalCenter: parent.horizontalCenter
			visible: false
			Rectangle {
				id: leftround
				width: 32
				height: width
				color: "#00b9c8"
				radius: height / 2
				anchors.left: parent.left
			}
			Hix.RoundButton {
				width: ((parent.width - 2) / 2) - 16
				height: parent.height
				color: "#00b9c8"
				anchors.left: parent.left
				anchors.leftMargin: 16
				onEntered: { color = "#21959e"; leftround.color = "#21959e" }
				onExited: { color = "#00b9c8"; leftround.color = "#00b9c8" }
				Text {
					anchors.left: parent.left
					anchors.leftMargin: 8
					anchors.verticalCenter: parent.verticalCenter
					color: "#FFFFFF"
					font.pointSize: 10
					text: "Apply edits"
					font.family: openSemiBold.name
				}
			}
			Rectangle {
				id: rightround
				width: 32
				height: width
				color: "#00b9c8"
				radius: height / 2
				anchors.right: parent.right
			}
			Hix.RoundButton {
				width: ((parent.width - 2) / 2) - 16
				height: parent.height
				color: "#00b9c8"
				anchors.right: parent.right
				anchors.rightMargin: 16
				onEntered: { color = "#21959e"; rightround.color = "#21959e" }
				onExited: { color = "#00b9c8"; rightround.color = "#00b9c8" }
				Text {
					anchors.right: parent.right
					anchors.rightMargin: 6
					anchors.verticalCenter: parent.verticalCenter
					color: "#FFFFFF"
					font.pointSize: 10
					text: "Cancel edits"
					font.family: openSemiBold.name
				}
			}
		}
		
		Hix.RoundButton {
			id: clearsupports
			width: 224
			height: 32
			radius: height / 2
			color: "#00b9c8"
			anchors.bottom: parent.bottom
			anchors.bottomMargin: 16
			anchors.horizontalCenter: parent.horizontalCenter
			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
				color: "#FFFFFF"
				font.pointSize: 10
				text: "Clear All Supports"
				font.family: openSemiBold.name
			}
			onClicked: { console.log("clicked"); }
			onEntered: { color = "#21959e" }
			onExited: { color = "#00b9c8" }
		}
		
	}

	Rectangle {
		id: supportsettingpopup
		width: 296
		height: 224
		color: "transparent"
		anchors.left: shell.right
		anchors.leftMargin: 12
		anchors.top: parent.top
		anchors.topMargin: 42
		visible: false

		Canvas {
			id: canvas
			anchors.fill: parent
			width: 296
			height: 224
			onPaint: {
				var ctx = getContext("2d");
				ctx.reset();

				var centreX = width / 2;
				var centreY = height / 2;

				var windowRadius = 8;
				var startX = width - windowRadius;
				var startY = windowRadius;

				//280*224 168*88
				var w0 = 280
				var h0 = 224
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

			CtrDropdownBox {
				id: supporttype
				dropdownText: "Support Type"
				width: parent.width - 16 * 2 - 16	// 16 = triangle height
				height: 26
				anchors.left: parent.left
				anchors.leftMargin: 32	// 16 = triangle height
				anchors.top: parent.top
				anchors.topMargin: 16
			}
			CtrDropdownBox {
				id: supportdensity
				dropdownText: "Support Density"
				width: parent.width - 16 * 2 - 16	// 16 = triangle height
				height: 26
				anchors.left: parent.left
				anchors.leftMargin: 32	// 16 = triangle height
				anchors.top: supporttype.bottom
				anchors.topMargin: 16
			}
			CtrDropdownBox {
				id: maxradius
				dropdownText: "Max.radius"
				width: parent.width - 16 * 2 - 16	// 16 = triangle height
				height: 26
				anchors.left: parent.left
				anchors.leftMargin: 32	// 16 = triangle height
				anchors.top: supportdensity.bottom
				anchors.topMargin: 16
			}
			CtrDropdownBox {
				id: minradius
				dropdownText: "Min.radius"
				width: parent.width - 16 * 2 - 16	// 16 = triangle height
				height: 26
				anchors.left: parent.left
				anchors.leftMargin: 32	// 16 = triangle height
				anchors.top: maxradius.bottom
				anchors.topMargin: 16
			}
			CtrDropdownBox {
				id: rafttype
				dropdownText: "Raft Type"
				width: parent.width - 16 * 2 - 16	// 16 = triangle height
				height: 26
				anchors.left: parent.left
				anchors.leftMargin: 32	// 16 = triangle height
				anchors.top: minradius.bottom
				anchors.topMargin: 16
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
		anchors.fill: shell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: shell
	}
}
