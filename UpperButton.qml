import QtQuick 2.0

Rectangle{
    property string iconSource1
    property string iconSource2
    property string iconText
    id: temp
    width: buttonWidth
    height: buttonHeight

    anchors.top : parent.top
    anchors.bottom : tabgroupname.top

    color : "transparent"

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
        }
        Text{
            anchors.top:parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 16
            text: iconText
            font.pixelSize: 16
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
            font.pixelSize: 16
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
            name: "active"
            PropertyChanges {target: activebutton; visible: true}
        }
    ]

    state: "inactive"

    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        onClicked:
            if (parent.state == "inactive"){
                parent.state = "active";
                console.log("active");
            }
            else {
                parent.state = "inactive";
                console.log("inactive");
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



