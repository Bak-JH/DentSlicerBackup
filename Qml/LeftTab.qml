import Qt3D.Core 2.0

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.0

Rectangle {
    property alias ltso: ltso
    objectName: "boxLeftTab"

    color: "#EFEFEF"
    z: 10
    id: frame
    clip: true

    Rectangle{
        id: rightline
        width: 1
        anchors.right : parent.right
        anchors.top : parent.bottom
        anchors.bottom: parent.bottom

        color: "#CCCCCC"
    }
    LeftTabPartList{
        id : ltpl
        anchors.top : parent.top
        anchors.left: parent.left
    }

    Rectangle{
        id : line1
        width : parent.width-1
        height: 2
        anchors.left: parent.left
        anchors.top : ltpl.bottom
        color: "#D8D8D8"
    }

    LeftTabViewMode{
        id : ltvm
        objectName: "ltvm"
        anchors.top : line1.bottom
        anchors.left : parent.left
    }
    Rectangle{
        id : line2
        width : parent.width-1
        height: 2
        anchors.left: parent.left
        anchors.top : ltvm.bottom
        color: "#D8D8D8"
    }
    LeftTabSlicingOption{
        id:ltso
        objectName: "ltso"
        anchors.top : line2.bottom
        anchors.left: parent.left
    }

    function disableLefttab(){
        lefttab.enabled = false
    }

    function enableLefttab(){
        lefttab.enabled = true
    }

}
