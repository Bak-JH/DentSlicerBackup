import QtQuick 2.0
Item{
    width: 116
    height: 52
    //anchors.top : parent.top
    //anchors.right : parent.right

    signal unDo();
    signal reDo();


    Rectangle{
        id:undo
        width: 50
        height: parent.height
        anchors.left: parent.left

        color: "transparent"

        Rectangle{//shadow
            width: parent.width-2
            height: parent.height-2
            anchors.bottom: parent.bottom
            anchors.right: parent.right

            color: "#CCCCCC"
        }

        Rectangle{//main
            width: parent.width-2
            height: parent.height-2
            anchors.top: parent.top
            anchors.left: parent.left

            color: "#ffffff"

            Rectangle{
                id : undo_button
                width : 48
                height: 50
                anchors.left : parent.left
                anchors.top: parent.top

                Image{
                    id:undo_icon
                    width: 32
                    height: 32
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 4
                    source:"qrc:/Resource/undo_icon.png"
                }
                Text{
                    text:"UNDO"
                    anchors.bottom : parent.bottom
                    anchors.horizontalCenter: undo_icon.horizontalCenter
                    anchors.topMargin: 0

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
        }
    }


    Rectangle{
        id:redo
        width: 50
        height: parent.height
        anchors.right: parent.right

        color: "transparent"

        Rectangle{//shadow
            width: parent.width-2
            height: parent.height-2
            anchors.bottom: parent.bottom
            anchors.right: parent.right

            color: "#CCCCCC"
        }

        Rectangle{//main
            id : redo_button
            width: parent.width-2
            height: parent.height-2
            anchors.top: parent.top
            anchors.left: parent.left

            color: "#ffffff"

            Image{
                id:redo_icon
                width: 32
                height: 32
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 4
                source:"qrc:/Resource/redo_icon.png"
            }
            Text{
                text:"REDO"
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: redo_icon.horizontalCenter
                anchors.bottomMargin: 0

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
