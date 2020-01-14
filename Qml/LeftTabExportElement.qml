import QtQuick 2.0

import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Private 1.0

Item {
    width : 224
    height: 20

    Layout.alignment: Qt.AlignHCenter
    property string columnName
    property string columnContent

    Text{
        id:leftText
        width: 100
        anchors.left: parent.left
        anchors.top: parent.top

        text : columnName

        font.pixelSize: 14
        //font.bold: true
        font.family: mainFont.name

        color: "black"
    }

    Text{
        id:rightText
        anchors.right: parent.right
        anchors.top: parent.top

        text : columnContent

        font.pixelSize: 14
        font.family: mainFont.name

        color: "#7c7c7c"
    }

    Rectangle{
        width: parent.width
        height: 1

        anchors.bottom: parent.bottom
        color: "#b2b2b2"
    }




}
