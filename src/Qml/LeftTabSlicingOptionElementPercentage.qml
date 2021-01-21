import QtQuick 2.4

import QtQuick.Controls 2.2


Item {
    width: parent.width
    height: 24
    signal usrInputChanged(string opName, variant percent)
    property string columnName
    property alias currentValue: inputControl.value

    Text{
        id : elemnt3_text
        width: 116
        height: parent.height
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.verticalCenter: parent.verticalCenter

        font.pixelSize: 14
        anchors.leftMargin:14
        text: columnName
        font.family: 
		.name
        verticalAlignment: Text.AlignBottom
        color : "black"
    }

    Item {
        id: numberBoxItem
        width: 116
        height: parent.height
        anchors.top: parent.top
        anchors.left : elemnt3_text.right
        //number box

        SpinBox
        {
            id: inputControl
            width: parent.width
            height: parent.height
            anchors.centerIn: parent
            font.pixelSize: 12
            font.family: mainFont.name
            from: 0
            to: 100
            stepSize: 10
            textFromValue: function(value, locale) {
                usrInputChanged(columnName, value);
                return qsTr(value + "%");
            }

        }


    }


}
