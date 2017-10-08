import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    property string inputSource;

    property alias testMesh: testMesh
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

        rotation: objectRotation
        translation: Qt.vector3d(0,0,0);
    }

    ObjectPicker{
        id : picker
        onClicked: {
            console.log("click")
            //var globalCoordinares = ttt.mapToItem(ttt.parent, 0, 0);
            console.log(ttt.parent.id);
            //console.log("X: " + globalCoordinares.x + " y: " + globalCoordinares.y);

            var point = Qt.vector3d(0,0,0);
            var matrix = Qt.matrix4x4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);




            matrix = cm.camera.projectionMatrix.times(cm.camera.viewMatrix);

            point = matrix.times(point)

            point.x = (point.x+1) * scene3d.width/2;
            point.y = (-1 * point.y+1) * scene3d.height/2;


            console.log("point2 " + point);
            console.log("viwport  w" + scene3d.width);
            console.log("viwport  h" + scene3d.height);

            rt.anchors.leftMargin = point.x
            rt.anchors.topMargin = point.y
        }
    }
    PhongMaterial {
        id: testMaterial
        property int counter : 0
        ambient: Qt.rgba(81/255,200/255,242/255,1.0)

    }

    components: [ testMesh, testMaterial, meshTransform,picker ]
    //components: [ testMesh, testMaterial, meshTransform]


}
