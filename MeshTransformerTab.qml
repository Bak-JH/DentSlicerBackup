import QtQuick 2.0

Item {
    width : 36
    height: 168


    Image {
        id: model_move
        width: 36
        height: 36

        anchors.top : parent.top
        anchors.left: parent.left
        anchors.topMargin: 8

        source: "qrc:/resource/model_move.png"
    }

    Image {
        id: model_rotate
        width: 36
        height: 36

        anchors.top : model_move.bottom
        anchors.left: parent.left
        anchors.topMargin: 8

        source: "qrc:/resource/model_rotate.png"
    }

    Image {
        id: model_zoom
        width: 36
        height: 36

        anchors.top : model_rotate.bottom
        anchors.left: parent.left
        anchors.topMargin: 8

        source: "qrc:/resource/model_zoom.png"
    }

    Image {
        id: model_delete

        width: 36
        height: 36

        anchors.top : model_zoom.bottom
        anchors.left: parent.left
        anchors.topMargin: 8

        source: "qrc:/resource/model_delete.png"
    }

}
