import Qt3D.Core 2.0
import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Rectangle{
    width:240
    height: 295
    color: "#EEEEEE"
    border.color: "black"

    Item{
        id:flat
        width:10;height:66
        anchors.left:parent.left
        anchors.leftMargin: 30
        anchors.bottom:parent.bottom
        anchors.bottomMargin: 160
        objectName: "flat"
        signal flatSignal()
            Image{
                id:flatd
                source: "qrc:/Resource/flat_cut.png"
                    }
            MouseArea{
                anchors.fill:parent
                hoverEnabled : true
                onEntered : qm.setHandCursor();
                onExited : qm.resetCursor();
                onClicked: flat.flatSignal()}
    }

    Item{
        id:curve
        width:66;height:66
        anchors.right:parent.right
        anchors.rightMargin:30
        anchors.bottom:parent.bottom
        anchors.bottomMargin: 160
        objectName: "curve"
        signal curveSignal()
            Image {
                id:curved
                anchors.fill: parent
                source: "qrc:/Resource/free_cut.png"
                }
            MouseArea {
                anchors.fill:parent
                hoverEnabled : true
                onEntered : qm.setHandCursor();
                onExited : qm.resetCursor();
                onClicked: curve.curveSignal()}}


    Text {
        text: "CUT"
        font.family: mainFont.name
        styleColor: "#000000"
        font.pixelSize: 17
        anchors.top:  parent.top
        anchors.topMargin: 10
        anchors.left:  parent.left
        anchors.leftMargin: 10
        }

    Text {
        text: "Cutting Surface --------"
        font.family: mainFont.name
        color: "#aaaaaa"
        font.pixelSize: 15
        anchors.top:  parent.top
        anchors.topMargin: 40
        anchors.left:  parent.left
        anchors.leftMargin: 20
        }

    Text {
        text: "After treatment --------"
        font.family: mainFont.name
        color: "#aaaaaa"
        font.pixelSize: 15
        anchors.top:  parent.top
        anchors.topMargin: 160
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
                font.family: mainFont.name
                font.bold: true
                font.pixelSize: 17
                anchors.horizontalCenter:  parent.horizontalCenter
                anchors.verticalCenter:  parent.verticalCenter
                }
            MouseArea{
                anchors.fill:parent
                hoverEnabled : true
                onEntered : qm.setHandCursor();
                onExited : qm.resetCursor();
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
        state:'Inactivation'
        states: [
            State{
                name:"Inactivation"
                PropertyChanges { target: slider;  visible: false}
            },
            State{
                name:"Activation"
                PropertyChanges { target: slider;  visible: true}

            }
        ]
            Item{
                width:parent.width;height:parent.height;anchors.horizontalCenter:parent.horizontalCenter;anchors.verticalCenter:parent.verticalCenter
                id:item
                objectName: "item"
                signal qmlSignal()
                    Text {
                        text: "Apply"
                        color:"#ffffff"
                        font.family: mainFont.name
                        font.bold: true
                        font.pixelSize:17
                        anchors.horizontalCenter:  parent.horizontalCenter
                        anchors.verticalCenter:  parent.verticalCenter}
                    MouseArea{
                        anchors.fill:parent
                        hoverEnabled : true
                        onEntered : qm.setHandCursor();
                        onExited : qm.resetCursor();
                        onClicked:{ item.qmlSignal()
                         applyBox.state == 'Activation' ? applyBox.state = 'Inactivation' : applyBox.state = 'Activation';}}}
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
    Planeslider{id:slider;anchors.right: parent.left;anchors.rightMargin:20;anchors.bottom:parent.bottom;anchors.bottomMargin:-20;}
}
