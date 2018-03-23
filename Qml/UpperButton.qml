import QtQuick 2.0


Rectangle{
    property string iconSource1
    property string iconSource2
    property string iconText


    //property string activefunction
    id: temp
    width: buttonWidth
    height: buttonHeight
    anchors.top : parent.top
    anchors.bottom : tabgroupname.top

    color : "transparent"
    signal buttonClicked()
    function do_buttonclick(iconText){
        switch(iconText){
        case "Rotate":
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
            anchors.margins: 2
            anchors.fill: parent
            source: iconSource1
            /*
            MouseArea{
                anchors.fill: parent

                onEntered: {
                    basicbutton.color = "red"
                    console.log("enter")
                }
                onExited: {
                    basicbutton.color = parent.color
                    console.log("exit");
                }
            }*/
        }
        Text{
            anchors.top:parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 16
            text: iconText
            font.pixelSize: 15
            //font.bold: true
            font.family: "Arial"
            color: "#494949"
        }

    }

    //Active
    Rectangle {
        id: activebutton
        visible: false
        anchors.fill: parent
        color: "#5595a0"
        Rectangle {
            width: parent.width - 6
            height: parent.height - 6
            color: "#669fad"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Image {
            anchors.margins: 2
            anchors.fill: parent
            source: iconSource2
        }
        Text {
            anchors.top:parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 16
            text: iconText
            font.pixelSize: 15
            font.family: "Arial"
            color: "#ffffff"
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
            basicbutton.color = "#D8D8D8"
        }
        onExited: {
            basicbutton.color = parent.color
        }

        onClicked:{
            do_buttonclick(iconText)

           /* if (parent.state == "inactive"){
                box_uppertab.inbox(iconText);
                parent.state = "active";
                console.log("active");
                console.log(box_uppertab.box);
            }
            else {
                //box_uppertab.inbox("none");
                parent.state = "inactive";
                console.log("inactive");
                console.log(box_uppertab.box);
            }*/

            if (parent.state == "inactive"){
                all_off();
                parent.state = "active";
                console.log("active" + iconText);
            }
            else {
                parent.state = "inactive";
                console.log("inactive" + iconText);
            }
        }
            //loadFileWindow();



            /*(function upperButtonFunction(id){
                switch (id){
                case first_tab_button_export:{
                    console.log("exporting");
                    // collect configurations
                    var cfg = collectConfigurations();
                    se.slice(cfg);
                    break;
                }
                }
            })();*/
    }

}



