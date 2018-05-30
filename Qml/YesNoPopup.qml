import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    objectName: "yesno_popup"
    width: 352
    height: 162
    anchors.centerIn: parent
    visible : false

    color: "#E5E5E5"
    border.width: 1
    border.color:"#CCCCCC"

    property string high_text: high_text
    property string mid_text: mid_text
    property string low_text: low_text
    property int popup_type: popup_type

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
            id: result_text_high

            text: high_text
            anchors.top: parent.top
            anchors.topMargin: 30
            anchors.horizontalCenter: parent.horizontalCenter
            verticalAlignment: Text.AlignTop
            font.pixelSize: 16
            color: "#0DA3B2"
            font.family: mainFont.name
        }

        Text {
            height:20
            id: result_text_mid

            text: mid_text
            anchors.top: parent.top
            anchors.topMargin: 44
            anchors.horizontalCenter: parent.horizontalCenter
            verticalAlignment: Text.AlignTop
            font.pixelSize: 16
            color: "#0DA3B2"
            font.family: mainFont.name
        }

        Text {
            height:20
            id: result_low

            text: low_text
            anchors.top: parent.top
            anchors.topMargin: 58
            anchors.horizontalCenter: parent.horizontalCenter
            verticalAlignment: Text.AlignTop
            font.pixelSize: 16
            color: "#0DA3B2"
            font.family: mainFont.name
        }

        Rectangle {
            id: yesButton
            width: 138
            height: 34
            color: "#A3A3A5"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 23
            anchors.left: parent.left
            anchors.leftMargin: 30

            signal yesClicked()

            Text {
                id: yesButton_text
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                text: "YES"
                color: "#ffffff"
                font.pixelSize: 24
                font.family: mainFont.name
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true

                onClicked:{

                    switch (popup_type){
                        case uppertab.ftrRepair:
                            qm.fixMesh();
                            console.log("repair called");
                            break;
                        case uppertab.ftrCut:
                            break;
                        case uppertab.ftrArrange:
                            qm.runArrange();
                            break;
                        default:
                            break;
                    }
                }

                onEntered: {
                    qm.setHandCursor();
                    parent.color = "#BCBCBE"
                }
                onExited: {
                    qm.resetCursor();
                    parent.color = "#A3A3A5"
                }
                onPressed: parent.color = "#3ea6b7"
                onReleased: {
                    closePopUp();
                    parent.color = "#A3A3A5"
                }
            }
        }

        Rectangle {
            id: noButton
            width: 138
            height: 34
            color: "#A3A3A5"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 23
            anchors.right: parent.right
            anchors.rightMargin: 30

            Text {
                id: noButton_text
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                text: "NO"
                color: "#ffffff"
                font.pixelSize: 24
                font.family: mainFont.name
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    qm.setHandCursor();
                    parent.color = "#BCBCBE"
                }
                onExited: {
                    qm.resetCursor();
                    parent.color = "#A3A3A5"
                }
                onPressed: parent.color = "#3ea6b7"
                onReleased: {
                    closePopUp();
                    parent.color = "#A3A3A5"
                }
            }
        }
    }







    function openYesNoPopUp(inputText_h, inputText_m, inputText_l, inputPopupType){
        console.log("open resulttttttttttttttt")
        yesnoPopUp.visible = true
        high_text = inputText_h
        mid_text = inputText_m
        low_text = inputText_l
        popup_type = inputPopupType
    }

    function closePopUp(){
        yesnoPopUp.visible = false
    }


}
