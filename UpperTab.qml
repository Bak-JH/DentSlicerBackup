import Qt3D.Core 2.0

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Rectangle {
    color: "#E2E1E1"
    property int buttonWidth : 86
    property int buttonHeight : 100


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

    Rectangle{
        id : tabgroupname
        height: 16
        //anchors.fill: parent

        anchors.left : parent.left
        anchors.right : parent.right
        anchors.bottom: bottomine.top
        color: "#CECECE"
    }

    Rectangle{
        id : firsttab
        width : buttonWidth * 3 + 2
        height: parent.height

        anchors.left : parent.left
        anchors.top : parent.top
        anchors.bottom : parent.bottom

        color: "transparent"


        Rectangle{
            id : first_tab_right_line
            width: 2
            height: buttonHeight

            anchors.right: parent.right
            anchors.top : parent.top
            anchors.bottom : tabgroupname.top

            color : "#CECECE"
        }
    }

    Item{
        id : secondtab
        width : buttonWidth * 5 + 2
        height: parent.height

        anchors.left : firsttab.right
        anchors.top : parent.top
        anchors.bottom : parent.bottom
        /*
        Rectangle{
            id : second_tab_right_line
            width: 2
            height: buttonHeight

            anchors.right: parent.right
            anchors.top : parent.top
            anchors.bottom : tabgroupname.top

            color : "#CECECE"
        }
        */
    }
    /*
    Rectangle{
        id : thirdtab
        width : buttonWidth * 4 + 2
        height: parent.height

        anchors.left : secondtab.right
        anchors.top : parent.top
        anchors.bottom : parent.bottom
        //color: "blue"
    }

    Rectangle{
        id : fourthtab
        width : buttonWidth * 3 + 2
        height: parent.height

        anchors.left : thirdtab.right
        anchors.top : parent.top
        anchors.bottom : parent.bottom
        //color: "green"
    }
    */



}
