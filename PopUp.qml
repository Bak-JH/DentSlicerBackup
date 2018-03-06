import Qt3D.Core 2.0
import QtQuick 2.7
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Rectangle {
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
    property real numbox_detail2_defalult : 0
    property bool numbox_updown_vis: true
    property alias numbox_total_height: numberbox.height
    property alias numbox_detail2_vis: numberbox_detail2.visible
    property int numberbox_detail2_y
    property real numbox_updown_scale

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
    property real numbox_value_detail2 : numbox_detail2_defalult

    signal applyClicked()

    function do_apply(functionname){
        switch(functionname.text){
                case "Move":
                    break;
                case "Rotate":
                    break;
                case "Lay flat":
                    break;
                case "Arrange":
                    break;
                case "Orient":
                    popup_orient.autoOrientSignal();
                    break;
                case "Scale":
                    break;
                case "Auto Repair":
                    break;
                case "Cut":
                    break;
                case "Shell Offset":
                    break;
                case "Extend":
                    break;
                case "Support":
                    break;
                case "Label":
                    applyClicked();
                    break;
                case "Move":
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
        numbox_value_x = numbox_default;
        numberbox1_text.text = numbox_value_x + number_unit;
        numbox_value_y = numbox_default;
        numberbox2_text.text = numbox_value_y + number_unit;
        numbox_value_z = numbox_default;
        numberbox3_text.text = numbox_value_z + number_unit;
        numbox_value_detail2 = numbox_detail2_defalult;
        numberbox_detail2_text.text = numbox_value_detail2 + number_unit;
    }

    function destroy_popup(){
        popup_target.state="inactive";
    }

    id: popup_target
    visible: false
    width: 244
    //height: 300
    anchors.leftMargin: 20
    anchors.topMargin: 20
    color: "#e5e5e5"
    border.width: 1
    border.color: "#cccccc"

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
    Text {
        id: funcname
        anchors.top: parent.top
        anchors.topMargin: 8
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.pointSize: 10.5
        color: "#494949"
        font.family: "Arial"
    }

    //detail name1
    Text {
        id: detail1
        anchors.top: funcname.bottom
        anchors.topMargin: 15
        anchors.left: funcname.left
        anchors.leftMargin: 10
        font.pointSize: 9
        color: "#999999"
        font.family: "Arial"
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
        font.pointSize: 9
        color: "#999999"
        font.family: "Arial"
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
            font.pointSize: 10
            font.family: "Arial"
        }
        MouseArea {
            id: mousearea_apply
            anchors.fill: parent
            hoverEnabled: true
            onEntered: parent.color = "#b5b5b5"
            onExited: parent.color = "#999999"
            onPressed: parent.color = "#3ea6b7"
            onReleased: {do_apply(funcname.text);all_off(); focus_all_off(); numbox_reset(); parent.color = "#999999"}
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
                font.pointSize: 8
                font.family: "Arial"
            }
            MouseArea {
                id: mousearea_ok
                anchors.fill: parent
                hoverEnabled: true
                onEntered: parent.color = "#b5b5b5"
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
        color: "#999999"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 7
        anchors.right: parent.horizontalCenter
        anchors.rightMargin: 3.5
        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Apply"
            color: "#ffffff"
            font.pointSize: 10
            font.family: "Arial"
        }
        MouseArea {
            id: mousearea_applyfinish_apply
            anchors.fill: parent
            hoverEnabled: true
            onEntered: parent.color = "#b5b5b5"
            onExited: parent.color = "#999999"
            onPressed: parent.color = "#3ea6b7"
            onReleased: {all_off(); focus_all_off() ;numbox_reset();do_apply(funcname); parent.color = "#999999"}
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
            font.pointSize: 10
            font.family: "Arial"
        }
        MouseArea {
            id: mousearea_applyfinish_finish
            anchors.fill: parent
            hoverEnabled: true
            onEntered: parent.color = "#b5b5b5"
            onExited: parent.color = "#999999"
            onPressed: parent.color = "#3ea6b7"
            onReleased: {all_off(); focus_all_off(); numbox_reset(); parent.color = "#999999"}
        }
    }

    Item {
        id: leftselectimage
        visible: false
        width: 94
        height: 66
        anchors.left: parent.left
        anchors.leftMargin: 17
        anchors.top: parent.top
        anchors.topMargin: 70
        objectName: "flat"
        Image {
            id: flatd
            source: "qrc:/resource/flat.png"
        }

        signal flatSignal
        MouseArea {
            anchors.fill: parent
            onClicked: leftselectimage.flatSignal()
        }
    }
    Item {
        id: rightselectimage
        visible: false
        width: 94
        height: 66
        anchors.right: parent.right
        anchors.rightMargin: 17
        anchors.top: parent.top
        anchors.topMargin: 70
        objectName: "curve"
        Image {
            id: curved
            anchors.fill: parent
            source: "qrc:/resource/curve.png"
        }
        signal curveSignal
        MouseArea {
            anchors.fill: parent
            onClicked: curveSignal()
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
                font.family: "Arial"
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
                font.family: "Arial"
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
                color: "#f5f5f5"
                anchors.verticalCenter: parent.verticalCenter
                TextInput {
                    id: numberbox1_text
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    text: numbox_value_x + number_unit
                    //maximumLength:
                    font.pointSize: 9
                    font.family: "Arial"
                    color: focus ? "black" : "#595959"
                    selectByMouse: true
                    onEditingFinished:  {
                        //console.log(displayText);
                        //console.log(displayText.substring(0,displayText.length - number_unit.length).replace(/\D/g, '').length);
                        //console.log(displayText.substring(0,displayText.length - number_unit.length).length);
                        //console.log(displayText.substring(displayText.length - number_unit.length, displayText.length));
                        //console.log(number_unit);
                        if(displayText.substring(0,displayText.length - number_unit.length).replace(/\D/g, '').length === displayText.substring(0,displayText.length - number_unit.length).length){
                            if(displayText.substring(displayText.length - number_unit.length, displayText.length) === number_unit){
                                numbox_value_x = parseFloat(displayText.substring(0,displayText.length - number_unit.length));
                                //console.log("ok");
                                text = numbox_value_x + number_unit;
                            }
                            else if(displayText.replace(/\D/g, '').length === displayText.length) {
                                numbox_value_x = parseFloat(displayText);
                                text = numbox_value_x + number_unit;
                            }
                            else {
                                text = numbox_value_x + number_unit;
                            }
                        }
                        else {
                            text = numbox_value_x + number_unit;
                        }
                        //console.log(displayText);
                    }
                }
            }
            //up-button
            Rectangle {
                visible: numbox_updown_vis
                width:17
                height: 14.5
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
                width: 17
                height: 14.5
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
                color: "#f5f5f5"
                anchors.verticalCenter: parent.verticalCenter

                TextInput {
                    id: numberbox2_text
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    text: numbox_value_y + number_unit
                    font.pointSize: 9
                    font.family: "Arial"
                    color: focus ? "black" : "#595959"
                    selectByMouse: true
                    onEditingFinished:  {
                        if(displayText.substring(0,displayText.length - number_unit.length).replace(/\D/g, '').length === displayText.substring(0,displayText.length - number_unit.length).length){
                            if(displayText.substring(displayText.length - number_unit.length, displayText.length) === number_unit){
                                numbox_value_y = parseFloat(displayText.substring(0,displayText.length - number_unit.length));
                                text = numbox_value_y + number_unit;
                            }
                            else if(displayText.replace(/\D/g, '').length === displayText.length) {
                                numbox_value_y = parseFloat(displayText);
                                text = numbox_value_y + number_unit;
                            }
                            else {
                                text = numbox_value_y + number_unit;
                            }
                        }
                        else {
                            text = numbox_value_y + number_unit;
                        }
                    }
                }

            }
            //up-button
            Rectangle {
                visible: numbox_updown_vis
                width:17
                height: 14.5
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
                width: 17
                height: 14.5
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
                color: "#f5f5f5"
                anchors.verticalCenter: parent.verticalCenter
                TextInput {
                    id: numberbox3_text
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    text: numbox_value_z + number_unit
                    font.pointSize: 9
                    font.family: "Arial"
                    color: focus ? "black" : "#595959"
                    selectByMouse: true
                    onEditingFinished:  {
                        if(displayText.substring(0,displayText.length - number_unit.length).replace(/\D/g, '').length === displayText.substring(0,displayText.length - number_unit.length).length){
                            if(displayText.substring(displayText.length - number_unit.length, displayText.length) === number_unit){
                                numbox_value_z = parseFloat(displayText.substring(0,displayText.length - number_unit.length));
                                text = numbox_value_z + number_unit;
                            }
                            else if(displayText.replace(/\D/g, '').length === displayText.length) {
                                numbox_value_z = parseFloat(displayText);
                                text = numbox_value_z + number_unit;
                            }
                            else {
                                text = numbox_value_z + number_unit;
                            }
                        }
                        else {
                            text = numbox_value_z + number_unit;
                        }
                    }
                }
            }
            //up-button
            Rectangle {
                visible: numbox_updown_vis
                width:17
                height: 14.5
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
                    font.pointSize: 9
                    font.family: "Arial"
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
                    font.pointSize: 9
                    font.family: "Arial"
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
                    font.pointSize: 9
                    font.family: "Arial"
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
            TextInput {
                id: numberbox_detail2_text
                anchors.right: parent.right
                anchors.rightMargin: 5
                anchors.verticalCenter: parent.verticalCenter
                text: numbox_value_detail2 + number_unit_detail2
                font.pointSize: 9
                font.family: "Arial"
                color: focus ? "black" : "#595959"
                selectByMouse: true
                onEditingFinished:  {
                    if(displayText.substring(0,displayText.length - number_unit_detail2.length).replace(/\D/g, '').length === displayText.substring(0,displayText.length - number_unit_detail2.length).length){
                        if(displayText.substring(displayText.length - number_unit_detail2.length, displayText.length) === number_unit_detail2){
                            numbox_value_detail2 = parseFloat(displayText.substring(0,displayText.length - number_unit_detail2.length));
                            text = numbox_value_detail2 + number_unit_detail2;
                        }
                        else if(displayText.replace(/\D/g, '').length === displayText.length) {
                            numbox_value_detail2 = parseFloat(displayText);
                            text = numbox_value_detail2 + number_unit_detail2;
                        }
                        else {
                            text = numbox_value_detail2 + number_unit_detail2;
                        }
                    }
                    else {
                        text = numbox_value_detail2 + number_unit_detail2;
                    }
                }
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
}
