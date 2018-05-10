import QtQuick 2.0

import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

Rectangle {
    objectName: "partList"
    property real originalHeight: 336
    width : 264
    height : originalHeight

    color: "transparent"

    state : "open"

    /************************** upper **************************/
    Rectangle{
        id : tab
        width: parent.width
        height: 28
        anchors.left : parent.left
        anchors.top :  parent.top

        //color: "#CECECE"
        color: "transparent"

        Text{
            height: parent.height
            text : "Part List"

            color:"#888D8E"
            font.family: mainFont.name
            font.pixelSize: 16

            anchors.bottom: parent.bottom
            anchors.left : parent.left
            anchors.leftMargin: 14
            verticalAlignment: Text.AlignBottom
        }

        Image{
            id : arrow
            source: "qrc:/resource/triangle.png"
            width : 12
            height : 10

            anchors.verticalCenter: parent.verticalCenter
            anchors.right : parent.right
            anchors.rightMargin: 12

            MouseArea {
                    anchors.fill: parent
                    hoverEnabled : true
                    onEntered : qm.setHandCursor();
                    onExited : qm.resetCursor();
                    onClicked: { ltpl.state == 'open' ? ltpl.state = "close" : ltpl.state = 'open';}
            }
        }

    }

    /************************** content **************************/

    Rectangle{
        id : content
        width: parent.width
        height: parent.height - 24
        anchors.left : parent.left
        anchors.top :  tab.bottom

        color:"transparent"

        Rectangle{
            width : 240
            height: 294

            anchors.top : content.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 4

            color : "#F9F9F9"
            border.width: 1
            border.color:"#C6C6C6"
            Item {
                width: parent.width
                height: parent.height
                anchors.left: parent.left
                anchors.top : parent.top
                //anchors.leftMargin: 16

                ColumnLayout {
                    id: partListColumn
                    spacing:0
                    /*
                    LeftTabPartListElement{
                        modelName : "stst"
                        state : "on"
                        objectName : "qwer"
                        glModelID : -1
                    }*/
                }
            }


        }
    }

    states: [
        State{
            name:"open"
            PropertyChanges { target: arrow; rotation:0 }
            PropertyChanges { target: content; visible:true }
            PropertyChanges { target: ltpl; height : originalHeight }
        },
        State{
            name:"close"
            PropertyChanges { target: arrow; rotation:180 }
            PropertyChanges { target: content; visible:false }
            PropertyChanges { target: ltpl; height : 28 }
        }
    ]

    function addPart(fileName, ID){ // add in list with filename
        var newComponent = Qt.createComponent("LeftTabPartListElement.qml")
        var newPart = newComponent.createObject(partListColumn, {"modelName" : fileName, "state" : "on", "objectName" : "qwer", "glModelID" : ID})
        console.log("glmodel  glglgl  : " + ID);
    }

    function deletePart(ID){ // delete in list by ID
        for(var i=0 ; i<partListColumn.children.length; i++){
            console.log("iddididididid   " + partListColumn.children[i].glModelID)
            if(partListColumn.children[i].glModelID === ID){
                partListColumn.children[i].destroy()
            }
        }
    }

    function selectPartByModel(ID){
        for(var i=0 ; i<partListColumn.children.length; i++){
            console.log("selsel   " + partListColumn.children[i].glModelID)
            if(partListColumn.children[i].glModelID === ID){
                partListColumn.children[i].state = 'select'
            }
        }
    }
    function unselectPartByModel(ID){
        for(var i=0 ; i<partListColumn.children.length; i++){
            console.log("unselunsel   " + partListColumn.children[i].glModelID)
            if(partListColumn.children[i].glModelID === ID){
                partListColumn.children[i].state = 'on'
            }
        }
    }

}
