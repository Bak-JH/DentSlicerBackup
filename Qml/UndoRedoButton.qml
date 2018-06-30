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
                    width: 26
                    height: 24
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 6
                    source:"qrc:/Resource/undo_icon.png"
                }
                Text{
                    id:undo_text
                    text:"UNDO"
                    anchors.bottom : parent.bottom
                    anchors.horizontalCenter: undo_icon.horizontalCenter
                    anchors.bottomMargin: 2

                    font.family: mainFont.name
                    font.pixelSize: 12

                }

                MouseArea{
                    anchors.fill: undo_button

                    hoverEnabled : true

                    onClicked: {
                        unDo();
                        //views.visible = !views.visible
                    }
                    onEntered: {
                        undo_button.color = "#3ea6b7"
                        undo_icon.source = "qrc:/Resource/undo_icon_select.png"
                        undo_text.color = "#ffffff"
                        qm.setHandCursor()
                    }
                    onExited: {
                        undo_button.color = "#ffffff"
                        undo_icon.source = "qrc:/Resource/undo_icon.png"
                        undo_text.color = "black"
                        qm.resetCursor()
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
                width: 26
                height: 24
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 6
                source:"qrc:/Resource/redo_icon.png"
            }
            Text{
                id:redo_text
                text:"REDO"
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: redo_icon.horizontalCenter
                anchors.bottomMargin: 2

                font.family: mainFont.name
                font.pixelSize: 12

            }
            MouseArea{
                anchors.fill: redo_button
                hoverEnabled : true

                onClicked: {
                    reDo();
                }
                onEntered: {
                    redo_button.color = "#3ea6b7"
                    redo_icon.source = "qrc:/Resource/redo_icon_select.png"
                    redo_text.color = "#ffffff"
                    qm.setHandCursor()
                }
                onExited: {
                    redo_button.color = "#ffffff"
                    redo_icon.source = "qrc:/Resource/redo_icon.png"
                    redo_text.color = "black"
                    qm.resetCursor()
                }
            }
        }
    }

}
