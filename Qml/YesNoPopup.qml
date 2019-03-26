import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

Rectangle {
    objectName: "yesno_popup"
    width: 352
    height: 162 + selectedList.height * (selectedList.visible ? 1 : 0) + descriptionimage.height * (descriptionimage.visible ? 1 : 0)
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

        //shadow rectangle (for list)
        Rectangle {
            id: selectedList
            width: 300
            height: 148
            anchors.horizontalCenter: contentRect.horizontalCenter
            anchors.top: contentRect.top
            anchors.topMargin: 25
            color: "#F5F5F5"

            ScrollView {
                width: parent.width
                height: parent.height - 10
                anchors.left: parent.left
                anchors.top : parent.top
                anchors.topMargin: 5
                //anchors.leftMargin: 16
                clip: true

                ColumnLayout {
                    id: partListColumn
                    spacing:0
                }
            }
        }

        Text {
            height:20
            id: result_text_high

            text: high_text
            anchors.top: parent.top
            anchors.topMargin: 35 + (selectedList.visible ? selectedList.height + 3 : 0)
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
            anchors.topMargin: 49 + (selectedList.visible ? selectedList.height + 3 : 0)
            anchors.horizontalCenter: parent.horizontalCenter
            verticalAlignment: Text.AlignTop
            font.pixelSize: 16
            color: "#0DA3B2"
            font.family: mainFont.name
        }

        Text {
            height:20
            id: result_text_low

            text: low_text
            anchors.top: parent.top
            anchors.topMargin: 63 + (selectedList.visible ? selectedList.height + 3 : 0)
            anchors.horizontalCenter: parent.horizontalCenter
            verticalAlignment: Text.AlignTop
            font.pixelSize: 16
            color: "#0DA3B2"
            font.family: mainFont.name
        }

        //Description image
        Rectangle {
            id: descriptionimage
            width: 176.6
            height: image.height
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: contentRect.bottom
            anchors.bottomMargin: 80
            color: "white"
            Image {
                id: image
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                source: ""
            }
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
                            qm.setViewMode(1);
                            break;
                        case uppertab.ftrExport:
                        // case uppertab.ftrSupportViewMode:
                        case uppertab.ftrLayerViewMode:
                            function collectConfigurations(){
                                var options = uppertab.options;
                                var configurations = {};

                                // do collecting things
                                // configurations[key] = value;
                                console.log("collectConfigurations")
                                configurations["resolution"] = options[0];
                                configurations["layer_height"] = options[1];
                                configurations["resin_type"] = options[2];
                                configurations["support_type"] = options[3];
                                configurations["infill_type"] = options[4];
                                configurations["raft_type"] = options[5];
                                return configurations;
                            }
                            var cfg = collectConfigurations();

                            if (popup_type == uppertab.ftrExport){
                                cfg["temporary"] = "false";
                                yesnoPopUp.runGroupFeature(uppertab.ftrExport, "", 0, 0, 0, cfg);
                                //yesnoPopUp.runFeature(uppertab.ftrExport, cfg);
                            } /* else if( popup_type == uppertab.ftrSupportViewMode ) {
                                cfg["temporary"] = "true";
                                qm.setViewMode(1);
                                yesnoPopUp.runGroupFeature(uppertab.ftrExport, "", 0, 0, 0, cfg);
                                //yesnoPopUp.runFeature(uppertab.ftrExport, cfg);
                            } */ else if( popup_type == uppertab.ftrLayerViewMode ) {
                                cfg["temporary"] = "true";
                                qm.setViewMode(2);
                                yesnoPopUp.runGroupFeature(uppertab.ftrExport, "", 0, 0, 0, cfg);
                                //yesnoPopUp.runFeature(uppertab.ftrExport, cfg);
                            }

                            break;
                        case uppertab.ftrSave:
                            qm.save();
                            console.log("save called by YesNoPopup");
                            break;
                        case uppertab.ftrRepair:
                            qm.fixMesh();
                            console.log("repair called");
                            break;
                        case uppertab.ftrCut:
                            break;
                        case uppertab.ftrArrange:
                            qm.runArrange();
                            console.log("arrange called by YesNoPopup");
                            uppertab.all_off();
                            break;
                        case uppertab.ftrDelete:
                            qm.deleteSelectedModels();
                            console.log("delete called by YesNoPopup");
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
                    } else if (popup_type == uppertab.ftrSave)
                        qm.closeSave();
                    closePopUp();
                    uppertab.all_off();

                }
            }
        }
    }

    function openYesNoPopUp(selectedList_vis, inputText_h, inputText_m, inputText_l, inputText_fontsize, image_source, inputPopupType, yesNo_okCancle){
        yesnoPopUp.visible = true
        selectedList.visible = selectedList_vis
        high_text = inputText_h
        result_text_high.font.pixelSize = inputText_fontsize
        mid_text = inputText_m
        result_text_mid.font.pixelSize = inputText_fontsize
        low_text = inputText_l
        result_text_low.font.pixelSize = inputText_fontsize
        popup_type = inputPopupType
        if (image_source === "") descriptionimage.visible = false
        else {
            descriptionimage.visible = true
            image.source = image_source
        }
        // yesNo_okCancle = 0 => YES or NO
        // yesNo_okCancle = 1 => OK or Cancel
        if (yesNo_okCancle) okCancle()
        else yesNo()

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
    function yesNo() {
        yesButton_text.text = "YES"
        noButton_text.text = "NO"
    }

    function okCancle(){
        yesButton_text.text = "OK"
        noButton_text.text = "Cancel"
    }

    signal runGroupFeature(int type, string state, double arg1, double arg2, double arg3, var data);
    //signal runFeature(int type, var config);

    function addPart(fileName, ID){ // add in list with filename
        var newComponent = Qt.createComponent("LeftTabPartListElement.qml")

        console.log("added part " + fileName);
        var newPart = newComponent.createObject(partListColumn, {"modelName" : fileName, "state" : "list", "objectName" : "qwer", "glModelID" : ID, "trimLength" : 37, "vis" : false,"fontsize" : 17})
    }

    function deletePart(ID){ // delete in list by ID
        console.log("yesno_popup: deletePart")
        for(var i=0 ; i<partListColumn.children.length; i++){
            if(partListColumn.children[i].glModelID === ID){
                partListColumn.children[i].destroy()
            }
        }
    }

}
