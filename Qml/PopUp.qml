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

    //------------------------------------------------data
    property int numberbox1_number:0
    property int numberbox2_number:0
    property int numberbox3_number:0

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
    }

    function numbox_reset() {
        applyfinishbutton.color = "#BBB"
        numbox_value_x = numbox_default;
        numberbox1_text.text = numbox_value_x;
        numberbox1_number = numbox_value_x;
        numbox_value_y = numbox_default;
        numberbox2_text.text = numbox_value_y;
        numberbox2_number = numbox_value_x;
        numbox_value_z = numbox_default;
        numberbox3_text.text = numbox_value_z;
        numberbox3_number = numbox_value_x;
        numbox_value_detail2 = numbox_detail2_default;
        numberbox_detail2_text.text = numbox_value_detail2;
    }
    function colorApplyFinishButton(mode){
        console.log(mode)
        if (mode === 1){
            applyfinishbutton.color="#3ea6b7"
        }else{
            applyfinishbutton.color="#BBB"
        }

    }

    function destroy_popup(){
        popup_target.state="inactive";
    }



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
        //color: "#999999"
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
        //color: "#999999"
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
            //onEntered: parent.color = "#b5b5b5"
            //onExited: parent.color = "#999999"
            onPressed: parent.color = applybutton_action ? "#3ea6b7" : "#999999"
            onReleased: {applyClicked(); focus_all_off(); numbox_reset(); parent.color = "#999999"}
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
                onEntered: parent.color = "#BCBCBE"
                onExited: parent.color = "#999999"
                onPressed: parent.color = "#3ea6b7"
                onReleased: {destroy_popup();focus_all_off(); numbox_reset();  parent.color = "#999999"}
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
            //onEntered: parent.color = "#BCBCBE"
            //onExited: parent.color = "#999999"
            //onPressed: parent.color = "#3ea6b7"
            onReleased: {do_apply(funcname.text); /*all_off();*/ focus_all_off() ;numbox_reset();parent.color = "#BBB"}
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
            onEntered: parent.color = "#BCBCBE"
            onExited: parent.color = "#999999"
            onPressed: parent.color = "#3ea6b7"
            onReleased: {finishClicked(); all_off(); focus_all_off(); numbox_reset(); parent.color = "#999999"}
        }
    }

    signal flatModeClicked();

    Item {
        id: leftselectimage
        visible: false
        width: 94
        height: 66
        anchors.left: parent.left
        anchors.leftMargin: 17
        anchors.top: parent.top
        anchors.topMargin: 70
        objectName: "flatButton"
        Image {
            source: "qrc:/resource/flat.png"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: flatModeClicked()
        }
    }

    signal curveModeClicked();

    Item {
        id: rightselectimage
        visible: false
        width: 94
        height: 66
        anchors.right: parent.right
        anchors.rightMargin: 17
        anchors.top: parent.top
        anchors.topMargin: 70
        objectName: "curveButton"
        Image {
            anchors.fill: parent
            source: "qrc:/resource/curve.png"
        }
        MouseArea {
            anchors.fill: parent
            onClicked: curveModeClicked();
        }
    }


    ExclusiveGroup {
        id: viewModeGroup
    }

    RadioButton {
        id: radiobutton1
        visible: false
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
    }

    Planeslider {
        id: slider
        visible: false
        anchors.right: parent.left
        anchors.rightMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: -20
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
                    onEntered: {
                    ///    numberbox1.color =  "#f5f5f5"
                    }
                    onExited: {
                    //    numberbox1.color = "#ffffff"
                    }

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
                        if (text.length == 1){
                            text = text.replace(/[^-|0-9]/g,'');
                        }else{
                            text = text.charAt(0).replace(/[^-|1-9]/g,'')+text.substring(1,text.length).replace(/[^0-9]/g, '');
                        }
                    //    numberbox1.color = "#ffffff"
                        if (text === "") text = "0"
                        numberbox1_number = parseInt(text,10)
                        console.log(numberbox1_number);
                        //if (numberbox1_number != 0 ) applyfinishbutton.color = "#3ea6b7"
                        console.log(applyfinishbutton.color);
                        //if (numberbox1_number == 0 && numberbox2_number == 0 && numberbox3_number==0 ) applyfinishbutton.color = "#999999"
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
                    onEntered: {
                    //    numberbox2.color =  "#f5f5f5"
                    }
                    onExited: {
                    //    numberbox2.color = "#ffffff"
                    }

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
                            text = text.replace(/[^-|0-9]/g,'');
                        }else{
                            text = text.charAt(0).replace(/[^-|1-9]/g,'')+text.substring(1,text.length).replace(/[^0-9]/g, '');
                        }
                        if (text === "") text = "0"
                        numberbox2_number = parseInt(text,10)
                     //   if (numberbox2_number != 0 ) applyfinishbutton.color = "#3ea6b7"
                     //   if (numberbox1_number == 0 && numberbox2_number == 0 && numberbox3_number==0 ) applyfinishbutton.color = "#999999"
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
                    onEntered: {
                    //    numberbox3.color =  "#f5f5f5"
                    }
                    onExited: {
                    //    numberbox3.color = "#ffffff"
                    }
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
                            text = text.replace(/[^-|0-9]/g,'');
                        }else{
                            text = text.charAt(0).replace(/[^-|1-9]/g,'')+text.substring(1,text.length).replace(/[^0-9]/g, '');
                        }
                    //    numberbox3.color = "#ffffff"
                        if (text === "") text = "0"
                        numberbox3_number = parseInt(text,10)
                       // if (numberbox3_number != 0 ) applyfinishbutton.color = "#3ea6b7"
                       // if (numberbox1_number == 0 && numberbox2_number == 0 && numberbox3_number==0 ) applyfinishbutton.color = "#999999"
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
            color: "#f5f5f5"
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
                        text = text.replace(/[^-|0-9]/g,'');
                    }else{
                        text = text.charAt(0).replace(/[^-|1-9]/g,'')+text.substring(1,text.length).replace(/[^0-9]/g, '');
                    }
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
                onPressed: {
                    numberbox_detail2_text.focus = false;
                    numbox_value_detail2 = numbox_value_detail2 - numbox_updown_scale;
                    numberbox_detail2_text.text = numbox_value_detail2 + number_unit_detail2;
                    detail2_downbutton_image1.source = "qrc:/Resource/popup_image/down_button_hover.png"
                }
                onReleased: { detail2_downbutton_image1.source = "qrc:/Resource/popup_image/down_button.png"}
            }
        }
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
