import QtQuick 2.0

Item {
    property alias mttab: mttab
    property alias mtz: mtz
    property alias mtm: mtm
    property alias mtr: mtr
    property alias slicing_data:slicing_data

    property bool isTab : false
    property bool isZoom : false
    property bool isMove : false
    property bool isRotate : false

    MeshTransformerTab{
        id:mttab
        anchors.left : parent.left
        anchors.top : parent.top

        anchors.leftMargin: 400
        anchors.topMargin: 400

        visible: isTab
    }

    MeshTransformerZoom{
        id : mtz

        anchors.top: parent.top
        anchors.left: parent.left

        anchors.topMargin: 0
        anchors.leftMargin: 0
        visible: isZoom
    }

    MeshTransformerMove{
        id : mtm

        anchors.top: parent.top
        anchors.left: parent.left

        anchors.topMargin: 0
        anchors.leftMargin: 0
        visible: isMove
    }
    MeshTransformerRotateTest{
        id : mtr

        anchors.top: parent.top
        anchors.left: parent.left

        anchors.topMargin: 100
        anchors.leftMargin: 100
        visible: isRotate
    }



    ViewChange{
        anchors.top : parent.top
        anchors.right : parent.right
        anchors.topMargin: 32
        anchors.rightMargin: 32
    }

    SlicingData{
        id : slicing_data
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        anchors.rightMargin: 22
        anchors.bottomMargin: 22
    }


    function world2Screen(target){

        //var point = Qt.vector3d(0,0,0);
        var point = target
        var matrix = Qt.matrix4x4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
        matrix = sceneRoot.cm.camera.projectionMatrix.times(sceneRoot.cm.camera.viewMatrix);

        point = matrix.times(point)

        point.x = (point.x+1) * scene3d.width/2;
        point.y = (-1 * point.y+1) * scene3d.height/2;

        return Qt.vector2d(point.x+100,point.y-50)

        /*
        console.log("point2 " + point);
        console.log("viwport  w" + scene3d.width);
        console.log("viwport  h" + scene3d.height);
        */
    }



}
