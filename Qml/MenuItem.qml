import QtQuick 2.6
import hix.qml 1.0 as Hix

//MenuBtnShell {
Hix.MenuButton {
	id: menuButton
	property string iconBasic
	property string nameColor: "#9d9d9d"
	property string featureName

	width: 85
	height: 90
	color: "#ffffff"
	//color: hovered ? "#eeeeee" : "#ffffff"
	/*
	featureName {
		anchors.horizontalCenter: menuButton.horizontalCenter
		anchors.bottom: menuButton.bottom
		anchors.bottomMargin: 10
		//text: parent.fName
		color: nameColor
		font.pointSize: 10
		font.family: openRegular.name
	}
	*/
	icon {
		id: icon
		sourceSize.width: width
		sourceSize.height: height
		anchors.top: menuButton.top
		anchors.topMargin: (90 - height) * 0.5 - 90 * 0.05
		anchors.horizontalCenter: menuButton.horizontalCenter;
		//anchors.verticalCenter: menuButton.verticalCenter;
		//source: fimg
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