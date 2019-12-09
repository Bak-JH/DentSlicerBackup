import QtQuick 2.4
import hix.qml 1.0

RoundButton {
	width: 224
	//width: parent.btnwidth
	height: 32
	radius: 16
	color: btncolor
	//anchors.bottom: parent.bottom
	//anchors.bottomMargin: 16
	//anchors.horizontalCenter: parent.horizontalCenter

	Text {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		color: "#FFFFFF"
		font.pointSize: 10
		//font.weight: Font.Bold
		text: parent.btntext
		font.family: openSemiBold.name
        //text: Roundbutton.btntext
	}

	onBtnColorChanged : {
		color = changedColor
	}
	/*
	MouseArea {
		anchors.fill: parent // parent
		hoverEnabled: true
		//onEntered: { parent.color = "#21959E" }
		//onExited: { parent.color = themeColor }
	}
	*/
	
}