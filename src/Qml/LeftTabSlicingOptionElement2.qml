import QtQuick 2.0

import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtQuick.Controls.Private 1.0

Item {
    width: parent.width
    height: 24 //14


    property string columnName
    property string columnText

    Text{
        id:text
        width: 116
        height: parent.height
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter

        font.pixelSize: 14
        //font.
		: true
        anchors.leftMargin:14
        text: columnName
        font.family: 
		.name
        verticalAlignment: Text.AlignBottom
        color : "black"
    }

    Text {
        width: 116
        height: 14
        anchors.left: text.right
        anchors.bottom: parent.bottom

        font.pixelSize: 12
        text: columnText
        font.family: mainFont.name
        color: "#727272"
        verticalAlignment: Text.AlignBottom
    }


}
