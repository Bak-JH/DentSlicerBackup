import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    objectName: "progress_popup"
    width: 462
    height: 110
    anchors.centerIn: parent
    visible : false

    color: "#E5E5E5"
    //color: "red"
    border.width: 1
    border.color:"#CCCCCC"

    property int progressNumber
    property string progressText : "calculating..."

    Rectangle{//shadow
        id : shadowRect
        width: parent.width-2
        height: parent.height-2
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        color: "#CCCCCC"
    }

    Rectangle{//main
        id : contentRect
        width: parent.width-2
        height: parent.height-2
        anchors.top: parent.top
        anchors.left: parent.left

        color: "#ffffff"

        Text {
            id: progress_text1

            text: "0% to complete"
            anchors.top: parent.top
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 20
            color: "black"
            font.family: mainFont.name
        }
        Text {
            id: progress_text2
            text: "Calculating..."
            anchors.top : parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 50
            font.pixelSize: 18
            color: "#666666"
            font.family: mainFont.name
        }

        ProgressBar {
            id:progress_value
            value: 0
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 22
            style:ProgressBarStyle {
                background: Rectangle{
                    color:"#EFEFEF"
                    implicitWidth: 410
                    implicitHeight: 8
                }
                progress: Rectangle{
                    color:"#44A6B6"
                    implicitHeight: 8
                    implicitWidth: progress_value.value
                }
            }
        }
    }





    function openPopUp(){
        console.log("open tttttttttttttt")
        progressPopUp.visible = true
        progress_text1.text = "0% to complete"
        progress_text2.text = "Working..."
    }

    function updateNumber(value){
        progress_value.value=value;
        progress_text1.text=Math.round(value*100) + "% to complete";
        if(value==1){
            progressPopUp.visible = false
            progress_text1.text = "0% to complete"
            //result_orient.state="active";
            //result_text.text="Orientation Complete.";
        }
    }

    function updateText(inputText){
        console.log("upupuputtttt " + inputText);
        progress_text2.text = inputText;
    }

}
