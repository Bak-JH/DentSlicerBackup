import QtQuick 2.0

import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

Rectangle {
    objectName: "partList"
    property real originalHeight: 350
    width : 260
    height : originalHeight

    color: "transparent"

    state : "open"

    /************************** upper **************************/
    Rectangle{
        id : tab
        width: parent.width
        height: 24
        anchors.left : parent.left
        anchors.top :  parent.top

        color: "#CECECE"

        Text{
            text : "Part List"

            color:"#303030"
            font.family: "Arial"
            font.pointSize: 12

            anchors.verticalCenter: parent.verticalCenter
            anchors.left : parent.left
            anchors.leftMargin: 10
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
            width : 232
            height: 294

            anchors.top : content.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 12

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
                        modelName :"tttttt"
                        state: "on"
                    }
                    LeftTabPartListElement{
                        modelName :"cc"
                        state: "off"
                    }
                    LeftTabPartListElement{
                        modelName :"tttaasat"
                        state: "off"
                    }
                    */
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
            PropertyChanges { target: ltpl; height : 24 }
        }
    ]

    function addPart(fileName, ID){ // add in list with filename
        var newComponent = Qt.createComponent("LeftTabPartListElement.qml")
        //var newPart = Qt.createQmlObject(test,container,'firstObject');
        var newPart = newComponent.createObject(partListColumn, {"modelName" : fileName, "state" : "on", "objectName" : "qwer", "glModelID" : ID})
        console.log("glmodel  glglgl  : " + ID);
    }

    function deletePart(num){ // delete in list by number
        partListColumn.children[num].destroy()
    }

}
