import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Scene3D 2.0
import QtQuick.Window 2.2
//import QtCanvas3D 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2
import QtWinExtras 1.0
import Qt3D.Input 2.0
ApplicationWindow {
    width: 356
    height: 360
    visible: false
    flags : Qt.Window | Qt.FramelessWindowHint | Qt.WindowMaximizeButtonHint | Qt.WindowMinimizeButtonHint
    id: root
    signal resume();

    function doResume()
    {
        visible = false;
        resume();
    }
    
    Rectangle{//shadow
        id : shadowRect
        width: parent.width-2
        height: parent.height-2
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        color: "#CCCCCC"
    }
    /*
    Rectangle{//main
        width: parent.width-2
        height: parent.height-2
        anchors.top: parent.top
        anchors.left: parent.left
        color: "#ffffff"
    }*/

    Rectangle{//main
        id : contentRect
        width: parent.width-2
        height: parent.height-2
        anchors.top: parent.top
        anchors.left: parent.left
        // focus: true

        color: "#ffffff"

        Keys.onPressed: {
            if (event.key == Qt.Key_Enter) {
                root.doResume();
            }
        }


        Image{
            width : 217
            height: 32
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 75

            source: "qrc:/Resource/dentslicer_login_logo.png"
        }

        Text{
            id : info
            anchors.top : parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 170

            text : "Disconnected, resume?"

            font.pixelSize: 14
            font.family: mainFont.name
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "#00B9C8"
            visible: true
        }





        Rectangle { // login button
            id : resumeButton
            objectName: "resumeButton"
            width: 128
            height: 36
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.leftMargin: 36
            anchors.bottomMargin: 36
            color: "#00B9C8"
            Text{
                width: parent.width
                height: parent.height
                anchors.centerIn: parent

                text : "RESUME"

                font.pixelSize: 20
                font.family: mainFont.name
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "white"
            }
            MouseArea {
                cursorShape: Qt.PointingHandCursor
                anchors.fill: parent
                hoverEnabled: true
                onEntered : qm.setHandCursor();
                onExited : qm.resetCursor();
                onClicked: {
                    root.doResume();
                }
            }
        }

        Rectangle { // cancel button
            width: 128
            height: 36
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.rightMargin: 36
            anchors.bottomMargin: 36
            color: "#00B9C8"
            Text{
                width: parent.width
                height: parent.height
                anchors.centerIn: parent

                text : "CANCEL"

                font.pixelSize: 20
                font.family: mainFont.name
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "white"
            }

            MouseArea {
                cursorShape: Qt.PointingHandCursor
                anchors.fill: parent
                hoverEnabled: true
                onEntered : qm.setHandCursor();
                onExited : qm.resetCursor();
                onClicked: {
                    Qt.quit()
                }
            }
        }

    }
    TaskbarButton {
            overlay.iconSource: "qrc:/icon.ico"
    }




}
