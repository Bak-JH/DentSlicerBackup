import QtQuick 2.0

import QtQuick.Controls 1.2
import QtQuick.Layouts 1.3

Rectangle {

    signal optionChanged(string opName, variant newIndex);
    property var optionElements: []
    function addOptionElementCombo(optionName, contents, defaultVal)
    {
        var opComp = Qt.createComponent("LeftTabSlicingOptionElement.qml");
        var newOption =
                opComp.createObject(optionColLayout,
                {
                    "columnName" : optionName,
                    "columnContents" : contents,
                    "currentValue" : defaultVal
                });
        newOption.usrInputChanged.connect(optionChanged);
        optionElements.push(newOption);
    }

    function addOptionElementPercentage(optionName, defaultVal)
    {
        var opComp = Qt.createComponent("LeftTabSlicingOptionElementPercentage.qml");
        var newOption =
                opComp.createObject(optionColLayout,
                {
                    "columnName" : optionName,
                    "currentValue" : defaultVal
                });
        newOption.usrInputChanged.connect(optionChanged);
        optionElements.push(newOption);
    }
    width : 264
    height : 180

    color : "#efefef"

    state : "open"

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
            text : "Slicing Option"

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
            //transform:Rotation{origin.x: arrow.width/2; origin.y: arrow.height/2; axis { x: 1; y: 0; z: 0 } angle: 0 }
            rotation: 0

            anchors.verticalCenter: parent.verticalCenter
            anchors.right : parent.right
            anchors.rightMargin: 12

            MouseArea {
                    anchors.fill: parent
                    hoverEnabled : true
                    onEntered : qm.setHandCursor();
                    onExited : qm.resetCursor();
                    onClicked: { ltso.state == 'open' ? ltso.state = "close" : ltso.state = 'open';}
            }
        }
    }

    Rectangle{
        id : content
        width: parent.width
        height: parent.height - 24
        anchors.left : parent.left
        anchors.top :  tab.bottom

        color:"#efefef"
        Item {
            width: parent.width
            height: parent.height
            anchors.left: parent.left
            anchors.top : parent.top

            anchors.topMargin: 16

            ColumnLayout {
                id: optionColLayout
                spacing: 2
            }
        }
    }

    states: [
        State{
            name:"open"
            PropertyChanges { target: arrow; rotation:0 }
            PropertyChanges { target: content; visible:true }
            PropertyChanges { target: ltso; height : 180 }

        },
        State{
            name:"close"
            PropertyChanges { target: arrow; rotation:180 }
            PropertyChanges { target: content; visible:false }
            PropertyChanges { target: ltso; height : 28 }
        }
    ]


}
