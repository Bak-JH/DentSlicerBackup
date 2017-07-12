import Qt3D.Core 2.0

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Item {
    height : parent.height - 120
    width : 260
    /*
    Rectangle{
        anchors.fill: parent
        color: "#E2E1E1"


    }*/
    Rectangle{
        id: rightline
        width: 2
        anchors.right : parent.right
        anchors.top : parent.top
        anchors.bottom: parent.bottom

        color: "#CECECE"
    }


}
