//import QtQuick 2.0
import QtQuick 2.11

Item {
    width: 240
    height: 28

    property string modelName
    property int glModelID
    property Item container;

    focus: true
//    Keys.onPressed: {
//        if(event.key === Qt.Key_Escape) {
//            icon.parent.state = 'on'
////            unselectPart(glModelID)
//            event.accepted = true;
//            console.log("왜어째서~~~반응이 없니~~~")
//        }
//        console.log("여기 무슨일이 일어난거니 오우워어")
//    }
    Keys.onEscapePressed: {
        console.log("이제는 될꺼야우워우")
        console.log(icon.parent.state)
        icon.parent.state = 'on'
        console.log(icon.parent.state)
        console.log(icon.parent.children)
        console.log(icon.parent.objectName)
        console.log(icon.parent.children.objectName)
        console.log(icon.parent.children.state)
    }

    Rectangle{
        id:background
        width:parent.width-2
        height: parent.height-2
        anchors.centerIn: parent
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
            hoverEnabled : true
            onEntered : {
                qm.setHandCursor();
            }
            onExited : qm.resetCursor();
            onClicked: {
                if(icon.parent.state == 'on'){
                    icon.parent.state = 'off';
                }
                else if(icon.parent.state == 'off'){
                    icon.parent.state = 'on';
                }
                else if(icon.parent.state == 'select'){
                    icon.parent.state = 'off';
                    unselectPart(glModelID)
                }


                if(icon.parent.state == 'on')
                    qm.modelVisible(glModelID,true);
                else
                    qm.modelVisible(glModelID,false);
            }
        }

    }

    Rectangle{
        id : deletePart
        width: 20
        anchors.right: parent.right
        anchors.rightMargin: 8
        height: parent.height
        color: "transparent"

        Image{
            anchors.centerIn:parent
            source: "qrc:/Resource/part_delete.png"
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled : true
            onEntered : qm.setHandCursor();
            onExited : qm.resetCursor();
            onClicked: {
                console.log("delete");
                if(icon.parent.state == 'on'){
                    icon.parent.state = 'select';
                    selectPart(glModelId);
                }
                deletePopUp.visible = true;
                deletePopUp.targetID = glModelID;

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
    Rectangle{
        id : clickArea
        width: parent.width-32-28
        height: parent.height
        anchors.left: icon.right
        color: "transparent"
        MouseArea {
            anchors.fill: clickArea
            onClicked: {
                console.log("clcl");
                if(icon.parent.state == 'on'){
                    icon.parent.state = 'select';
                    selectPart(glModelID);
                }
                else if(icon.parent.state == 'off')
                    return;
                else if(icon.parent.state == 'select'){
                    icon.parent.state = 'on'
                    unselectPart(glModelID)
                }
            }
        }
    }

    Text{
        id : modelNameText
        text : trimName(modelName)
        anchors.left: line.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 10

        font.family: mainFont.name
        font.pixelSize: 14

    }

    states: [
        State{
            name:"off"
            PropertyChanges { target: iconimage; source:"qrc:/resource/part_off.png" }
        },
        State{
            name:"on"
            PropertyChanges { target: iconimage; source:"qrc:/resource/part_on.png" }
            PropertyChanges { target: background; color:"transparent" }
            PropertyChanges { target: modelNameText; color:"black" }
        },
        State{
            name:"select"
            PropertyChanges { target: iconimage; source:"qrc:/Resource/part_select.png" }
            PropertyChanges { target: background; color:"#EAEAEA" }
            PropertyChanges { target: modelNameText; color:"#0DA3B2" }
            /*
            StateChangeScript {
                script: selectPart(glModelID);
            }
            */
        }
    ]

    function selectPart(ID){ // select Model in PartList by ID
        qm.selectPart(ID);
    }
    function unselectPart(ID){ // select Model in PartList by ID
        qm.unselectPart(ID);
    }




    function trimName(modelName){
        if(modelName.length > 22)
            modelName = modelName.substr(0,20) +"..."

        return modelName
    }

}
