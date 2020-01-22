import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Item{
	property var camHoverColor: "#eeeeee"
	function camToggle() {
		if (campopupshell.visible == false) { campopupshell.visible = true; }
		else { campopupshell.visible = false; }
	}

	Hix.Button {
		id: settingbutton
		width: 90
		height: width
		radius: width / 2
		color: "#ffffff"
		
		Image {
			id: settingimg
			source: "qrc:/Resource/btn_setting.png"
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top
			anchors.topMargin: parent.height * 0.16
		}
		Text {
			text: qsTr("Setting")
			font.family: openSemiBold.name
			font.pointSize: 10
			color: "#aaaaaa"
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.bottom: parent.bottom
			anchors.bottomMargin: parent.height * 0.12
		}
		onClicked: { camToggle(); }
		onEntered: { settingbutton.color = camHoverColor; }
		onExited: { settingbutton.color = "#ffffff"; }
		
	}
	
	DropShadow {
		anchors.fill: settingbutton
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: settingbutton
	}
}