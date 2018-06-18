import QtQuick 2.0

Item {
    /*
    property alias mttab: mttab
    property alias mtz: mtz
    //property alias mtm: mtm
    property alias mtr: mtr
    property alias slicing_data:slicing_data

    //property bool isTab : true
    property bool isZoom : false
    property bool isMove : false
    property bool isRotate : false

    MeshTransformerTab{
        id:mttab
        objectName: "mttab"
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
    */

    UndoRedoButton{
        id: undoRedoButton
        objectName: "undoRedoButton"
        anchors.top : parent.top
        anchors.right : parent.right
        anchors.topMargin: 32
        anchors.rightMargin: 86
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
        /*
    //not working old version
        //var point = Qt.vector3d(0,0,0);
        var point = target
        var matrix = Qt.matrix4x4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
        matrix = sceneRoot.cm.camera.projectionMatrix.times(sceneRoot.cm.camera.viewMatrix);

        point = matrix.times(point)

        point.x = (point.x+1) * scene3d.width/2;
        point.y = (-1 * point.y+1) * scene3d.height/2;

        return Qt.vector2d(point.x,point.y)
        */
        //var point = Qt.vector3d(0,0,0);
        var tmp = Qt.vector3d(0,0,0)
        tmp = target
        target = tmp.times(sceneRoot.systemTransform.scale3D)

        var theta = (-1)*sceneRoot.systemTransform.rotationX/180.0*Math.PI
        var alpha = (-1)*sceneRoot.systemTransform.rotationZ/180.0*Math.PI
        //console.log("cordinate")
        //console.log(theta,alpha)
        //console.log(sceneRoot.systemTransform.rotationX,sceneRoot.systemTransform.rotationZ)
        //console.log(target.x,target.y,target.z)

        target = Qt.vector3d(target.x * Math.cos(alpha)+target.y*Math.sin(alpha),
                             target.x * (-1) * Math.sin(alpha) + target.y*Math.cos(alpha),
                             target.z)
        var target3 = Qt.vector3d(target.x,
                             target.y * Math.cos(theta)+target.z*Math.sin(theta),
                             (-1)*target.y*Math.sin(theta)+target.z*Math.cos(theta))
        console.log(target3.x,target3.y,target.z)
        var point2 = target3
        var matrix = Qt.matrix4x4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
        matrix = sceneRoot.cm.camera.projectionMatrix.times(sceneRoot.cm.camera.viewMatrix);//
        point2 = matrix.times(point2)
        point2.x = (point2.x+1) * scene3d.width/2;
        point2.y = (-1 * point2.y+1) * scene3d.height/2;
        return Qt.vector2d(point2.x,point2.y)




    }



}
