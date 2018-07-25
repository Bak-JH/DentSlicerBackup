import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    objectName: "delete_popup"
    width: 352
    height: 162
    anchors.centerIn: parent
    visible : false

    color: "#E5E5E5"
    border.width: 1
    border.color:"#CCCCCC"

    property string mid_text: mid_text
    property var targetID: -1

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


        Text {
            height:20
            id: result_text_mid

            text: "Are you sure to delete this model?"
            anchors.top: parent.top
            anchors.topMargin: 44
            anchors.horizontalCenter: parent.horizontalCenter
            verticalAlignment: Text.AlignTop
            font.pixelSize: 20
            color: "#0DA3B2"
            font.family: mainFont.name
        }



        Rectangle {
            id: okButton
            width: 138
            height: 34
            color: "#A3A3A5"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 23
            anchors.left : parent.left
            anchors.leftMargin: 24

            Text {
                id: okButton_text
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                text: "OK"
                color: "#ffffff"
                font.pixelSize: 20
                font.family: mainFont.name
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    qm.setHandCursor();
                    parent.color = "#3ea6b7"
                }
                onExited: {
                    qm.resetCursor();
                    parent.color = "#999999"
                }
                onReleased: {
                    qm.doDelete()
                    closePopUp();
                }
            }
        }
        Rectangle {
            id: cancelButton
            width: 138
            height: 34
            color: "#A3A3A5"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 23
            anchors.right : parent.right
            anchors.rightMargin: 24

            Text {
                id: cancelButton_text
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Cancel"
                color: "#ffffff"
                font.pixelSize: 20
                font.family: mainFont.name
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    qm.setHandCursor();
                    parent.color = "#3ea6b7"
                }
                onExited: {
                    qm.resetCursor();
                    parent.color = "#999999"
                }
                onReleased: {
                    closePopUp();
                }
            }
        }


    }

    function openDeletePopUp(){
        deletePopUp.visible = true
    }

    function closePopUp(){
        deletePopUp.visible = false
    }


}
