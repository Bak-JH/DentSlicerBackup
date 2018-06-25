import QtQuick 2.0
Item{
    width: 104
    height: 52
    //anchors.top : parent.top
    //anchors.right : parent.right

    signal unDo();
    signal reDo();

    Rectangle{//shadow
        id : shadowRect
        width: parent.width-2
        height: parent.height-2
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        color: "#CCCCCC"
    }

    Rectangle{//main
        id : contentRect
        width: parent.width-2
        height: parent.height-2
        anchors.top: parent.top
        anchors.left: parent.left


        color: "#ffffff"


        Rectangle{
            id : undo_button
            width : 50
            height: 50
            anchors.left : parent.left
            anchors.top: parent.top

            Image{
                id:undo_icon
                width: 32
                height: 32
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                source:"qrc:/Resource/undo_icon.png"
            }
            Text{
                text:"UNDO"
                anchors.top : parent.top
                anchors.horizontalCenter: undo_icon.horizontalCenter
                anchors.topMargin: 5

                font.family: mainFont.name
                font.pixelSize: 12

            }

            MouseArea{
                anchors.fill: undo_button

                hoverEnabled : true
                onEntered : qm.setHandCursor();
                onExited : qm.resetCursor();

                onClicked: {
                    unDo();
                    //views.visible = !views.visible
                }
            }
        }

        Rectangle{ // mid line
            width: 2
            height: parent.height-6
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            color:"#eaeaea"
        }

        Rectangle{
            id : redo_button
            width : 50
            height: 50
            anchors.right : parent.right
            anchors.top: parent.top

            Image{
                id:redo_icon
                width: 32
                height: 32
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                source:"qrc:/Resource/redo_icon.png"
            }
            Text{
                text:"REDO"
                anchors.top : parent.top
                anchors.horizontalCenter: redo_icon.horizontalCenter
                anchors.topMargin: 5

                font.family: mainFont.name
                font.pixelSize: 12

            }
            MouseArea{
                anchors.fill: redo_button

                hoverEnabled : true
                onEntered : qm.setHandCursor();
                onExited : qm.resetCursor();

                onClicked: {
                    reDo();
                }
            }
        }
    }

}
