import QtQuick 2.0


Rectangle{
    property string iconSource1
    property string iconSource2
    property string iconText

    id: temp
    width: buttonWidth
    height: buttonHeight

    anchors.top : parent.top
    anchors.left: parent.left

    color : "transparent"

    signal buttonClicked()
    function do_buttonclick(iconText){
        switch(iconText){
        case "Move":
            buttonClicked();
            break;
        case "Rotate":
            buttonClicked();
            break;
        case "Orient":
            buttonClicked();
            break;
        case "Lay Flat":
            buttonClicked();
            break;
        case "Support":
            buttonClicked();
            break;
        case "Scale":
            buttonClicked();
            break;
        case "Auto Repair":
            buttonClicked();
            break;
        case "Cut":
            buttonClicked();
            break;
        case "Shell Offset":
            buttonClicked();
            break;
        case "Extend":
            buttonClicked();
            break;
        case "Label":
            buttonClicked();
            break;
        case "Shell Offset":
            buttonClicked();
            break;
        case "Extend":
            buttonClicked();
            break;
        case "Label":
            buttonClicked();
            break;
        case "Extend":
            buttonClicked();
            break;
        case "Arrange":
            buttonClicked();
            break;
        case "Setting":
            buttonClicked();
            break;
        case "Feedback":
            buttonClicked();
            break;



        default:
            //console.log(functionname);
            break;
        }
    }


    //Default
    Rectangle {
        id: basicbutton
        visible: true
        anchors.fill: parent
        color: parent.color
        Image {
            //anchors.margins: 2
            //anchors.fill: parent
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter

            source: iconSource1
        }
        Text{
            anchors.top:parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 14
            text: iconText
            font.pixelSize: 16
            //font.bold: true
            font.family: mainFont.name
            color: "#ffffff"
        }

    }

    //Active
    Rectangle {
        id: activebutton
        visible: false
        anchors.fill: parent
        color: "#ffffff"
        /*
        Rectangle {
            width: parent.width - 6
            height: parent.height - 6
            color: "#3EA6B7"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }
        */
        Image {
            //anchors.margins: 2
            //anchors.fill: parent
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            source: iconSource2
        }
        Text {
            anchors.top:parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 14
            text: iconText
            font.pixelSize: 16
            font.family: mainFont.name
            color: "#3BA1B2"
        }
    }

    states: [
        State {
            name: "inactive"
            PropertyChanges {target: activebutton; visible: false}
        },
        State {
            name: "active"; //when: box_uppertab.state == iconText
            PropertyChanges {target: activebutton; visible: true}
        }
    ]

    state: "inactive"

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            basicbutton.color = "#3EABBA"
            qm.setHandCursor();
        }
        onExited: {
            basicbutton.color = parent.color
            qm.resetCursor();
        }

        onClicked:{
            if( qm.getViewMode() !== 0 ) {
                return;
            }

            if (parent.state == "inactive"){
                all_off();
                parent.state = "active";
                console.log("active" + iconText);
            }
            else {
                parent.state = "inactive";
                console.log("inactive" + iconText);
            }
            do_buttonclick(iconText)
        }

    }

}



