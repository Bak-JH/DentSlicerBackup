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

        MouseArea {
            anchors.fill: parent
            onClicked: {
                deselectAll()
                selectMove()


            }
        }
    }

    Image {
        id: model_rotate
        width: 36
        height: 36

        anchors.top : model_move.bottom
        anchors.left: parent.left
        anchors.topMargin: 8

        source: "qrc:/resource/model_rotate.png"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                deselectAll()
                selectRotate()

                ui.isRotate = true
            }
        }
    }

    Image {
        id: model_zoom
        width: 36
        height: 36

        anchors.top : model_rotate.bottom
        anchors.left: parent.left
        anchors.topMargin: 8

        source: "qrc:/resource/model_zoom.png"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                deselectAll()
                selectZoom()

                ui.isZoom = true
            }
        }
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

    function show(w,h){
        ui.world2Screen()

        var result = Qt.vector2d(0,0)

        result.x = w + scene3d.anchors.leftMargin
        result.y = h + scene3d.anchors.topMargin


        setPosition(result.x,result.y)
    }

    function setPosition(w, h){
        anchors.leftMargin = w
        anchors.topMargin = h
    }
    function deselectAll(){
        isMove = false
        isRotate = false
        isZoom = false


        model_move.source = "qrc:/resource/model_move.png"
        model_rotate.source = "qrc:/resource/model_rotate.png"
        model_zoom.source = "qrc:/resource/model_zoom.png"

    }

    function selectMove(){
        model_move.source = "qrc:/resource/model_move_select.png"

        var point = ui.world2Screen(Qt.vector3d(0,0,0))
        ui.isMove = true
        ui.mtm.show(point.x,point.y)
    }
    function selectRotate(){
        model_rotate.source = "qrc:/resource/model_rotate_select.png"

        var point = ui.world2Screen(Qt.vector3d(0,0,0))
        ui.isRotate = true
        ui.mtr.show(point.x,point.y)
    }
    function selectZoom(){
        model_zoom.source = "qrc:/resource/model_zoom_select.png"

        var point = ui.world2Screen(Qt.vector3d(0,0,0))
        ui.isZoom = true
        ui.mtz.show(point.x,point.y)
    }


}

