import QtQuick 2.0

Item {
    width: 240
    height: 28

    property string modelName
    property int glModelID
    property Item container
    property int trimLength
    property bool vis
    property int fontsize

    property int ftrDelete : 18

    Rectangle{
        id:background
        width:parent.width-2
        height: parent.height-2
        anchors.centerIn: parent
        color: "transparent"

    }

    Rectangle{
        visible: vis
        width : parent.width
        height: 1
        anchors.bottom: parent.bottom
        color: "#C6C6C6"

    }

    Rectangle{
        id : icon
        visible: vis
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
            onEntered : qm.setHandCursor();
            onExited : qm.resetCursor();
            onClicked: {
                if(icon.parent.state == 'on')
                    icon.parent.state = 'off';
                else if(icon.parent.state == 'off')
                    icon.parent.state = 'on';
                else if(icon.parent.state == 'select'){
                    icon.parent.state = 'off';
                    unselectPart(glModelID)
                    qm.deleteList(glModelID)
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
        visible: vis
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
                //if (icon.parent.state == "on") {
                if (qm.getSelectedModelsSize() !== 1) selectPart(glModelID)
                else if (icon.parent.state != "select") selectPart(glModelID)
                icon.parent.state = "select"
                //}
                //deletePopUp.visible = true;
                //deletePopUp.popupDelete();
                //deletePopUp.targetID = glModelID;
                yesnoPopUp.openYesNoPopUp(false, "", "Are you sure to delete this model?", "", 18, "", ftrDelete, 0)

            }
        }
    }


    Rectangle{
        id:line
        visible: vis
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
                    console.log("group selection active =" + qm.getGroupSelectionActive()) // shift pressed
                    if (qm.getSelectedModelsSize() > 1 && !qm.getGroupSelectionActive()) {
                        selectPart(glModelID)
                    } else {
                        icon.parent.state = 'on'
                        unselectPart(glModelID)
                        qm.deleteList(glModelID)
                    }
                }

            }
        }
    }

    Text{
        id : modelNameText
        text : vis ? trimName(modelName, trimLength) : "· " + trimName(modelName, trimLength)
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: vis ? 43 : 13

        font.family: mainFont.name
        //font.pixelSize: 14
        font.pixelSize: fontsize
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
            PropertyChanges { target: modelNameText; color: "#0DA3B2" }
            /*
            StateChangeScript {
                script: selectPart(glModelID);
            }
            */
        },
        State{
            name: "list"
            PropertyChanges { target: modelNameText; color: "#888D8E" }
            PropertyChanges { target: modelNameText; font.bold: true }
        }
    ]

    function selectPart(ID){ // select Model in PartList by ID
        qm.selectPart(ID);
    }
    function unselectPart(ID){ // select Model in PartList by ID
        qm.unselectPart(ID);
    }




    function trimName(modelName, length){
        if (modelName.length > length) modelName = modelName.substr(0, length-2) + "..."

        return modelName
    }

}
