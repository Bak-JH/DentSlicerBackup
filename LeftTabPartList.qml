import QtQuick 2.0
import QtQuick.Layouts 1.3

BlurRectangle{
    inputWidth: 256
    inputHeight: 320
    inputRadius: 8

    anchors.top: parent.top
    anchors.left: parent.left

    Text{
        width: 160
        height : 20

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 16
        anchors.topMargin: 16

        text : "Part List"

        color: "#000000"
        font.family: "Helvetica"
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 16
    }

    ColumnLayout{  // Update to ScrollView
        width: 232
        height: 252
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin:  52

        Layout.fillHeight: true

        spacing : 8

        LeftTabPartListElement{}
        LeftTabPartListElement{}
        /*
        LeftTabPartListElement{}
        LeftTabPartListElement{}
        LeftTabPartListElement{}
        LeftTabPartListElement{}
        LeftTabPartListElement{}
        */

        Item { Layout.fillHeight: true } // For killing remain space

    }



}
