import QtQuick 2.0

Item {
    width: 232
    height: 28

    property string modelName
    property int glModelID

    Rectangle{
        width:parent.width
        height: parent.height-1
        color: "transparent"

    }

    Rectangle{
        width : parent.width
        height: 1

        anchors.bottom: parent.bottom
        color: "#C6C6C6"

    }

    Rectangle{
        id : icon
        width: 32
        anchors.left: parent.left
        height: parent.height
        color: "transparent"

        Image{
            id : iconimage
            anchors.centerIn:parent
            source: "qrc:/resource/part_on.png"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                icon.parent.state == 'on' ? icon.parent.state = "off" : icon.parent.state = 'on';
                //ModelVisible(icon.parent.state);
                qm.ModelVisible(glModelID,icon.parent.state);
            }
        }
    }
    Rectangle{
        id:line
        width: 1
        height: parent.height
        anchors.left:icon.right
        anchors.top : parent.top
        color: "#C6C6C6"
    }

    Text{
        text : modelName
        anchors.left: line.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 10

        font.family: "Arial"
        font.pixelSize: 14

    }

    states: [
        State{
            name:"on"
            PropertyChanges { target: iconimage; source:"qrc:/resource/part_on.png" }
        },
        State{
            name:"off"
            PropertyChanges { target: iconimage; source:"qrc:/resource/part_off.png" }
        }
    ]
    /*
    function ModelVisible(isVisible){
        console.log("ididid  "+ glModelID);
        qm.ModelVisible(glModelID,isVisible);
    }*/
}
