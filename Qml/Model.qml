import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {

    objectName: "Models"
    property string inputSource;

    //property alias testMesh: testMesh
    property alias meshTransform: meshTransform

    Transform {
        id: meshTransform
        property quaternion objectRotation : fromAxisAndAngle(Qt.vector3d(0,0, 1), 60)

        scale3D: Qt.vector3d(1,1,1)

        //rotation: objectRotation
        translation: Qt.vector3d(0,0,0);
    }


    components: [meshTransform]


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
