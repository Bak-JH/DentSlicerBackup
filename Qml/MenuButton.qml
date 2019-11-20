import QtQuick 2.4
import hix.qml 1.0

MenuBtnShell {
	property string iconBasic
	width: 85
	height: 90
	color: "#FFFFFF"
	
	Image {
		//width: 50
		//height: 50
		anchors.top: parent.top;
		
		anchors.horizontalCenter: parent.horizontalCenter;
		//source: fimg
		source: iconBasic
	}
	Text {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		anchors.bottomMargin: 10
		text: parent.fname
	}

	states: [
        State {
            name: "inactive"
            PropertyChanges {target: menubuttonshell; color: "#fefefe"}
        },
        State {
            name: "active";
            PropertyChanges {target: menubuttonshell; color: "red"}
        }
    ]

    //state: "inactive"

	MouseArea {
		anchors.fill: parent
		hoverEnabled: true
		onClicked: { 
			console.log("clicked");
			parent.color = "#ff0000";
			//if (parent.state == "inactive") {
			//	parent.state == "active";
			//	console.log("activated");
			//}
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
	
}