import Qt3D.Core 2.0
import QtQuick 2.7
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Rectangle {

    id: popup_target
    visible: false
    width: 244
    //height: 300
    anchors.leftMargin: 20
    anchors.topMargin: 20
    color: "#ffffff"
    //border.width: 1
    //border.color: "#cccccc"

    MouseArea {
        id: popup_area
        anchors.fill: parent
        //hoverEnabled: true
    }

    property alias funcname: funcname.text
    property alias detail1: detail1.text
    property alias detail2: detail2.text
    property alias image: image.source
    property alias detailline1_vis: detailline1.visible
    property alias detailline2_vis: detailline2.visible
    property alias applybutton_vis: applybutton.visible
    property alias okbutton_vis: okbutton.visible
    property alias descriptionimage_vis: descriptionimage.visible
    property alias applyfinishbutton_vis: applyfinishbutton.visible
    property alias applybutton_text: applybutton_text.text
    property alias applyfinishbutton_text:applyfinishbutton_text.text
    property bool applybutton_action:true

    //----------------------------------------------------------------
    // properties for label info
    property string labelTextContent:labelTextContent
    property int labelContentWidth:labelContentWidth
    property string labelFontName:labelFontName
    property bool labelIsBold:labelIsBold
    property int labelFontSize:labelFontSize

    //----------------------------------------------------------------

    property int imageHeight

    //----------------------------------------------------------------

    property alias leftselectimage_vis : leftselectimage.visible
    property alias rightselectimage_vis : rightselectimage.visible
    property alias radiobutton1_vis : radiobutton1.visible
    property alias radiobutton2_vis : radiobutton2.visible
    property alias slider_vis : slider.visible

    //----------------------------------------------------------------

    property alias numberbox_vis: numberbox.visible
    property alias numberbox_nameing_vis: numberbox_naming.visible
    property alias numberboxset2_vis: numberboxset2.visible
    property alias numberboxset3_vis: numberboxset3.visible
    property int numberbox_width
    property int numberbox_y
    property real numbox_default : 0
    property real numbox_detail2_default : 0
    property bool numbox_updown_vis: true
    property alias numbox_total_height: numberbox.height
    property alias numbox_detail2_vis: numberbox_detail2.visible
    property int numberbox_detail2_y
    property real numbox_updown_scale
    property alias text3DInputBackground_vis : text3DInputBackground.visible

    //------------------------------------------------data
    property real numberbox1_number:0
    property real numberbox2_number:0
    property real numberbox3_number:0
    property real numberbox1_number_origin:0
    property real numberbox2_number_origin:0
    property real numberbox3_number_origin:0
    property alias numberbox1_text: numberbox1_text
    property alias numberbox2_text: numberbox2_text
    property alias numberbox3_text: numberbox3_text
    property alias slider_value : slider.value


    /*
    function possible_calculate(unit,lastvalue,input){
        //unit = String(unit);
        //input_unit = lastvalue.substring(2)
        var output;
        var inputLength = input.length;
            var input_unit = input.substring(inputLength - unit.length, inputLength);
            var input_number = input.substring(0, inputLength - unit.length);
            console.log("input unit is " + input_unit);
            console.log("input number is " + input_number);
            if (input_number.replace(/\D/g, '').length === input_number.length){
                if(input_number.length <= 5){
                    output = input_number + input_unit;
                }
                else {
                    output = lastvalue;
                }
            }
            else {
                output = lastvalue;
            }
        return output;
    }
    */

    property string number_unit : "mm"
    property string number_unit_detail2 : "mm"
    property real numbox_value_x : numbox_default
    property real numbox_value_y : numbox_default
    property real numbox_value_z : numbox_default
    property real numbox_value_detail2 : numbox_detail2_default

    signal applyClicked()
    signal finishClicked()

    function do_apply(functionname){
        switch(functionname){
                case "Save":
                    applyClicked();
                    break;
                case "Move":
                    applyClicked();
                    break;
                case "Rotate":
                    applyClicked();
                    break;
                case "Lay flat":
                    applyClicked();
                    break;
                case "Arrange":
                    applyClicked();
                    break;
                case "Orient":
                    applyClicked();
                    //popup_orient.autoOrientSignal();
                    break;
                case "Scale":
                    applyClicked();
                    break;
                case "Auto Repair":
                    applyClicked();
                    break;
                case "Cut":
                    applyClicked();
                    break;
                case "Shell Offset":
                    applyClicked();
                    break;
                case "Extend":
                    applyClicked();
                    break;
                case "Support":
                    break;
                case "Label":
                    applyClicked();
                    break;
                default:
                    console.log(functionname);
                    break;
                }
    }

    function focus_all_off() {
        numberbox1_text.focus = false;
        numberbox2_text.focus = false;
        numberbox3_text.focus = false;
        numberbox_detail2_text.focus = false;
        text3DInput.focus = false;
        qm.keyboardHandlerFocus();
    }

    function numbox_reset() {
        applyfinishbutton.color = "#BBB"
        numbox_value_x = numbox_default;
        numberbox1_text.text = numberbox1_number_origin;
        numberbox1_number = numberbox1_number_origin;
        numbox_value_y = numbox_default;
        numberbox2_text.text = numberbox2_number_origin;
        numberbox2_number = numberbox2_number_origin;
        numbox_value_z = numbox_default;
        numberbox3_text.text = numberbox3_number_origin;
        numberbox3_number = numberbox3_number_origin;
        numbox_value_detail2 = numbox_detail2_default;
        numberbox_detail2_text.text = numbox_value_detail2;
    }

    function cut_reset() {
        leftselectimage.image_source = "qrc:/Resource/flat_cut.png"
        leftselectimage.text_color = "black"
        leftselectimage.color = "#FFFFFF"
        leftselectimage.border.color =  "#CCCCCC"

        rightselectimage.image_source = "qrc:/Resource/free_cut.png"
        rightselectimage.text_color ="black"
        rightselectimage.color = "#FFFFFF"
        rightselectimage.border.color =  "#CCCCCC"

        //flatModeClicked()
        radiobutton1.checked = true;
    }

    function colorApplyFinishButton(mode){
        //console.log("colorApplyFinishButton mode " + mode)
        if (mode === 1){
            applyfinishbutton.color="#3ea6b7"
        }else if(mode === 0){
            applyfinishbutton.color="#BBB"
        }else{
            applyfinishbutton.color="#999"
        }

    }

    function destroy_popup(){
        popup_target.state="inactive";
    }

    /*
    function isApplyClickable(){

        if(okbutton_vis){

        }else if(applybutton_vis){

        }else if(applyfinishbutton_vis){
    }*/

    states: [
        State{
            name:"inactive"; //when: parent.state=="inactive"
            PropertyChanges { target: popup_target;  visible: false }
        },
        State{
            name:"active"; //when: parent.state=="active"
            PropertyChanges { target: popup_target;  visible: true }
        }
    ]

    //function name
    Rectangle{
        width: parent.width
        height: 32
        anchors.top: parent.top
        anchors.left: parent.left
        color: "#0DA3B2"

    }


    Text {
        id: funcname
        anchors.top: parent.top
        anchors.topMargin: 4
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.pixelSize: 19
        color: "#ffffff"
        font.family: mediumFont.name
        verticalAlignment: Text.AlignTop
    }

    //detail name1
    Text {
        id: detail1
        anchors.top: funcname.bottom
        anchors.topMargin: 15
        anchors.left: funcname.left
        anchors.leftMargin: 10
        font.pixelSize: 15
        color: "#3EA6B7"
        font.family: mainFont.name
    }

    //detail name right side line1
    Rectangle {
        id: detailline1
        width: 195 - detail1.implicitWidth
        height: 1
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.verticalCenter: detail1.verticalCenter
        color: "#b2b2b2"
    }

    //detail name2
    Text {
        id: detail2
        anchors.top: descriptionimage.bottom
        anchors.topMargin: 15
        anchors.left: funcname.left
        anchors.leftMargin: 10
        font.pixelSize: 15
        color: "#3EA6B7"

        font.family: mainFont.name
    }

    //detail name right side line2
    Rectangle {
        id: detailline2
        width: 195 - detail2.implicitWidth
        height: 1
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.verticalCenter: detail2.verticalCenter
        color: "#b2b2b2"
    }

    //Apply button
    Rectangle {
        id: applybutton
        width: 230
        height: 30.7
        color: "#999999"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 7
        anchors.horizontalCenter: parent.horizontalCenter

        Text {
            id: applybutton_text
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Apply"
            color: "#ffffff"
            font.pixelSize: 16
            font.family: mainFont.name
        }
        MouseArea {
            id: mousearea_apply
            anchors.fill: parent
            hoverEnabled: true
            onEntered : {
                if(parent.color.toString().indexOf("#bbb") < 0)
                    parent.color = "#3ea6b7"
                qm.setHandCursor();
            }
            onExited : {
                parent.color = "#999999"
                qm.resetCursor();
            }
            onReleased: {applyClicked(); focus_all_off(); numbox_reset();}
        }
    }

    //OK button
    Rectangle {
        id: okbutton
        width: 90
        height: 20
        color: "#999999"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 15
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
            id: okbutton_text
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text: "OK"
            color: "#ffffff"
            font.pixelSize: 16
            font.family: mainFont.name
        }
        MouseArea {
            id: mousearea_ok
            anchors.fill: parent
            hoverEnabled: true
            onEntered:{
                qm.setHandCursor();
                if(parent.color.toString().indexOf("#bbb") < 0)
                    parent.color = "#3ea6b7"
            }
            onExited:{
                qm.resetCursor();
                parent.color = "#999999"
            }
            onReleased: {destroy_popup();focus_all_off(); numbox_reset();}
        }
    }

    //Description image
    Rectangle {
        id: descriptionimage
        width: 176.6
        height: imageHeight
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: detail1.bottom
        anchors.topMargin: 20
        color: parent.color
        //color: "#ff0000"
        Image {
            id: image
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    //Apply-Finish button
    Rectangle {
        id: applyfinishbutton
        width: 111.6
        height: 30.7
        color: "#BBB"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 7
        anchors.right: parent.horizontalCenter
        anchors.rightMargin: 3.5
        property color beforeHorver : "#BBBBBB";
        Text {
            id: applyfinishbutton_text
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Apply"
            color: "#ffffff"
            font.pixelSize: 16
            font.family: mainFont.name
        }
        MouseArea {
            id: mousearea_applyfinish_apply
            anchors.fill: parent
            hoverEnabled: true
            onEntered : {
                qm.setHandCursor();
                parent.beforeHorver = parent.color
                if(parent.color.toString().indexOf("#bbb") < 0)
                    parent.color = "#3ea6b7"
            }
            onExited : {
                qm.resetCursor();
                parent.color = parent.beforeHorver
            }
            onReleased: {do_apply(funcname.text); /*all_off();*/ focus_all_off() ;numbox_reset();}
        }
    }
    Rectangle {
        visible: applyfinishbutton.visible
        width: 111.6
        height: 30.7
        color: "#999999"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 7
        anchors.left: parent.horizontalCenter
        anchors.leftMargin: 3.5
        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Finish"
            color: "#ffffff"
            font.pixelSize: 16
            font.family: mainFont.name
        }
        MouseArea {
            id: mousearea_applyfinish_finish
            anchors.fill: parent
            hoverEnabled: true
            onEntered:{
                qm.setHandCursor();
                if(parent.color.toString().indexOf("#bbb") < 0)
                    parent.color = "#3ea6b7"
            }
            onExited:{
                qm.resetCursor();
                parent.color = "#999999"
            }
            onReleased: {finishClicked(); all_off(); focus_all_off(); numbox_reset();}
        }
    }

    signal flatModeClicked();

    Rectangle {
        id: leftselectimage
        visible: false
        width: 92
        height: 64
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 80
        objectName: "flatButton"

        color: "white"
        border.width: 1
        border.color: "#CCCCCC"
        radius: 1

        property string image_source : "qrc:/Resource/flat_cut.png"
        property color text_color : "#000000"

        Image {
            width: 36
            height: 36
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 8
            source: leftselectimage.image_source
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled : true
            onEntered : {
                if(leftselectimage.border.color === "CCCCCC")
                    leftselectimage.color = "#E3E3E5"
                qm.setHandCursor();
            }
            onExited : {
                if(leftselectimage.border.color === "CCCCCC")
                    leftselectimage.color = "#FFFFFF"
                qm.resetCursor();
            }
            onClicked: {
                leftselectimage.image_source = "qrc:/Resource/flat_cut_select.png"
                leftselectimage.text_color = "white"
                leftselectimage.color = "#3EABBA"
                leftselectimage.border.color =  "#3EABBA"

                rightselectimage.image_source = "qrc:/Resource/free_cut.png"
                rightselectimage.text_color ="black"
                rightselectimage.color = "#FFFFFF"
                rightselectimage.border.color =  "#CCCCCC"

                flatModeClicked()
            }
        }
        Text {
            text: "Flat Cut"
            font.family: mainFont.name
            color: leftselectimage.text_color
            font.pixelSize: 13
            anchors.bottom:  parent.bottom
            anchors.bottomMargin: 4
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    signal curveModeClicked();

    Rectangle {
        id: rightselectimage
        visible: false
        width: 92
        height: 64
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 80
        objectName: "curveButton"

        color: "white"
        border.width: 1
        border.color: "#CCCCCC"
        radius: 1

        property string image_source : "qrc:/Resource/free_cut.png"
        property color text_color : "#000000"

        Image {
            width: 36
            height: 36
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 8

            source: rightselectimage.image_source
        }
        MouseArea {
            anchors.fill: parent
            hoverEnabled : true
            onEntered : {
                if(rightselectimage.border.color === "CCCCCC")
                    rightselectimage.color = "#E3E3E5"
                qm.setHandCursor();
            }
            onExited : {
                if(rightselectimage.border.color === "CCCCCC")
                    rightselectimage.color = "#FFFFFF"
                qm.resetCursor();
            }
            onClicked: {
                rightselectimage.image_source = "qrc:/Resource/free_cut_select.png"
                rightselectimage.text_color ="white"
                rightselectimage.color = "#3EABBA"
                rightselectimage.border.color =  "#3EABBA"

                leftselectimage.image_source = "qrc:/Resource/flat_cut.png"
                leftselectimage.text_color ="black"
                leftselectimage.color = "#FFFFFF"
                leftselectimage.border.color =  "#CCCCCC"

                curveModeClicked();
            }
        }
        Text {
            text: "Free Cut"
            font.family: mainFont.name
            color: rightselectimage.text_color
            font.pixelSize: 13
            anchors.bottom:  parent.bottom
            anchors.bottomMargin: 4
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }


    ExclusiveGroup {
        id: viewModeGroup
    }

    signal radioClicked(int value);

    RadioButton {
        id: radiobutton1
        visible: false
        checked: true
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 90
        anchors.left: parent.left
        anchors.leftMargin: 30
        exclusiveGroup: viewModeGroup
        style: RadioButtonStyle {
            indicator: Rectangle {
                implicitWidth: 16
                implicitHeight: 16
                radius: 9
                color : "#E2E2E2"
                border.color: control.activeFocus ? "darkblue" : "gray"
                border.width: 0
                Rectangle {
                    anchors.fill: parent
                    visible: control.checked
                    color: "#505A5E"
                    radius: 9
                    anchors.margins: 4
                }
            }
            label: Text {
                text: "None"
                anchors.left: parent.left
                anchors.leftMargin: 10
                font.family: mainFont.name
                font.pixelSize: 14
            }
        }
        onClicked: {
            radioClicked(1);
        }
        onHoveredChanged: {
            if(hovered)
                qm.setHandCursor();
            else
                qm.resetCursor();
        }
    }

    RadioButton {
        id: radiobutton2
        visible: false
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 65
        anchors.left: parent.left
        anchors.leftMargin: 30
        exclusiveGroup: viewModeGroup
        style: RadioButtonStyle {
            indicator: Rectangle {
                implicitWidth: 16
                implicitHeight: 16
                radius: 9
                color : "#E2E2E2"
                border.color: control.activeFocus ? "darkblue" : "gray"
                border.width: 0
                Rectangle {
                    anchors.fill: parent
                    visible: control.checked
                    color: "#505A5E"
                    radius: 9
                    anchors.margins: 4
                }
            }
            label: Text {
                text: "Gernerate Surface"
                anchors.left: parent.left
                anchors.leftMargin: 10
                font.family: mainFont.name
                font.pixelSize: 14
            }
        }
        onClicked: {
            radioClicked(2);
        }
        onHoveredChanged: {
            if(hovered)
                qm.setHandCursor();
            else
                qm.resetCursor();
        }
    }

    signal planeSliderValueChanged(double value);
    Planeslider {
        id: slider
        visible: false
        anchors.right: parent.right
        anchors.rightMargin: -60
        anchors.top: parent.top
        anchors.topMargin: 0

        onSliderValueChanged: {
            planeSliderValueChanged(value);
        }
    }

    //Number write down button
    Item{
        id: numberbox
        visible: false
        width: 165
        height: 142
        anchors.horizontalCenter: parent.horizontalCenter
        y: numberbox_y

        //number box 1
        Item {
            id: numberboxset1
            width: 140
            height: 32
            anchors.top: parent.top
            anchors.right: parent.right
            //number box
            Rectangle {
                id: numberbox1
                width: numberbox_width
                height: 24
                color: "#ffffff"
                anchors.verticalCenter: parent.verticalCenter
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered : qm.setHandCursor();
                    onExited : qm.resetCursor();
                    /*
                    onEntered: {
                    ///    numberbox1.color =  "#f5f5f5"
                    }
                    onExited: {
                    //    numberbox1.color = "#ffffff"
                    }*/

                }
                TextField {
                    id: numberbox1_text
                    anchors.right: parent.right
                    anchors.rightMargin: 8*number_unit.length
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: TextInput.AlignRight
                    //text: numbox_value_z + number_unit
                    placeholderText: numbox_value_x
                    font.pixelSize: 12
                    font.family: mainFont.name
                    textColor: focus ? "black" : "#595959"
                    selectByMouse: true
                    onFocusChanged: {
                    //    if(numberbox1_text.focus)
                    //        numberbox1.color = "#f5f5f5"
                    }
                    onTextChanged: {
                        console.log("on text changed called" + text);
                        if (text.length == 1){
                            text = text.replace(/[^-|0-9.]/g,'');
                        }else{
                            text = text.charAt(0).replace(/[^-|1-9.]/g,'')+text.substring(1,text.length).replace(/[^0-9.]/g, '');
                        }

                        console.log("on text changed ok 1 ")

                        if (text.length > 6){
                            text = text.substring(0,6)
                        }

                        if (text === "") text = "0"
                        numberbox1_number = parseFloat(text)

                        if ((numberbox1_text.text === "0" || numberbox1_text.text === "") && (numberbox2_text.text === "0" || numberbox2_text.text === "") && (numberbox3_text.text === "0" || numberbox3_text.text === "" || !numberboxset3.visible)){
                            colorApplyFinishButton(0)
                        }
                        else{
                            colorApplyFinishButton(2)
                        }
                    }
                    style: TextFieldStyle {
                        textColor: "black"
                        background: Rectangle {
                            border.width: 0
                        }
                    }
                }
                Text{
                    font.pixelSize: 12
                    font.family: mainFont.name
                    text:number_unit
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#111"
                }

                Rectangle{
                    width: parent.width
                    height: 1
                    anchors.left: parent.left
                    anchors.top: parent.bottom
                    color: "#3f3f3f"
                }
            }


            //up-button
            Rectangle {
                visible: numbox_updown_vis
                width:18
                height: 15
                anchors.right: parent.right
                anchors.top: parent.top
                color: "transparent"
                Image {
                    id: upbutton_image1
                    anchors.fill: parent
                    source: "qrc:/Resource/popup_image/up_button.png"
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered : qm.setHandCursor();
                    onExited : qm.resetCursor();
                    onPressed: {
                        numberbox1_text.focus = false;
                        numbox_value_x = numbox_value_x + numbox_updown_scale;
                        numberbox1_text.text = numbox_value_x + number_unit;
                        upbutton_image1.source = "qrc:/Resource/popup_image/up_button_hover.png"
                        console.log(numberbox1_text.text);
                    }
                    onReleased: { upbutton_image1.source = "qrc:/Resource/popup_image/up_button.png"}
                }
            }
            //down-button
            Rectangle {
                visible: numbox_updown_vis
                width: 18
                height: 15
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                color: "transparent"
                Image {
                    id: downbutton_image1
                    anchors.fill: parent
                    source: "qrc:/Resource/popup_image/down_button.png"
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered : qm.setHandCursor();
                    onExited : qm.resetCursor();
                    onPressed: {
                        numberbox1_text.focus = false;
                        numbox_value_x = numbox_value_x - numbox_updown_scale;
                        numberbox1_text.text = numbox_value_x + number_unit;
                        downbutton_image1.source = "qrc:/Resource/popup_image/down_button_hover.png"
                        console.log(numberbox1_text.text);
                    }
                    onReleased: { downbutton_image1.source = "qrc:/Resource/popup_image/down_button.png"}
                }
            }
        }

        //number box 2
        Item {
            id: numberboxset2
            width: 140
            height: 32
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            //number box
            Rectangle {
                id: numberbox2
                //width: 110
                width: numberbox_width
                height: 24
                color: "#ffffff"
                anchors.verticalCenter: parent.verticalCenter
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered : qm.setHandCursor();
                    onExited : qm.resetCursor();
                    /*
                    onEntered: {
                    //    numberbox2.color =  "#f5f5f5"
                    }
                    onExited: {
                    //    numberbox2.color = "#ffffff"
                    }*/

                }
                TextField {
                    id: numberbox2_text
                    anchors.right: parent.right
                    anchors.rightMargin: 8*number_unit.length
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: TextInput.AlignRight
                    //text: numbox_value_z + number_unit
                    placeholderText: numbox_value_y
                    font.pixelSize: 12
                    font.family: mainFont.name
                    textColor: focus ? "black" : "#595959"
                    selectByMouse: true
                    onFocusChanged: {
                    }
                    onTextChanged: {
                        if (text.length == 1){
                            text = text.replace(/[^-|0-9.]/g,'');
                        }else{
                            text = text.charAt(0).replace(/[^-|1-9.]/g,'')+text.substring(1,text.length).replace(/[^0-9.]/g, '');
                        }

                        if (text.length > 6){
                            text = text.substring(0,6)
                        }

                        if (text === "") text = "0";
                        numberbox2_number = parseFloat(text)

                        if ((numberbox1_text.text === "0" || numberbox1_text.text === "") && (numberbox2_text.text === "0" || numberbox2_text.text === "") && (numberbox3_text.text === "0" || numberbox3_text.text === "" || !numberboxset3.visible)){
                            colorApplyFinishButton(0)
                        }
                        else{
                            colorApplyFinishButton(2)
                        }
                    }
                    style: TextFieldStyle {
                        textColor: "black"
                        background: Rectangle {
                            border.width: 0
                        }
                    }
                }
                Text{
                    font.pixelSize: 12
                    font.family: mainFont.name
                    text:number_unit
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#111"
                }
                Rectangle{
                    width: parent.width
                    height: 1

                    anchors.left: parent.left
                    anchors.top: parent.bottom
                    color: "#3f3f3f"
                }

            }
            //up-button
            Rectangle {
                visible: numbox_updown_vis
                width:18
                height: 15
                anchors.right: parent.right
                anchors.top: parent.top
                color: "transparent"
                Image {
                    id: upbutton_image2
                    anchors.fill: parent
                    source:"qrc:/Resource/popup_image/up_button.png"
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered : qm.setHandCursor();
                    onExited : qm.resetCursor();
                    onPressed: {
                        numberbox2_text.focus = false;
                        numbox_value_y = numbox_value_y + numbox_updown_scale;
                        numberbox2_text.text = numbox_value_y + number_unit;
                        upbutton_image2.source = "qrc:/Resource/popup_image/up_button_hover.png"
                    }
                    onReleased: { upbutton_image2.source = "qrc:/Resource/popup_image/up_button.png"}
                }
            }
            //down-button
            Rectangle {
                visible: numbox_updown_vis
                width: 18
                height: 15
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                color: "transparent"
                Image {
                    id: downbutton_image2
                    anchors.fill: parent
                    source: "qrc:/Resource/popup_image/down_button.png"
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered : qm.setHandCursor();
                    onExited : qm.resetCursor();
                    onPressed: {
                        numberbox2_text.focus = false;
                        numbox_value_y = numbox_value_y - numbox_updown_scale;
                        numberbox2_text.text = numbox_value_y + number_unit;
                        downbutton_image2.source = "qrc:/Resource/popup_image/down_button_hover.png"
                    }
                    onReleased: { downbutton_image2.source = "qrc:/Resource/popup_image/down_button.png"}
                }
            }
        }

        //number box 3
        Item {
            id: numberboxset3
            width: 140
            height: 32
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            //number box
            Rectangle {
                id: numberbox3
                //width: 110
                width: numberbox_width
                height: 24
                color: "#ffffff"
                anchors.verticalCenter: parent.verticalCenter
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered : qm.setHandCursor();
                    onExited : qm.resetCursor();
                    /*
                    onEntered: {
                    //    numberbox3.color =  "#f5f5f5"
                    }
                    onExited: {
                    //    numberbox3.color = "#ffffff"
                    }*/
                }
                TextField {
                    id: numberbox3_text
                    anchors.right: parent.right
                    anchors.rightMargin: 8*number_unit.length
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: TextInput.AlignRight
                    //text: numbox_value_z + number_unit
                    placeholderText: numbox_value_z
                    font.pixelSize: 12
                    font.family: mainFont.name
                    textColor: focus ? "black" : "#595959"
                    selectByMouse: true
                    onFocusChanged: {
                    //    if(numberbox3_text.focus)
                    //        numberbox3.color = "#f5f5f5"
                    }
                    onTextChanged: {
                        if (text.length == 1){
                            text = text.replace(/[^-|0-9.]/g,'');
                        }else{
                            text = text.charAt(0).replace(/[^-|1-9.]/g,'')+text.substring(1,text.length).replace(/[^0-9.]/g, '');
                        }
                        if (text.length > 6){
                            text = text.substring(0,6)
                        }

                        if (text === "") text = "0";
                        numberbox3_number = parseFloat(text)

                        if ((numberbox1_text.text === "0" || numberbox1_text.text === "") && (numberbox2_text.text === "0" || numberbox2_text.text === "") && (numberbox3_text.text === "0" || numberbox3_text.text === "" || !numberboxset3.visible)){
                            colorApplyFinishButton(0)
                        }
                        else{
                            colorApplyFinishButton(2)
                        }
                    }
                    style: TextFieldStyle {
                        textColor: "black"
                        background: Rectangle {
                            border.width: 0
                        }
                    }
                }
                Text{
                    font.pixelSize: 12
                    font.family: mainFont.name
                    text:number_unit
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#111"
                }
                Rectangle{
                    width: parent.width
                    height: 1

                    anchors.left: parent.left
                    anchors.top: parent.bottom
                    color: "#3f3f3f"
                }

            }
            //up-button
            Rectangle {
                visible: numbox_updown_vis
                width:18
                height: 15
                anchors.right: parent.right
                anchors.top: parent.top
                color: "transparent"
                Image {
                    id: upbutton_image3
                    anchors.fill: parent
                    source: "qrc:/Resource/popup_image/up_button.png"
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered : qm.setHandCursor();
                    onExited : qm.resetCursor();
                    onPressed: {
                        numberbox3_text.focus = false;
                        numbox_value_z = numbox_value_z + numbox_updown_scale;
                        numberbox3_text.text = numbox_value_z + number_unit;
                        upbutton_image3.source = "qrc:/Resource/popup_image/up_button_hover.png"
                    }
                    onReleased: { upbutton_image3.source = "qrc:/Resource/popup_image/up_button.png"}
                }
            }
            //down-button
            Rectangle {
                visible: numbox_updown_vis
                width: 17
                height: 14.5
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                color: "transparent"
                Image {
                    id: downbutton_image3
                    anchors.fill: parent
                    source: "qrc:/Resource/popup_image/down_button.png"
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered : qm.setHandCursor();
                    onExited : qm.resetCursor();
                    onPressed: {
                        numberbox3_text.focus = false;
                        numbox_value_z = numbox_value_z - numbox_updown_scale;
                        numberbox3_text.text = numbox_value_z + number_unit;
                        downbutton_image3.source = "qrc:/Resource/popup_image/down_button_hover.png"
                    }
                    onReleased: { downbutton_image3.source = "qrc:/Resource/popup_image/down_button.png"}
                }
            }
        }

        Item {
            id: numberbox_naming
            visible: false
            height: parent.height
            Rectangle {
                width: 15
                height: 32
                anchors.top: parent.top
                anchors.left: parent.left
                color: "transparent"
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    text: "X :"
                    font.pixelSize: 12
                    font.family: mainFont.name
                }
            }
            Rectangle {
                width: 15
                height: 32
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                color: "transparent"
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    text: "Y :"
                    font.pixelSize: 12
                    font.family: mainFont.name
                }
            }
            Rectangle {
                width: 15
                height: 32
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                color: "transparent"
                visible: numberboxset3_vis
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    text: "Z :"
                    font.pixelSize: 12
                    font.family: mainFont.name
                }
            }
        }
    }


    //detail2 - number button
    Item {
        id: numberbox_detail2
        visible: false
        width: 190
        height: 32
        //y: 240
        y: numberbox_detail2_y
        anchors.left: parent.left
        anchors.leftMargin: 25
        //number box
        Rectangle {
            width: 158
            height: 24
            color: "transparent"
            anchors.verticalCenter: parent.verticalCenter

            TextField {
                id: numberbox_detail2_text
                anchors.right: parent.right
                anchors.rightMargin: 8*number_unit_detail2.length
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: TextInput.AlignRight
                placeholderText: numbox_value_detail2
                font.pixelSize: 12
                font.family: mainFont.name
                textColor: focus ? "black" : "#595959"
                selectByMouse: true
                onTextChanged: {
                    if (text.length == 1){
                        text = text.replace(/[^0-9.]/g,'');
                    }else{
                        text = text.charAt(0).replace(/[^1-9.]/g,'')+text.substring(1,text.length).replace(/[^0-9.]/g, '');
                    }

                    /*if (parseFloat(text) > 100){
                        text = text.slice(0,-1)
                    }*/

                    numbox_value_detail2 = parseFloat(text)
                }
                style: TextFieldStyle {
                    textColor: "black"
                    background: Rectangle {
                        border.width: 0
                    }
                }
            }
            Text{
                font.pixelSize: 12
                font.family: mainFont.name
                text:number_unit_detail2
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                color: "#111"
            }
            Rectangle{
                width: parent.width
                height: 1
                color: "#3f3f3f"
                anchors.top : numberbox_detail2_text.bottom
                anchors.right: parent.right
                anchors.rightMargin: 0
            }
        }
        //up-button
        Rectangle {
            width:17
            height: 14.5
            anchors.right: parent.right
            anchors.top: parent.top
            color: "transparent"
            Image {
                id: detail2_upbutton_image1
                anchors.fill: parent
                source: "qrc:/Resource/popup_image/up_button.png"
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered : qm.setHandCursor();
                onExited : qm.resetCursor();
                onPressed: {
                    numberbox_detail2_text.focus = false;
                    numbox_value_detail2 = numbox_value_detail2 + numbox_updown_scale;
                    numberbox_detail2_text.text = numbox_value_detail2 + number_unit_detail2;
                    detail2_upbutton_image1.source = "qrc:/Resource/popup_image/up_button_hover.png"
                }
                onReleased: { detail2_upbutton_image1.source = "qrc:/Resource/popup_image/up_button.png"}
            }
        }
        //down-button
        Rectangle {
            width: 17
            height: 14.5
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            color: "transparent"
            Image {
                id: detail2_downbutton_image1
                anchors.fill: parent
                source: "qrc:/Resource/popup_image/down_button.png"
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered : qm.setHandCursor();
                onExited : qm.resetCursor();
                onPressed: {
                    numberbox_detail2_text.focus = false;
                    if (numbox_value_detail2 - numbox_updown_scale)
                        numbox_value_detail2 = numbox_value_detail2 - numbox_updown_scale;
                    numberbox_detail2_text.text = numbox_value_detail2 + number_unit_detail2;
                    detail2_downbutton_image1.source = "qrc:/Resource/popup_image/down_button_hover.png"
                }
                onReleased: { detail2_downbutton_image1.source = "qrc:/Resource/popup_image/down_button.png"}
            }
        }
    }

    signal labelTextChanged(string text, int contentWidth)

    // text3d input
    Rectangle {
        id: text3DInputBackground
        width: 194
        height: 24
        visible: false

        y: 320

        color: "#ffffffff"

        //anchors.left: parent.left
        //anchors.right : parent.right
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        //anchors.leftMargin: 25
        //anchors.rightMargin: 25
        anchors.bottomMargin: 55

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
            anchors.fill: parent
//            activeFocusOnPress: true
 //           activeFocusOnPress: false
            style: TextFieldStyle {
                textColor: "#333333"
                background: Rectangle {
                    radius: 2
                    border.color: "#CCC"
                    border.width: 1
                }
            }
            selectByMouse: true

            onFocusChanged: {
                console.log("focus changed: ", text3DInput.focus, text3DInputBackground.focus);
            //    if(numberbox1_text.focus)
            //        numberbox1.color = "#f5f5f5"
            }
            placeholderText: qsTr("Enter text")
            font.family: mainFont.name
            onTextChanged: {
                console.log("\n\ncontent width changed ");
                //console.log(hiddenText.text.length);
                //console.log(hiddenText.text);
                //console.log(hiddenText.contentWidth);
                labelTextChanged(text, hiddenText.contentWidth);

                //sendTextChanged(text, hiddenText.text.length)
            }



        }

        /*
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            focus: false
            onEntered: {
                text3DInput.focus = true;
                text3DInput.activeFocusOnPress = true;
                console.log("entered ^^^^", text3DInput.activeFocusOnPress);
            }
            onExited: {
                text3DInput.focus = false;
                text3DInput.activeFocusOnPress = false;
                console.log("exited ^^^^", text3DInput.activeFocusOnPress);
            }
        }
        */
    }

    Rectangle{//shadow
        width: parent.width
        height: 2
        anchors.left: parent.left
        anchors.top : parent.bottom
        anchors.leftMargin: 2
        color: "#CCCCCC"
    }
    Rectangle{//shadow
        width: 2
        height: parent.height
        anchors.left: parent.right
        anchors.top : parent.top
        anchors.topMargin: 2
        color: "#CCCCCC"
    }
}
