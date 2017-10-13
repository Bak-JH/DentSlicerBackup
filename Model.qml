import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    property string inputSource;

    property alias testMesh: testMesh
    property alias meshTransform: meshTransform
    id : ttt
    Mesh{
        id: testMesh
        source: inputSource
        //source: "file:///D:/Dev/Trash/3Dtest/full.stl"
    }

    Transform {
        id: meshTransform
        property quaternion objectRotation : fromAxisAndAngle(Qt.vector3d(0,0, 1), 60)

        scale3D: Qt.vector3d(1,1,1)

        //rotation: objectRotation
        translation: Qt.vector3d(0,0,0);
    }

    ObjectPicker{
        id : picker
        onClicked: {
            /*
            console.log("click")

            var point = Qt.vector3d(0,0,0);
            var matrix = Qt.matrix4x4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);

            matrix = cm.camera.projectionMatrix.times(cm.camera.viewMatrix);

            point = matrix.times(point)

            point.x = (point.x+1) * scene3d.width/2;
            point.y = (-1 * point.y+1) * scene3d.height/2;


            console.log("point2 " + point);
            console.log("viwport  w" + scene3d.width);
            console.log("viwport  h" + scene3d.height);

            //ui.mttab.anchors.leftMargin = point.x
            //ui.mttab.anchors.topMargin = point.y
            */

            var point = ui.world2Screen(Qt.vector3d(0,0,0))

            ui.isTab = true
            ui.mttab.show(point.x,point.y)


        }
    }
    PhongMaterial {
        id: testMaterial
        property int counter : 0
        ambient: Qt.rgba(81/255,200/255,242/255,1.0)

    }

    components: [ testMesh, testMaterial, meshTransform,picker ]
    //components: [ testMesh, testMaterial, meshTransform]

    function zoomUp(){
        meshTransform.scale3D = meshTransform.scale3D.plus(Qt.vector3d(0.01,0.01,0.01))
    }
    function zoomDown(){
        meshTransform.scale3D = meshTransform.scale3D.minus(Qt.vector3d(0.01,0.01,0.01))
    }

    function moveXPlus(){
        meshTransform.translation = meshTransform.translation.plus(Qt.vector3d(0.0,0.00,0.1))
    }
    function moveXMinus(){
        meshTransform.translation = meshTransform.translation.minus(Qt.vector3d(0.0,0.00,0.1))
    }

    function moveYPlus(){
        meshTransform.translation = meshTransform.translation.plus(Qt.vector3d(0.1,0.0,0.0))
    }
    function moveYMinus(){
        meshTransform.translation = meshTransform.translation.minus(Qt.vector3d(0.1,0.0,0.0))
    }
}
