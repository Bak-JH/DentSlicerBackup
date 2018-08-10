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

    property bool isFlawOpen: false

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
                font.pixelSize: 20
                font.family: mainFont.name
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true

                onClicked:{

                    switch (popup_type){
                        case uppertab.ftrSupportViewMode:
                        case uppertab.ftrLayerViewMode:
                            function collectConfigurations(){
                                var options = uppertab.options;
                                var configurations = {};

                                // do collecting things
                                // configurations[key] = value;
                                configurations["resolution"] = options[0];
                                configurations["layer_height"] = options[1];
                                configurations["support_type"] = options[2];
                                configurations["infill_type"] = options[3];
                                configurations["raft_type"] = options[4];
                                return configurations;
                            }
                            var cfg = collectConfigurations();

                            if( popup_type == uppertab.ftrSupportViewMode ) {
                                qm.setViewMode(1);
                            } else if( popup_type == uppertab.ftrLayerViewMode ) {
                                qm.setViewMode(2);
                            }

                            yesnoPopUp.runFeature(uppertab.ftrExport, cfg);
                            break;
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

                    if( popup_type == uppertab.ftrSupportViewMode ||
                        popup_type == uppertab.ftrLayerViewMode ) {
                        qm.setViewMode(0);
                    }

                    closePopUp();
                }
            }
        }
    }



    function openYesNoPopUp(inputText_h, inputText_m, inputText_l, inputPopupType){
        yesnoPopUp.visible = true
        high_text = inputText_h
        mid_text = inputText_m
        low_text = inputText_l
        popup_type = inputPopupType

        isFlawOpen = isFlawPopupOpen()
    }

    function closePopUp(){
        yesnoPopUp.visible = false
        isFlawOpen = false
    }

    function isFlawPopupOpen(){
        var result = false
        if(yesnoPopUp.visible){
            if(high_text.indexOf("flaw") > 0){
                result = true
            }
        }

        return result
    }

    signal runFeature(int type, var config);

}
