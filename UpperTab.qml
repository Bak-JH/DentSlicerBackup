import Qt3D.Core 2.0

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Item {
    width : parent.width
    height: 120
    /*
    Rectangle{
        anchors.fill: parent

        color: "#E2E1E1"
    }
    */
    Rectangle{
        id : bottomine
        height: 4
        //anchors.fill: parent

        anchors.left : parent.left
        anchors.right : parent.right
        anchors.bottom: parent.bottom
        color: "#A8A8A8"
    }
}
