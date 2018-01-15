import QtQuick 2.0


Rectangle {
    property alias funcname: funcname.text
    property alias detail1: detail1.text
    property alias image: image.source
    property alias detailline_vis: detailline.visible
    property alias applybutton_vis: applybutton.visible
    property alias descriptionimage_vis: descriptionimage.visible
    property alias applyfinishbutton_vis: applyfinishbutton.visible
    //property alias popup_anchor_left: popup.left
    //property alias popup_anchor_top: popup.top
    id: popup
    visible: false
    width: 244
    height: 100
    anchors.left: parent.left
    anchors.top: parent.bottom
    anchors.leftMargin: 20
    anchors.topMargin: 20
    color: "#e5e5e5"
    border.width: 1
    border.color: "#cccccc"

    states: [
        State{
            name:"inactive"; when: parent.state=="inactive"
            PropertyChanges { target: popup;  visible: false }
        },
        State{
            name:"active"; when: parent.state=="active"
            PropertyChanges { target: popup;  visible: true }
        }
    ]
   // state: "inactive"


    //function name
    Text {
        id: funcname
        anchors.top: parent.top
        anchors.topMargin: 8
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.pointSize: 10.5
        color: "#494949"
        font.family: "Arial"
    }

    //detail name
    Text {
        id: detail1
        anchors.top: funcname.bottom
        anchors.topMargin: 15
        anchors.left: funcname.left
        anchors.leftMargin: 10
        font.pointSize: 9
        color: "#999999"
    }

    //detail name right side line
    Rectangle {
        id: detailline
        width: 195 - detail1.implicitWidth
        height: 1
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.verticalCenter: detail1.verticalCenter
        color: "#b2b2b2"
    }

    //Apply button
    Rectangle {
        id: applybutton
        width: 230
        height: 30.7
        color: "#999999"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 7
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Apply"
            color: "#ffffff"
            font.pointSize: 10
            font.family: "Arial"
        }
    }

    //Description image
    Rectangle {
        id: descriptionimage
        width: 176.6
        height: 75.7
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 13
        color: parent.color
        Image {
            id: image
            width: parent.width
            height: parent.height
            anchors.fill: parent
        }
    }


    //Apply-Finish button
    Rectangle {
        id: applyfinishbutton
        //visible: false
        width: 111.6
        height: 30.7
        color: "#999999"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 7
        anchors.right: parent.horizontalCenter
        anchors.rightMargin: 3.5
        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Apply"
            color: "#ffffff"
            font.pointSize: 10
        }
    }

    Rectangle {
        visible: applyfinishbutton.visible
        width: 111.6
        height: 30.7
        color: "#999999"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 7
        anchors.left: parent.horizontalCenter
        anchors.leftMargin: 3.5
        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Finish"
            color: "#ffffff"
            font.pointSize: 10
        }
    }


}
