import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    objectName: "result_popup"
    width: 320
    height: 120
    anchors.centerIn: parent
    visible : false

    color: "#E5E5E5"
    border.width: 1
    border.color:"#CCCCCC"


    Text {
        id: result_text

        text: "The model is already"
        anchors.top: parent.top
        anchors.topMargin: 32
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: 16
        color: "black"
        font.family: "Arial"
    }

    Rectangle {
        id: okButton
        width: 100
        height: 28
        color: "#999999"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 16
        anchors.horizontalCenter: parent.horizontalCenter

        Text {
            id: okButton_text
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text: "OK"
            color: "#ffffff"
            font.pointSize: 12
            font.family: "Arial"
        }
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: parent.color = "#b5b5b5"
            onExited: parent.color = "#999999"
            onPressed: parent.color = "#3ea6b7"
            onReleased: {
                closePopUp();
                parent.color = "#999999"
            }
        }
    }


    function openResultPopUp(inputText){
        console.log("open resulttttttttttttttt")
        resultPopUp.visible = true
        result_text.text = inputText;
    }

    function closePopUp(){
        resultPopUp.visible = false
    }


}
