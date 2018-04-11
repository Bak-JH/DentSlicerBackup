import Qt3D.Core 2.0

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Rectangle {
    property alias ltso: ltso

    color: "#EFEFEF"

    Rectangle{
        id: rightline
        width: 2
        anchors.right : parent.right
        anchors.top : parent.top
        anchors.bottom: parent.bottom

        color: "#CECECE"
    }
    LeftTabPartList{
        id : ltpl
        anchors.top : parent.top
        anchors.left: parent.left
    }

    LeftTabViewMode{
        id : ltvm
        anchors.top : ltpl.bottom
        anchors.left : parent.left
    }

    LeftTabSlicingOption{
        id:ltso
        anchors.top : ltvm.bottom
        anchors.left: parent.left
    }

}
