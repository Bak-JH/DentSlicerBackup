import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Scene3D 2.0
import QtQuick.Window 2.2
import QtCanvas3D 1.1
import QtQuick.Controls.Styles 1.4
import "glcode.js" as GLCode
import QtQuick.Dialogs 1.2
import QtWinExtras 1.0
import Qt3D.Input 2.0
ApplicationWindow {
    width: 356
    height: 360
    visible: false
    flags : Qt.Window | Qt.FramelessWindowHint | Qt.WindowMaximizeButtonHint | Qt.WindowMinimizeButtonHint

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
        focus: true

        color: "#ffffff"

        Keys.onPressed: {
            if (event.key == Qt.Key_Enter) {
                loginButton.loginTrial(serialbox_text1.text + serialbox_text2.text + serialbox_text3.text + serialbox_text4.text);
            }
        }


        Image{
            width : 136
            height: 86
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 60

            source: "qrc:/Resource/login_logo.png"
        }

        Text{
            id : loginErrorText
            anchors.top : parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 170

            text : "Please check your ID & Password"

            font.pixelSize: 14
            font.family: mainFont.name
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: "red"
            visible: false
        }

        //Serial Box
        Item {
            id: serialItem
            width: 284
            height: 28
            anchors.top: parent.top
            anchors.left : parent.left
            anchors.topMargin: 210
            anchors.leftMargin: 40
            //number box
            Rectangle {
                id: serialbox
                width: parent.width
                height: parent.height
                color: "transparent"
                Text{
                    width: parent.width
                    height: parent.height
                    anchors.centerIn: parent

                    text : qsTr("Serial")

                    font.pixelSize: 20
                    font.family: mainFont.name
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    color: "#0DA3B2"
                }
                TextField {
                    id: serialbox_text1
                    width: (parent.width-100)/4
                    anchors.right: serialbox_text2.left
                    anchors.rightMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: TextInput.AlignRight
                    placeholderText: "AAAA"
                    font.pixelSize: 12
                    font.family: mainFont.name
                    font.capitalization: Font.AllUppercase
                    textColor: focus ? "black" : "#737375"
                    selectByMouse: true
                    maximumLength : 4
                    inputMethodHints : Qt.ImhUppercaseOnly

                    style: TextFieldStyle {
                        textColor: "black"
                        background: Rectangle {
                            border.width: 0
                        }
                    }
                }
                Text{
                    width: 6
                    anchors.right: serialbox_text2.left
                    anchors.rightMargin: -2
                    anchors.verticalCenter: parent.verticalCenter

                    text : qsTr("-")

                    font.pixelSize: 15
                    font.family: mainFont.name
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "#0DA3B2"
                }
                TextField {
                    id: serialbox_text2
                    width: (parent.width-100)/4
                    anchors.right: serialbox_text3.left
                    anchors.rightMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: TextInput.AlignRight
                    placeholderText: "AAAA"
                    font.pixelSize: 12
                    font.family: mainFont.name
                    font.capitalization: Font.AllUppercase
                    textColor: focus ? "black" : "#737375"
                    selectByMouse: true
                    maximumLength : 4

                    style: TextFieldStyle {
                        textColor: "black"
                        background: Rectangle {
                            border.width: 0
                        }
                    }
                }
                Text{
                    width: 6
                    anchors.right: serialbox_text3.left
                    anchors.rightMargin: -2
                    anchors.verticalCenter: parent.verticalCenter
                    text : qsTr("-")

                    font.pixelSize: 15
                    font.family: mainFont.name
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "#0DA3B2"
                }
                TextField {
                    id: serialbox_text3
                    width: (parent.width-100)/4
                    anchors.right: serialbox_text4.left
                    anchors.rightMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: TextInput.AlignRight
                    placeholderText: "AAAA"
                    font.pixelSize: 12
                    font.family: mainFont.name
                    font.capitalization: Font.AllUppercase
                    textColor: focus ? "black" : "#737375"
                    selectByMouse: true
                    maximumLength : 4

                    style: TextFieldStyle {
                        textColor: "black"
                        background: Rectangle {
                            border.width: 0
                        }
                    }
                }
                Text{
                    width: 6
                    anchors.right: serialbox_text4.left
                    anchors.rightMargin: -2
                    anchors.verticalCenter: parent.verticalCenter

                    text : qsTr("-")

                    font.pixelSize: 15
                    font.family: mainFont.name
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "#0DA3B2"
                }

                TextField {
                    id: serialbox_text4
                    width: (parent.width-100)/4
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: TextInput.AlignRight
                    placeholderText: "AAAA"
                    font.pixelSize: 12
                    font.family: mainFont.name
                    font.capitalization: Font.AllUppercase
                    textColor: focus ? "black" : "#737375"
                    selectByMouse: true
                    maximumLength : 4

                    style: TextFieldStyle {
                        textColor: "black"
                        background: Rectangle {
                            border.width: 0
                        }
                    }
                }

                Rectangle{
                    width: parent.width
                    height: 1

                    anchors.left: parent.left
                    anchors.top: parent.bottom
                    color: "#939396"
                }
            }
        }

        /*
        //PW Box
        Item {
            id: pwboxItem
            width: 284
            height: 28
            anchors.top: parent.top
            anchors.left : parent.left
            anchors.topMargin: 270
            anchors.leftMargin: 40
            //number box
            Rectangle {
                id: pwbox
                width: parent.width
                height: parent.height
                color: "transparent"
                Text{
                    width: parent.width
                    height: parent.height
                    anchors.centerIn: parent

                    text : qsTr("Password")

                    font.pixelSize: 20
                    font.family: mainFont.name
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    color: "#0DA3B2"
                }
                TextField {
                    id: pwbox_text
                    width: parent.width-90
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: TextInput.AlignRight
                    placeholderText: "Enter Password"

                    font.pixelSize: 12
                    font.family: mainFont.name
                    textColor: focus ? "black" : "#737375"
                    selectByMouse: true
                    echoMode: TextInput.Password

                    style: TextFieldStyle {
                        textColor: "black"
                        background: Rectangle {
                            border.width: 0
                        }
                    }
                }
                Rectangle{
                    width: parent.width
                    height: 1

                    anchors.left: parent.left
                    anchors.top: parent.bottom
                    color: "#939396"
                }
            }
        }
        */

        Rectangle { // login button
            id : loginButton
            objectName: "loginButton"
            width: 128
            height: 36
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.leftMargin: 36
            anchors.bottomMargin: 36
            color: "transparent"
            Image {
                anchors.fill: parent
                source: "qrc:/Resource/login_leftbutton.png"
            }
            Text{
                width: parent.width
                height: parent.height
                anchors.centerIn: parent

                text : "LOG IN"

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
                    parent.loginTrial(serialbox_text1.text + serialbox_text2.text + serialbox_text3.text + serialbox_text4.text);
                }
            }
            signal loginTrial(string serial);

            function loginSuccess(){
                loginWindow.close()
                window.visible = true;
            }
            function loginFail(){
                loginErrorText.visible = true;
            }
        }

        Rectangle { // cancel button
            width: 128
            height: 36
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.rightMargin: 36
            anchors.bottomMargin: 36
            color: "transparent"
            Image {
                anchors.fill: parent
                source: "qrc:/Resource/login_rightbutton.png"
            }
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
            overlay.iconSource: "qrc:/icon-32.ico"
    }




}
