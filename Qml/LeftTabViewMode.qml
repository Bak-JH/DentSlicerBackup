import QtQuick 2.0

import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

Rectangle {
    z: 10
    property real originalHeight: 126
    width : 264
    height : originalHeight

    color : "#efefef"

    state : "open"

    function setObjectView() {
        viewObjectButton.checked = true;
    }

    function setViewMode(mode){
        viewObjectButton.checked = false;
        viewLayerButton.checked = false;
        if (mode === 0)
            viewObjectButton.checked = true;
        else if (mode === 2)
            viewLayerButton.checked = true;
    }

    function setEnable(enable) {
        viewObjectButton.checked = true;
        viewLayerButton.enabled = enable;

        qm.setViewMode(0);
    }

    Rectangle{
        id : tab
        width: parent.width
        height: 28
        anchors.left : parent.left
        anchors.top :  parent.top
        z: 10

        color: "#efefef"

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

            anchors.verticalCenter: parent.verticalCenter
            anchors.right : parent.right
            anchors.rightMargin: 12

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
        height: parent.height - 28
        anchors.left : parent.left
        anchors.top :  tab.bottom

        color: "#efefef"

        Item{
            width: parent.width
            height: parent.height
            anchors.left: parent.left
            anchors.top : parent.top
            anchors.leftMargin: 16
            anchors.topMargin: 12

            ColumnLayout {
                spacing: 10

                ExclusiveGroup { id: viewModeGroup }
                RadioButton {
                    id: viewObjectButton
                    objectName: "viewObjectButton"
                    checked: true
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
                            text: "Object View"
                            anchors.left : parent.left
                            anchors.leftMargin: 10

                            font.family: mainFont.name
                            font.pixelSize: 14
                        }
                    }

                    signal onChanged(bool checked);

                    //onCheckedChanged: {
                    onClicked: {
                        console.log("Object View Selected")
                        onChanged(checked);
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
                    id: viewLayerButton
                    objectName: "viewLayerButton"
                    exclusiveGroup: viewModeGroup
                    enabled: false

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
                            text: "Layer View"
                            anchors.left : parent.left
                            anchors.leftMargin: 10
                            color: control.enabled ? "#000000" : "#888888"

                            font.family: mainFont.name
                            font.pixelSize: 14
                        }
                    }

                    signal onChanged(bool checked);
                    
                    //onCheckedChanged: {
                    onClicked: {
                        console.log("Layer View Selected")
                        onChanged(checked);
                        if (qm.getViewMode() === 0) {
                            viewObjectButton.checked = true
                        } else if (qm.getViewMode() === 1) {
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
