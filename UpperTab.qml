import Qt3D.Core 2.0

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0


Rectangle {
    color: "#E2E1E1"
    property int buttonWidth : 86
    property int buttonHeight : 100
    property var options: []


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
            anchors.left: parent.left
            iconSource: "qrc:/resource/upper_open.png"
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

            anchors.left: first_tab_button_open.right
            iconSource: "qrc:/resource/upper_save.png"
            iconText: "Save"
        }


        UpperButton{
            id : first_tab_button_export

            anchors.left: first_tab_button_save.right
            iconSource: "qrc:/resource/upper_export.png"
            iconText: "Export"

            MouseArea{
                anchors.fill: parent
                onClicked:{
                    function collectConfigurations(){
                        var configurations = {};

                        // do collecting things
                        // configurations[key] = value;
                        configurations["resolution"] = "resolution#"+options[0];
                        configurations["layer_height"] = "layer_height#"+options[1];
                        configurations["support"] = "support#"+options[2];
                        configurations["infill"] = "infill#"+options[3];
                        configurations["raft"] = "raft#"+options[4];
                        return configurations;
                    }

                    console.log(parent.id);
                    console.log(parent.parent.id);

                    console.log("exporting");
                    // collect configurations
                    var cfg = collectConfigurations();
                    se.slice(cfg);
                }
            }
        }

        Rectangle{
            id : first_tab_right_line
            width: 2
            height: buttonHeight

            anchors.right: parent.right
            anchors.top : parent.top
            anchors.bottom : tabgroupname.top

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
            iconSource: "qrc:/resource/upper_move.png"
            iconText: "Move"

        }
        UpperButton{
            id : second_tab_button_rotate

            anchors.left: second_tab_button_move.right
            iconSource: "qrc:/resource/upper_rotate.png"
            iconText: "Rotate"
        }
        UpperButton{
            id : second_tab_button_layflat

            anchors.left: second_tab_button_rotate.right
            iconSource: "qrc:/resource/upper_layflat.png"
            iconText: "Lay flat"
        }
        UpperButton{
            id : second_tab_button_arrange

            anchors.left: second_tab_button_layflat.right
            iconSource: "qrc:/resource/upper_arrange.png"
            iconText: "Arrange"
        }
        UpperButton{
            id : second_tab_button_orient

            anchors.left: second_tab_button_arrange.right
            iconSource: "qrc:/resource/upper_orientation.png"
            iconText: "Orient"
        }

        Rectangle{
            id : second_tab_right_line
            width: 2
            height: buttonHeight

            anchors.right: parent.right
            anchors.top : parent.top
            anchors.bottom : tabgroupname.top

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

            anchors.left: parent.left
            iconSource: "qrc:/resource/upper_scale.png"
            iconText: "Scale"
        }
        UpperButton{
            id : third_tab_button_autorepair

            anchors.left: third_tab_button_scale.right
            iconSource: "qrc:/resource/upper_autorepair.png"
            iconText: "Auto Repair"
        }
        UpperButton{
            id : third_tab_button_cut
            state:'Inactivation'
            anchors.left: third_tab_button_autorepair.right
            anchors.leftMargin: 4
            anchors.bottom: third_tab_button_autorepair.bottom
            anchors.bottomMargin: 3
            iconSource: "qrc:/resource/upper_cut.png"
            iconText: "Cut"


            states: [
                State{
                    name:"Inactivation"
                    PropertyChanges { target: third_tab_button_cut;  iconSource: "qrc:/resource/upper_cut.png" }
                    PropertyChanges { target: cutbox;  visible: false}
                },
                State{
                    name:"Activation"
                    PropertyChanges { target: third_tab_button_cut;  iconSource: "qrc:/resource/cut_act.png" }
                    PropertyChanges { target: third_tab_button_cut;  iconText: ""}
                    PropertyChanges { target: cutbox;  visible: true}

                }
            ]

            MouseArea {
                    anchors.fill: parent

                    onClicked: { third_tab_button_cut.state == 'Activation' ? third_tab_button_cut.state = 'Inactivation' : third_tab_button_cut.state = 'Activation';}
                        }

             Rectangle{
                    id : cutbox
                    visible: false
                    width:240
                    height: 320
                    color: "#EEEEEE"
                    border.color: "black"
                    anchors.horizontalCenter:  parent.horizontalCenter
                    anchors.top:  parent.bottom
                    anchors.topMargin: 35


            MouseArea{

                    anchors.fill: parent
                    onPressed:parent.color = "pink"
                    onReleased: parent.color = "#EEEEEE"
            }

                    Text {
                    text: "CUT"
                    font.family: "Arial"
                    styleColor: "#000000"
                    font.pixelSize: 17
                    anchors.top:  parent.top
                    anchors.topMargin: 10
                    anchors.left:  parent.left
                    anchors.leftMargin: 10
                    }

                    Text {
                    text: "Cutting Surface --------"
                    font.family: "Arial"
                    color: "#aaaaaa"
                    font.pixelSize: 15
                    anchors.top:  parent.top
                    anchors.topMargin: 40
                    anchors.left:  parent.left
                    anchors.leftMargin: 20
                    }

                    Text {
                    text: "After treatment --------"
                    font.family: "Arial"
                    color: "#aaaaaa"
                    font.pixelSize: 15
                    anchors.top:  parent.top
                    anchors.topMargin: 180
                    anchors.left:  parent.left
                    anchors.leftMargin: 20
                    }

                    Rectangle{
                    width: 109.5
                    height: 30
                    color: "#ababab"
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin:7
                    anchors.right: parent.right
                    anchors.rightMargin:7
                        Text {
                        text: "Finish"
                        color:"#ffffff"
                        font.family: "Arial"
                        font.bold: true
                        font.pixelSize: 17
                        anchors.horizontalCenter:  parent.horizontalCenter
                        anchors.verticalCenter:  parent.verticalCenter
                    }
                    MouseArea{
                    anchors.fill:parent
                    onClicked:{
                                   Qt.quit();
                               }
                    }


                    }

                    Rectangle {
                    width: 109.5
                    height: 30
                    id:applyBox
                    color: "#ababab"
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin:7
                    anchors.left: parent.left
                    anchors.leftMargin:7
                Item{
                    width:parent.width;height:parent.height;anchors.horizontalCenter:parent.horizontalCenter;anchors.verticalCenter:parent.verticalCenter
                    id:item

                    objectName: "item"

                    signal qmlSignal()

                        Text {
                        text: "Apply"
                        color:"#ffffff"
                        font.family: "Arial"
                        font.bold: true
                        font.pixelSize:17
                        anchors.horizontalCenter:  parent.horizontalCenter
                        anchors.verticalCenter:  parent.verticalCenter}


                        MouseArea{
                            anchors.fill:parent
                            onClicked: item.qmlSignal()}}

}




                    ExclusiveGroup { id: viewModeGroup }
                    RadioButton {
                        anchors.bottom : parent.bottom
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
                                anchors.left : parent.left
                                anchors.leftMargin: 10

                                font.family: "Arial"
                                font.pixelSize: 14
                            }
                        }
                    }

                    RadioButton {
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
                                anchors.left : parent.left
                                anchors.leftMargin: 10

                                font.family: "Arial"
                                font.pixelSize: 14
                            }
                        }
                    }


             }


        UpperButton{
            id : third_tab_button_shelloffset

            anchors.left: third_tab_button_cut.right
            iconSource: "qrc:/resource/upper_shelloffset.png"
            iconText: "Shell Offset"
        }

        Rectangle{
            id : third_tab_right_line
            width: 2
            height: buttonHeight

            anchors.right: parent.right
            anchors.top : parent.top
            anchors.bottom : tabgroupname.top

            color : "#CECECE"
        }

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
            id : fourth_tab_button_base

            anchors.left: parent.left
            iconSource: "qrc:/resource/upper_base.png"
            iconText: "Base"
        }

        UpperButton{
            id : fourth_tab_button_support

            anchors.left: fourth_tab_button_base.right
            iconSource: "qrc:/resource/upper_support.png"
            iconText: "Support"
        }
        UpperButton{
            id : fourth_tab_button_label

            anchors.left: fourth_tab_button_support.right
            iconSource: "qrc:/resource/upper_label.png"
            iconText: "Label"
        }

        Rectangle{
            id : fourth_tab_right_line
            width: 2
            height: buttonHeight

            anchors.right: parent.right
            anchors.top : parent.top
            anchors.bottom : tabgroupname.top

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

}

