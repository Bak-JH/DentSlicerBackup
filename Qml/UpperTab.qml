import Qt3D.Core 2.0
import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
//import QtQuick.Controls.Styles 1.0
import QtQuick.Controls.Styles 1.4



Rectangle {
    id: box_uppertab
    color: "#E2E1E1"
    property int buttonWidth : 83
    property int buttonHeight : 97
    property var options: []


    // feature type number
    property int ftrOpen : 1
    property int ftrSave : 2
    property int ftrExport : 3
    property int ftrMove : 4
    property int ftrRotate : 5
    property int ftrLayFlat : 6
    property int ftrArrange : 7
    property int ftrOrient : 8
    property int ftrScale : 9
    property int ftrRepair : 10
    property int ftrCut : 11
    property int ftrShellOffset : 12
    property int ftrExtend : 13
    property int ftrSupport : 14
    property int ftrLabel : 15


    function all_off() {
        first_tab_button_open.state = "inactive";
        first_tab_button_export.state = "inactive";
        first_tab_button_save.state = "inactive";
        second_tab_button_arrange.state = "inactive";
        second_tab_button_layflat.state = "inactive";
        second_tab_button_move.state = "inactive";
        second_tab_button_orient.state = "inactive";
        second_tab_button_rotate.state = "inactive";
        third_tab_button_autorepair.state = "inactive";
        third_tab_button_cut.state = "inactive";
        third_tab_button_shelloffset.state = "inactive";
        third_tab_button_scale.state = "inactive";
        fourth_tab_button_extend.state = "inactive";
        fourth_tab_button_label.state = "inactive";
        fourth_tab_button_support.state = "inactive";

        console.log("all off");
    }

    property real move_x_value
    property real move_y_value
    property real move_z_value
    property real rotate_x_value
    property real rotate_y_value
    property real rotate_z_value
    property real scale_x_value
    property real scale_y_value
    property real scale_z_value




    /*
    Rectangle{
        anchors.fill: parent

        color: "#E2E1E1"
    }*/

    Rectangle{
        id : bottomine
        height: 3
        width: parent.width
        //anchors.fill: parent

        anchors.left : parent.left
        //anchors.right : parent.right
        anchors.bottom: parent.bottom
        color: "#A8A8A8"
    }

    Rectangle{
        id : tabgroupname
        width: parent.width
        height: 16
        //anchors.fill: parent

        anchors.left : parent.left
        anchors.right : parent.right
        anchors.bottom: bottomine.top
        color: "#CECECE"

        Text{
            id : firsttabname
            width : buttonWidth * 3 + 2
            height: parent.height
            anchors.left : parent.left
            anchors.bottom: parent.bottom

            text : "FILE"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            font.pixelSize: 12
            color: "#636363"
        }
        Text{
            id : secondtabname
            width : buttonWidth * 5 + 2
            height: parent.height
            anchors.left : firsttabname.right
            anchors.bottom: parent.bottom

            text : "POSITION"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            font.pixelSize: 12
            color: "#636363"
        }
        Text{
            id : thirdtabname
            width : buttonWidth * 4 + 2
            height: parent.height
            anchors.left : secondtabname.right
            anchors.bottom: parent.bottom

            text : "MODIFY"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            font.pixelSize: 12
            color: "#636363"
        }
        Text{
            id : fourthtabname
            width : buttonWidth * 3 + 2
            height: parent.height
            anchors.left : thirdtabname.right
            anchors.bottom: parent.bottom

            text : "EDIT"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.family: "Arial"
            font.pixelSize: 12
            color: "#636363"
        }
    }

    Item{

        id : firsttab
        width : buttonWidth * 3 + 2
        height: parent.height

        anchors.left : parent.left
        anchors.top : parent.top
        //anchors.bottom : parent.bottom

        //color: "transparent"
        UpperButton{
            id : first_tab_button_open
            objectName : "open"
            //anchors.left: parent.left
            iconSource1: "qrc:/resource/upper_open.png"
            iconSource2: "qrc:/Resource/upper2_open.png"
            iconText: "Open"
            MouseArea{
                anchors.fill: parent
                onClicked:{
                    filedialogload.open();
                }
            }
        }

        UpperButton{
            id : first_tab_button_save
            objectName: "saveBtn"

            anchors.left: first_tab_button_open.right
            iconSource1: "qrc:/resource/upper_save.png"
            iconSource2: "qrc:/Resource/upper2_save.png"
            iconText: "Save"
            signal runFeature(int type);
            MouseArea{
                anchors.fill: parent
                onClicked:{
                    parent.runFeature(ftrSave);
                }

            }
        }


        UpperButton{
            id : first_tab_button_export
            objectName: "exportBtn"
            anchors.left: first_tab_button_save.right
            iconSource1: "qrc:/resource/upper_export.png"
            iconSource2: "qrc:/Resource/upper2_export.png"
            iconText: "Export"

            MouseArea{
                anchors.fill: parent
                onClicked:{
                    function collectConfigurations(){
                        var configurations = "";
                        configurations += "r#"+options[0] + "/";
                        configurations += "l#"+options[1] + "/";
                        configurations += "s#"+options[2] + "/";
                        configurations += "f#"+options[3] + "/";
                        configurations += "b#"+options[4];
                        return configurations;

                        /*var configurations = {};

                        // do collecting things
                        // configurations[key] = value;
                        configurations["resolution"] = "resolution#"+options[0];
                        configurations["layer_height"] = "layer_height#"+options[1];
                        configurations["support"] = "support#"+options[2];
                        configurations["infill"] = "infill#"+options[3];
                        configurations["raft"] = "raft#"+options[4];
                        return configurations;*/
                    }

                    console.log(parent.id);
                    console.log(parent.parent.id);

                    console.log("exporting");
                    // collect configurations
                    var cfg = collectConfigurations();
                    //se.slice(cfg);
                    parent.runFeature(ftrExport, cfg);
                }
            }
            signal runFeature(int type, string config);
        }

        Rectangle{
            id : first_tab_right_line
            width: 2
            height: buttonHeight

            anchors.right: parent.right
            anchors.top : parent.top
            //anchors.bottom : tabgroupname.top

            color : "#CECECE"
        }
    }

    Item{
        id : secondtab
        width : buttonWidth * 5 + 2
        height: parent.height

        anchors.left : firsttab.right
        anchors.top : parent.top
        anchors.bottom : parent.bottom

        UpperButton{
            id : second_tab_button_move

            anchors.left: parent.left
            iconSource1: "qrc:/resource/upper_move.png"
            iconSource2: "qrc:/Resource/upper2_move.png"
            iconText: "Move"
        }
        UpperButton{
            id : second_tab_button_rotate
            objectName: "rotateButton"
            anchors.left: second_tab_button_move.right
            iconSource1: "qrc:/resource/upper_rotate.png"
            iconSource2: "qrc:/Resource/upper2_rotate.png"
            iconText: "Rotate"
            signal runGroupFeature(int type, string state);
            onButtonClicked:{
                   runGroupFeature(ftrRotate, state);
            }
        }
        UpperButton{
            id : second_tab_button_layflat

            anchors.left: second_tab_button_rotate.right
            iconSource1: "qrc:/resource/upper_layflat.png"
            iconSource2: "qrc:/Resource/upper2_layflat.png"
            iconText: "Lay flat"
        }
        UpperButton{
            id : second_tab_button_arrange

            anchors.left: second_tab_button_layflat.right
            iconSource1: "qrc:/resource/upper_arrange.png"
            iconSource2: "qrc:/Resource/upper2_arrange.png"
            iconText: "Arrange"
        }
        UpperButton{
            id : second_tab_button_orient

            anchors.left: second_tab_button_arrange.right
            iconSource1: "qrc:/resource/upper_orientation.png"
            iconSource2: "qrc:/Resource/upper2_orient.png"
            iconText: "Orient"
        }

        Rectangle{
            id : second_tab_right_line
            width: 2
            height: buttonHeight

            anchors.right: parent.right
            anchors.top : parent.top
            //anchors.bottom : tabgroupname.top

            color : "#CECECE"
        }

    }

    Item{
        id : thirdtab
        width : buttonWidth * 4 + 2
        height: parent.height

        anchors.left : secondtab.right
        anchors.top : parent.top
        anchors.bottom : parent.bottom
        //color: "transparent"
        UpperButton{
            id : third_tab_button_scale
            //state: inactive
            anchors.left: parent.left
            iconSource1: "qrc:/resource/upper_scale.png"
            iconSource2: "qrc:/Resource/upper2_scale.png"
            iconText: "Scale"
            /*
            Connections {
                target: box_uppertab
                box: third_tab_button_scale.iconText
            }
            visible: if(box_uppertab.box != iconText) return false;
            */

            /*
            parent.parent.box: iconText
            visible: if (parent.parent.box != iconText) return false;
            */

            //state: if(box_uppertab.box != "Scale") return "inactive";


        }

        UpperButton{
            id : third_tab_button_autorepair
            anchors.left: third_tab_button_scale.right
            iconSource1: "qrc:/resource/upper_autorepair.png"
            iconSource2: "qrc:/Resource/upper2_autorepair.png"
            iconText: "Auto Repair"
            /*
            Connections {
                target: box_uppertab
                box: third_tab_button_autorepair.iconText
            }
            visible: if(box_uppertab.box != iconText) return false;
            */

        }

        UpperButton{
            id : third_tab_button_cut
            //state:"inactive"
            anchors.left: third_tab_button_autorepair.right
            //anchors.leftMargin: 4
            //anchors.bottom: third_tab_button_autorepair.bottom
            //anchors.bottomMargin: 3
            iconSource1: "qrc:/resource/upper_cut.png"
            iconSource2: "qrc:/Resource/upper2_cut.png"
            iconText: "Cut"
        }
            
        UpperButton{
            id : third_tab_button_shelloffset

            anchors.left: third_tab_button_cut.right
            iconSource1: "qrc:/resource/upper_shelloffset.png"
            iconSource2: "qrc:/Resource/upper2_shelloffset.png"
            iconText: "Shell Offset"
        }

        Rectangle{
            id : third_tab_right_line
            width: 2
            height: buttonHeight

            anchors.right: parent.right
            anchors.top : parent.top
            //anchors.bottom : tabgroupname.top

            color : "#CECECE"
        }



    Item{
        id : fourthtab
        width : buttonWidth * 3 + 2
        height: parent.height

        anchors.left : thirdtab.right
        anchors.top : parent.top
        anchors.bottom : parent.bottom
        //color: "transparent"

        UpperButton{
            id : fourth_tab_button_extend

            anchors.left: parent.left
            iconSource1: "qrc:/Resource/upper_extend.png"
            iconSource2: "qrc:/Resource/upper2_extend.png"
            iconText: "Extend"
        }

        UpperButton{
            id : fourth_tab_button_support

            anchors.left: fourth_tab_button_extend.right
            iconSource1: "qrc:/resource/upper_support.png"
            iconSource2: "qrc:/Resource/upper2_support.png"
            iconText: "Support"

        }
        UpperButton{
            id : fourth_tab_button_label

            anchors.left: fourth_tab_button_support.right
            iconSource1: "qrc:/resource/upper_label.png"
            iconSource2: "qrc:/Resource/upper2_label.png"
            iconText: "Label"
        }

        Rectangle{
            id : fourth_tab_right_line
            width: 2
            height: buttonHeight

            anchors.right: parent.right
            anchors.top : parent.top
            //anchors.bottom : tabgroupname.top

            color : "#CECECE"
        }
    }


    function openFile(){
        //fileDialog.open()
        console.log("button")
        var compo = Qt.createComponent("Model.qml");

        var loadmodel = compo.createObject(sceneRoot);

    }
}

    // Pop-Up
    Item {
        id: popup
        width: 300
        height: 300
        anchors.left: parent.left
        anchors.top: parent.bottom
        anchors.topMargin: 20
        anchors.leftMargin: 280
        //color: "transparent"


        //4. PopUp - Move
        PopUp {
            id: popup_move
            funcname: "Move"
            height: 250
            //detail1: ""
            //detail2: ""
            //image: ""
            detailline1_vis: false
            detailline2_vis: false
            applyfinishbutton_vis: false

            okbutton_vis: false
            applybutton_vis: true
            applybutton_text: "Finish"
            descriptionimage_vis: false
            numberbox_vis: true
            numberbox_nameing_vis: true
            numberbox_width: 110
            numberbox_y: 45
            //numbox_default: 0
            numbox_updown_scale: 1
            numbox_value_x: move_x_value
            numbox_value_y: move_y_value
            numbox_value_z: move_z_value
            state: second_tab_button_move.state=="active" ? "active" : "inactive"
        }

        //5. PopUp - Rotate
        PopUp {
            id: popup_rotate
            objectName: "rotatePopup"
            funcname: "Rotate"
            height: 250
            detail1: ""
            detail2: ""
            //image: ""
            detailline1_vis: false
            detailline2_vis: false
            okbutton_vis: false
            applyfinishbutton_vis: false
            applybutton_vis: true
            applybutton_text: "Finish"
            descriptionimage_vis: false
            numberbox_vis: true
            numberbox_nameing_vis: true
            numberbox_width: 110
            numberbox_y: 45
            //numbox_default: 0
            numbox_updown_scale: 5
            number_unit: "°"
            signal runFeature(int type);
            onApplyClicked: {
                console.log("rotate")
                runFeature(ftrRotate);
            }
            state: second_tab_button_rotate.state=="active" ? "active" : "inactive"
        }

        //6. PopUp - Lay Flat
        PopUp {
            id: popup_layflat
            funcname: "Lay flat"
            height: 220
            detail1: "Click the surface to face it down."
            //detail2: ""
            image: "qrc:/Resource/popup_image/image_layflat.png"
            detailline1_vis: false
            detailline2_vis: false
            imageHeight: 76
            applyfinishbutton_vis: false
            okbutton_vis: false
            applybutton_vis: true
            applybutton_text: "Finish"
            descriptionimage_vis: true
            state: second_tab_button_layflat.state=="active" ? "active" : "inactive"
        }

        //7. PopUp - Arrange
        PopUp {
            objectName: "arrangePopup"
            id: popup_arrange
            funcname: "Arrange"
            height: 220
            detail1: "Click Apply to align the models."
            //detail2: ""
            image: "qrc:/Resource/popup_image/image_arrange.png"
            detailline1_vis: false
            detailline2_vis: false
            imageHeight: 76
            applyfinishbutton_vis: true
            okbutton_vis: false
            applybutton_vis: false
            descriptionimage_vis: true
            state: second_tab_button_arrange.state=="active" ? "active" : "inactive"
            signal runFeature(int type);
            onApplyClicked: {
                console.log("arrange");
                runFeature(ftrArrange);
            }
        }

        //8. PopUp - Orient
        PopUp {
            objectName: "orientPopup"
            id: popup_orient
            funcname: "Orient"
            function show_popup(id){
                if(id=="progress_popup"){
                    progress_popup.state="active";
                }
                if(id=="result_orient"){
                    result_text.text="The model is already.";
                    result_orient.state="active";
                }
            }

            height: 220
            detail1: "Click Apply to rotate the model."
            //detail2: ""
            image: "qrc:/Resource/popup_image/image_orient.png"
            detailline1_vis: false
            detailline2_vis: false
            imageHeight: 76
            okbutton_vis: false
            applybutton_vis: false
            applyfinishbutton_vis: true
            descriptionimage_vis: true
            signal runFeature(int type);
            onApplyClicked: {
                console.log("auto orientation")
                runFeature(ftrOrient);
            }
            state: second_tab_button_orient.state=="active" ? "active" : "inactive"
        }

        //9. PopUp - Scale
        PopUp {
            id: popup_scale
            funcname: "Scale"
            height: 330
            detail1: "Current size"
            detail2: "Scale"
            //image: ""
            detailline1_vis: true
            detailline2_vis: true
            imageHeight: 112
            applyfinishbutton_vis: false
            okbutton_vis: false
            applybutton_vis: true
            applybutton_text: "Finish"
            descriptionimage_vis: false
            numberbox_vis: true
            numberbox_nameing_vis: true
            numberbox_width: 110
            numbox_updown_vis: false
            numberbox_y: 75
            //numbox_default: 0
            numbox_total_height: 120
            state: third_tab_button_scale.state=="active" ? "active" : "inactive"

            //detail1 - size lock
            Rectangle {
                width: 25
                height: 100
                anchors.right: parent.right
                anchors.rightMargin: 35
                y: 85
                color: "transparent"
                Image {
                    id: scale_lock
                    anchors.left: parent.left
                    anchors.top: parent.top
                    source: "qrc:/Resource/popup_image/scale_lock.png"
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: scale_lock.source == "qrc:/Resource/popup_image/scale_lock.png" ?
                                   scale_lock.source = "qrc:/Resource/popup_image/scale_unlock.png"
                                 : scale_lock.source = "qrc:/Resource/popup_image/scale_lock.png"
                }
            }

            numbox_detail2_vis: true
            numberbox_detail2_y: 240
            numbox_detail2_defalult: 100
            numbox_updown_scale: 10
            number_unit_detail2: "%"

        }

        //10. PopUp - Auto Repair
        PopUp {
            id: popup_autorepair
            objectName: "repairPopup"
            funcname: "Auto Repair"
            height: 220
            detail1: "Click Apply to fix the model."
            //detail2: ""
            image: "qrc:/Resource/popup_image/image_autorepair.png"
            detailline1_vis: false
            detailline2_vis: false
            imageHeight: 76
            applyfinishbutton_vis: true
            applybutton_vis: false
            okbutton_vis: false
            descriptionimage_vis: true
            state: third_tab_button_autorepair.state=="active" ? "active" : "inactive"
            signal runFeature(int type);
            onApplyClicked: {
                console.log("auto repair");
                runFeature(ftrRepair);
            }
        }

        //11. PopUp - Cut
        PopUp {
            objectName: "cutPopup"
            id: popup_cut
            funcname: "Cut"
            height: 300
            detail1: "Cutting Surface"
            detail2: "After treatment"
            detailline1_vis: true
            detailline2_vis: true
            applyfinishbutton_vis: true
            applybutton_vis: false
            okbutton_vis:false
            descriptionimage_vis: false
            leftselectimage_vis: true
            rightselectimage_vis: true
            radiobutton1_vis: true
            radiobutton2_vis: true
            slider_vis: true
            imageHeight: 70
            state: third_tab_button_cut.state=="active" ? "active" : "inactive"

            onFlatModeClicked: {
                console.log("flat mode selected");
                cutModeSelected(1);
            }

            onCurveModeClicked: {
                console.log("curve mode selected");
                cutModeSelected(2);
            }

            onApplyClicked: {
                console.log("ApplyClicked")
                //runFeature(ftrCut);
                generatePlane();
                cutModeSelected(9999);
            }

            // on Finish Clicked:
            onFinishClicked: {
                console.log("Finish Clicked")
                modelCutFinish();
            }

            signal cutModeSelected(int type);
            signal runFeature(int type);
            signal generatePlane();
            signal modelCutFinish();
        }


        //12. PopUp - Shell Offset
        PopUp {
            id: popup_shelloffset
            funcname: "Shell Offset"
            height: 265
            detail1: "Direction"
            detail2: "Offset value"
            //image: ""
            detailline1_vis: true
            detailline2_vis: true
            imageHeight: 40
            applyfinishbutton_vis: true
            okbutton_vis: false
            applybutton_vis: false
            descriptionimage_vis: false
            numbox_detail2_vis: true
            numberbox_detail2_y: 170
            numbox_detail2_defalult: 3.0
            numbox_updown_scale: 0.5
            state: third_tab_button_shelloffset.state=="active" ? "active" : "inactive"
            //switch button
            Rectangle {
                width: 165
                height: 24
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin: 35
                anchors.topMargin: 85
                color: "transparent"
                Rectangle {
                    width: 26
                    height: 24
                    anchors.left: parent.left
                    color: parent.color
                    Image {
                        anchors.fill: parent
                        source: mousearea_shelloffset_switch.containsMouse
                                ? "qrc:/Resource/popup_image/shelloffset_switch_hover.png"
                                : "qrc:/Resource/popup_image/shelloffset_switch.png"
                    }
                    MouseArea {
                        id: mousearea_shelloffset_switch
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }
                Text {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    text: "Switch the direction"
                    font.family: "Arial"
                    font.pointSize: 9
                }
            }
        }

        //13. PopUp - Extend
        PopUp {
            id: popup_extend
            funcname: "Extend"
            height: 200
            detail1: "Extend Value"
            detail2: ""
            //image: ""
            detailline1_vis: true
            detailline2_vis: false
            okbutton_vis: false
            applyfinishbutton_vis: true
            applybutton_vis: false
            descriptionimage_vis: false
            numbox_detail2_vis: true
            numberbox_detail2_y: 90
            numbox_detail2_defalult: 30.0
            numbox_updown_scale: 0.5
            state: fourth_tab_button_extend.state=="active" ? "active" : "inactive"
        }

        //14. PopUp - Support
        /*PopUp {
            id: popup_support
            funcname: "Support"
            height: 300
            detail1: ""
            detail2: ""
            //image: ""
            detailline1_vis: false
            detailline2_vis: false
            applyfinishbutton_vis: false
            okbutton_vis: false
            applybutton_vis: true
            descriptionimage_vis: false
            state: fourth_tab_button_support.state=="active" ? "active" : "inactive"
        }*/
        PopUp {
                        id:popup_Support
                        funcname: "Support"
                        height: 320
                        imageHeight: 34
                        detail1: "Automatic Generation"
                        Rectangle {
                            id: support_autoButton
                            color: parent.color
                            width: 178
                            height: 32
                            radius: 2
                            border.color: "#cccccc"
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            anchors.topMargin: 72
                            Text {
                                id: generateText
                                text: "Generate supports"
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter: parent.verticalCenter
                                font.family: "Arial"
                                font.pointSize: 7
                            }
                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onEntered: {
                                    parent.color = "#f9f9f9"
                                    generateText.color = "#888888"
                                }
                                onExited: {
                                    if (support_autoButton.state == "clicked") {
                                        parent.color = "#f9f9f9"
                                        generateText.color = "#888888"
                                    }
                                    else {
                                        parent.color = "#e5e5e5"
                                        generateText.color = "black"
                                    }
                                }
                                onClicked: {
                                    support_autoButton.state = "clicked"
                                    support_addButton.state = "unclicked"
                                    support_removeButton.state = "unclicked"
                                }
                            }
                            states: [
                                State {
                                    name: "clicked"
                                    PropertyChanges { target: support_autoButton; color: "#f9f9f9" }
                                    PropertyChanges { target: generateText; color: "#888888" }
                                },
                                State {
                                    name: "unclicked"
                                    PropertyChanges { target: support_autoButton; color: "#e5e5e5" }
                                    PropertyChanges { target: generateText; color: "black" }
                                }
                            ]
                        }

                        detail2: "Manual Generation"
                        Rectangle {
                            id: support_addButton
                            color: parent.color
                            width: 178
                            height: 32
                            radius: 2
                            border.color: "#cccccc"
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            anchors.topMargin: 173
                            Text {
                                id: addText
                                text: "Add supports"
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter: parent.verticalCenter
                                font.family: "Arial"
                                font.pointSize: 7
                            }
                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onEntered: {
                                    parent.color = "#f9f9f9"
                                    addText.color = "#888888"
                                }
                                onExited: {
                                    if (support_addButton.state == "clicked") {
                                        parent.color = "#f9f9f9"
                                        addText.color = "#888888"
                                    }
                                    else {
                                        parent.color = "#e5e5e5"
                                        addText.color = "black"
                                    }
                                }
                                onClicked: {
                                    support_addButton.state = "clicked"
                                    support_autoButton.state = "unclicked"
                                    support_removeButton.state = "unclicked"
                                }
                            }
                            states: [
                                State {
                                    name: "clicked"
                                    PropertyChanges { target: support_addButton; color: "#f9f9f9" }
                                    PropertyChanges { target: addText; color: "#888888" }
                                },
                                State {
                                    name: "unclicked"
                                    PropertyChanges { target: support_addButton; color: "#e5e5e5" }
                                    PropertyChanges { target: addText; color: "black" }
                                }
                            ]
                        }

                        Rectangle {
                            id: support_removeButton
                            color: parent.color
                            width: 178
                            height: 32
                            radius: 2
                            border.color: "#cccccc"
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: support_addButton.bottom
                            anchors.topMargin: 20
                            Text {
                                id: removeText
                                text: "Remove supports"
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.verticalCenter: parent.verticalCenter
                                font.family: "Arial"
                                font.pointSize: 7
                            }
                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onEntered: {
                                    parent.color = "#f9f9f9"
                                    removeText.color = "#888888"
                                }
                                onExited: {
                                    if (support_removeButton.state == "clicked") {
                                        parent.color = "#f9f9f9"
                                        removeText.color = "#888888"
                                    }
                                    else {
                                        parent.color = "#e5e5e5"
                                        removeText.color = "black"
                                    }
                                }
                                onClicked:  {
                                    support_removeButton.state = "clicked"
                                    support_addButton.state = "unclicked"
                                    support_autoButton.state = "unclicked"
                                }
                            }
                            states: [
                                State {
                                    name: "clicked"
                                    PropertyChanges { target: support_removeButton; color: "#f9f9f9" }
                                    PropertyChanges { target: removeText; color: "#888888" }

                                },
                                State {
                                    name: "unclicked"
                                    PropertyChanges { target: support_removeButton; color: "#e5e5e5" }
                                    PropertyChanges { target: removeText; color: "black" }
                                }

                            ]
                        }
                        descriptionimage_vis: true
                        detailline1_vis: true
                        detailline2_vis: true
                        okbutton_vis: false
                        applyfinishbutton_vis: false
                        applybutton_vis: true
                    }

        //15. PopUp - Label
        /*PopUp {
            id: popup_label
            funcname: "Label"
            height: 300
            detail1: ""
            detail2: ""
            //image: ""
            detailline1_vis: false
            detailline2_vis: false
            applyfinishbutton_vis: false
            okbutton_vis: false
            applybutton_vis: true
            descriptionimage_vis: false
            state: fourth_tab_button_label.state=="active" ? "active" : "inactive"
        }*/
        PopUp {
            objectName: "labelPopup"
            id:popup_label
            funcname: "Label"
            height: 450 // 282
            imageHeight: 76
            detail1: "Type letters on the surface."
            detail2: "Font"
            image: "qrc:/Resource/label_description.png"

            //signal runFeature(int type);
            signal generateText3DMesh()
            signal openLabelling()
            signal closeLabelling()

            onApplyClicked: {
                console.log("ApplyClicked")
                //runFeature(ftrLabel);
                generateText3DMesh()
            }

            Rectangle {
                id: text3DInputBackground
                width: 176
                height: 20

                y: 300

                color: "#ffffffff"

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 25
                anchors.rightMargin: 25

                Text {
                    id: hiddenText
                    anchors.fill: text3DInput
                    text: text3DInput.text
                    font.pixelSize: text3DInput.font.pixelSize
                    visible: false
                }

                TextField {
                    id:text3DInput
                    objectName: "text3DInput"

                    signal sendTextChanged(string text, int contentWidth)

                    anchors.fill: parent

                    placeholderText: qsTr("Enter text")

                    onTextChanged: {
                        sendTextChanged(text, hiddenText.contentWidth)
                    }
                }
            }

            ComboBox {
                objectName: "labelFontBox"
                id: labelFontBox
                currentIndex: 0
                activeFocusOnPress: true
                width: 176
                anchors.top: parent.top
                anchors.topMargin: 200
                anchors.horizontalCenter: parent.horizontalCenter

                signal sendFontName(string fontName)

                onCurrentTextChanged: sendFontName(currentText)

                Image {
                    width: 12
                    height: 8
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.verticalCenter

                    source: "qrc:/resource/combo_arrow.png"

                }

                style: ComboBoxStyle {
                    background: Rectangle {     //box style (not drop-down part)
//                            implicitWidth: 176
//                            implicitHeight: 24
                        width: parent.width
                        height: parent.height
                        radius: 2
                        color: "#f9f9f9"
                        border.color: labelFontBox.hovered ? "light blue" : "transparent"
                    }

                    label: Text {
                        text: control.currentText
                        font.family: "Arial"
                        font.pointSize: 7
                    }

                    //drop-down customization
                    property Component __dropDownStyle: MenuStyle {
                        __maxPopupHeight: 120
                        __menuItemType: "comboboxitem"

                        frame: Rectangle {      //drop-down box style
                            color: "#f9f9f9"
                            width: 174
                            radius: 2
                        }
                        itemDelegate.label:     //item text
                            Text {
                                text: styleData.text
                                font.family: "Arial"
                                font.pointSize: 7
                                color: styleData.selected ? "#666666" : "#303030"
//                                    color: styleData.selected ? "red" : "blue"
                            }
                        itemDelegate.background: Rectangle {
                            color: styleData.selected ? "#eaeaea" : "#f9f9f9"
                        }

                        //scroller customization
                        __scrollerStyle: ScrollViewStyle {
                            scrollBarBackground: Rectangle {
                                color: "#eaeaea"
                                implicitWidth: 7
                                implicitHeight: 110
                            }
                            handle: Rectangle {
                                color: "#b7b7b7"
                                implicitWidth: 9
                                implicitHeight: 45
                                radius: 2
                            }
                            minimumHandleLength: 35
                            incrementControl: Rectangle {
                                implicitWidth: 0
                                implicitHeight: 0
                            }
                            decrementControl: Rectangle {
                                implicitWidth: 0
                                implicitHeight: 0
                            }
                        }
                    }
                }

                //fonts list
                model: ListModel {
                    id: fontItems
                    ListElement { text: "Arial" }
                    ListElement { text: "굴림체" }
                    ListElement { text: "바탕체" }
                    ListElement { text: "궁서체" }
                    ListElement { text: "Arial5" }
                    ListElement { text: "Arial6" }
                    ListElement { text: "Arial7" }
                    ListElement { text: "Arial8" }
                    ListElement { text: "Arial9" }
                    ListElement { text: "Arial10" }
                }
            }

            descriptionimage_vis: true
            detailline1_vis: false
            detailline2_vis: true
            okbutton_vis:false
            applyfinishbutton_vis: false
            applybutton_vis: true
            state: {
                if (fourth_tab_button_label.state=="active") {
                    openLabelling()
                    return "active"
                }
                else {
                    closeLabelling()
                    return "inactive"
                }
            }
        }
    }
}

