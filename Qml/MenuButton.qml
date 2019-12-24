import QtQuick 2.6
import hix.qml 1.0 as Hix

//MenuBtnShell {
Hix.RoundButton {
	property string iconBasic
	property string nameColor: "#9d9d9d"
	property string featureName

	width: 85
	height: 90
	//color: hovered ? "#eeeeee" : "#ffffff"
	
	Image {
		sourceSize.width: width
		anchors.top: parent.top;
		anchors.topMargin: (parent.height - height) * 0.5 - parent.height * 0.08
		anchors.horizontalCenter: parent.horizontalCenter;
		//source: fimg
		source: iconBasic
	}
	Text {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 10
		text: featureName
		//text: parent.fName
		color: nameColor
		font.pointSize: 10
		font.family: openRegular.name
	}
	
}


	/*
	states: [
        State {
            name: "inactive"
            PropertyChanges {target: menubuttonshell; color: "#fefefe"}
        },
        State {
            name: "active";
            PropertyChanges {target: menubuttonshell; color: "#00ff00"}
        }
    ]
	
	MouseArea {
		anchors.fill: parent
		hoverEnabled: true
		
		onClicked: { 
			console.log("clicked");
			parent.color = "#ff0000";
		}
		onEntered: {
			if (parent.color == "#ffffff") {
				parent.color = "#eeeeee";
			}
			else if (parent.color == "#ff0000")
				parent.color = "#ff0000";
			else
				parent.color = "#ffffff";
		}
		onExited: {
			console.log("exit");
			console.log(parent.color);
			if (parent.color == "#ff0000") {
				parent.color = "#ff0000";
			}
			else {
				parent.color = "#ffffff";
			}
		}
	}
	*/