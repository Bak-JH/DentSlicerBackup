import QtQuick 2.0

import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

Rectangle {
    property real originalHeight: 126
    width : 264
    height : originalHeight

    color : "transparent"

    state : "open"

    Rectangle{
        id : tab
        width: parent.width
        height: 28
        anchors.left : parent.left
        anchors.top :  parent.top

        color: "transparent"

        Text{
            height: parent.height
            text : "View Mode"

            color:"#888D8E"
            font.family: mainFont.name
            font.pixelSize: 16

            anchors.bottom: parent.bottom
            anchors.left : parent.left
            anchors.leftMargin: 14
            verticalAlignment: Text.AlignBottom
        }

        Image{
            id : arrow
            source: "qrc:/resource/triangle.png"
            width : 12
            height : 10
            rotation: 0

//            anchors.verticalCenter: parent.verticalCenter
            anchors.right : parent.right
            anchors.rightMargin: 12
            anchors.top: parent.top
            anchors.topMargin: 10

            MouseArea {
                    anchors.fill: parent
                    hoverEnabled : true
                    onEntered : qm.setHandCursor();
                    onExited : qm.resetCursor();
                    onClicked: { ltvm.state == 'open' ? ltvm.state = "close" : ltvm.state = 'open';}
            }
        }
    }

    Rectangle{
        id : content
        width: parent.width
        height: parent.height - 24
        anchors.left : parent.left
        anchors.top :  tab.bottom

        color: "transparent"

        Item{
            width: parent.width
            height: parent.height
            anchors.left: parent.left
            anchors.top : parent.top
            anchors.leftMargin: 16
            anchors.topMargin: 8
//            anchors.topMargin: 12

            ColumnLayout {
                spacing: 10

                ExclusiveGroup { id: viewModeGroup }
                RadioButton {
                    checked: true
                    exclusiveGroup: viewModeGroup
                    id: radio_ObjectView
                    style: RadioButtonStyle {
                        indicator: Rectangle {
                            implicitWidth: 16
                            implicitHeight: 16
                            radius: 9
//                            border.color: control.activeFocus ? "darkblue" : "gray"
                            border.color: radio_ObjectView.hovered ? "#42bfcc" : "transparent"
//                            border.width: 0
                            Rectangle {
                                anchors.fill: parent
                                visible: control.checked
                                color: "#505A5E"
                                radius: 9
                                anchors.margins: 4
                            }
                        }
                        label: Text {
                            text: "Object View"
                            anchors.left : parent.left
                            anchors.leftMargin: 10

                            font.family: mainFont.name
                            font.pixelSize: 14
                        }
                    }
                    onHoveredChanged: {
                        if(hovered)
                            qm.setHandCursor();
                        else
                            qm.resetCursor();
                    }
                }
                RadioButton {
//                    text: "Support View"
                    id: radio_SupportView
                    exclusiveGroup: viewModeGroup
                    style: RadioButtonStyle {
                        indicator: Rectangle {
                            implicitWidth: 16
                            implicitHeight: 16
                            radius: 9
//                            border.color: control.activeFocus ? "darkblue" : "gray"
                            border.color: radio_SupportView.hovered ? "#42bfcc" : "transparent"
//                            border.width: 1
                            Rectangle {
                                anchors.fill: parent
                                visible: control.checked
                                color: "#505A5E"
                                radius: 9
                                anchors.margins: 4
                            }
                        }
                        label: Text {
                            text: "Support View"
                            anchors.left : parent.left
                            anchors.leftMargin: 10

                            font.family: mainFont.name
                            font.pixelSize: 14
                        }
                    }
                    onHoveredChanged: {
                        if(hovered)
                            qm.setHandCursor();
                        else
                            qm.resetCursor();
                    }
                }
                RadioButton {
                    //text: "Layer View"
                    exclusiveGroup: viewModeGroup
                    id: radio_LayerView
                    style: RadioButtonStyle {
                        indicator: Rectangle {
                            implicitWidth: 16
                            implicitHeight: 16
                            radius: 9
                            border.color: radio_LayerView.hovered ? "#42bfcc" : "transparent"
//                            border.color: control.activeFocus ? "darkblue" : "gray"
//                            border.width: 0
                            Rectangle {
                                anchors.fill: parent
                                visible: control.checked
                                color: "#505A5E"
                                radius: 9
                                anchors.margins: 4
                            }
                        }
                        label: Text {
                            text: "Layer View"
                            anchors.left : parent.left
                            anchors.leftMargin: 10

                            font.family: mainFont.name
                            font.pixelSize: 14
                        }
                    }
                    onHoveredChanged: {
                        if(hovered)
                            qm.setHandCursor();
                        else
                            qm.resetCursor();
                    }
                }
            }
        }


    }

    states: [
        State{
            name:"open"
            PropertyChanges { target: arrow; rotation:0 }
            PropertyChanges { target: content; visible:true }
            PropertyChanges { target: ltvm; height : originalHeight }

        },
        State{
            name:"close"
            PropertyChanges { target: arrow; rotation:180 }
            PropertyChanges { target: content; visible:false }
            PropertyChanges { target: ltvm; height : 28 }
        }
    ]


}
