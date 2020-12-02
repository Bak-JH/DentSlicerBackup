//import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Scene3D 2.0
import QtQuick.Window 2.2
//import QtCanvas3D 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2


import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick 2.7
//import GLQML 1.0


import Qt3D.Logic 2.0

Entity {
    property vector3d xup : Qt.vector3d( 1.0, 0.0, 0.0 )
    property vector3d xdown : Qt.vector3d( -1.0, 0.0, 0.0 )

    property vector3d yup : Qt.vector3d( 0.0, 1.0, 0.0 )
    property vector3d ydown : Qt.vector3d( 0.0, -1.0, 0.0 )
    property vector3d zup : Qt.vector3d( 0.0, 0.0, 1.0 )
    property vector3d zdown : Qt.vector3d( 0.0, 0.0, -1.0 )

    // property alias total: total
    property alias cm: cm
    property alias systemTransform: systemTransform

    property int ftrDelete : 18
    signal cameraViewChanged();

    CameraManager{
        objectName: "cm"
        id : cm
        onCameraRotated:
        {
            cameraViewChanged();
        }
        function updateCameraView()
        {
            cameraViewChanged();

        }
    }


    Entity{
        id : total
        objectName:"total"
        Transform{
            id: systemTransform
            scale3D: Qt.vector3d(0.004,0.004,0.004)
            rotationX : -70
            rotationY : 0
            rotationZ : -40
        }

        components: [systemTransform]

        /* Grid Mesh */

        // PhongMaterial{
        //     id : meshMaterial
        //     ambient: Qt.rgba(100/255, 100/255, 100/255, 0)
        //     diffuse: Qt.rgba(100/255, 100/255, 100/255, 0)
        //     specular: Qt.rgba(100/255, 100/255, 100/255, 0)
        //     shininess: 0
        // }
		Entity{
            id: models
            //inputSource:"file:///C:/Users/user/Downloads/Spider_ascii.stl"
        }
        CoordinateMesh{}

    }

    function axisAngle2Quaternion(angle, axis){
        var result = Qt.quaternion(0,0,0,0);
        result.x = axis.x * Math.sin(angle/2)
        result.y = axis.y * Math.sin(angle/2)
        result.z = axis.z * Math.sin(angle/2)
        result.scalar = Math.cos(angle/2)

        return result
    }

    function multiplyQuaternion(a,b)
    {
        var result = Qt.quaternion(0,0,0,0);
        result.x = a.x*b.scalar + a.scalar*b.x + a.y*b.z - a.z*b.y
        result.y = a.y*b.scalar + a.scalar*b.y + a.z*b.x - a.x*b.z
        result.z = a.z*b.scalar + a.scalar*b.z + a.x*b.y - a.y*b.x
        result.scalar = a.scalar*b.scalar - a.x*b.x - a.y*b.y - a.z*b.z

        return result
    }

    function initCamera(){
        cm.initCamera()
    }

    function viewUp(){
        systemTransform.scale3D = Qt.vector3d(0.004,0.004,0.004)
        systemTransform.rotationX = -180
        systemTransform.rotationY = 0
        systemTransform.rotationZ = 0
        cm.camera.translateWorld(cm.camera.viewCenter.times(-1))
        cm.camera.translateWorld(Qt.vector3d(0.025,-0.25,0))
        cm.updateCameraView();
    }
    function viewDown(){
        systemTransform.scale3D = Qt.vector3d(0.004,0.004,0.004)
        systemTransform.rotationX = 0
        systemTransform.rotationY = 0
        systemTransform.rotationZ = 0
        cm.camera.translateWorld(cm.camera.viewCenter.times(-1))
        cm.camera.translateWorld(Qt.vector3d(0.025,-0.25,0))
        cm.updateCameraView();
    }
    function viewFront(){
        systemTransform.scale3D = Qt.vector3d(0.004,0.004,0.004)
        systemTransform.rotationX = -90
        systemTransform.rotationY = 0
        systemTransform.rotationZ = 0;
        cm.camera.translateWorld(cm.camera.viewCenter.times(-1))
        cm.camera.translateWorld(Qt.vector3d(0.02,-0.06,0))
        cm.updateCameraView();
    }
    function viewBack(){
        systemTransform.scale3D = Qt.vector3d(0.004,0.004,0.004)
        systemTransform.rotationX = -90
        systemTransform.rotationY = 0
        systemTransform.rotationZ = -180
        cm.camera.translateWorld(cm.camera.viewCenter.times(-1))
        cm.camera.translateWorld(Qt.vector3d(0.02,-0.06,0))
        cm.updateCameraView();
    }
    function viewLeft(){
        systemTransform.scale3D = Qt.vector3d(0.004,0.004,0.004)
        systemTransform.rotationX = -90
        systemTransform.rotationY = 0
        systemTransform.rotationZ = 90;
        cm.camera.translateWorld(cm.camera.viewCenter.times(-1))
        cm.camera.translateWorld(Qt.vector3d(0.015,-0.04,0))
        cm.updateCameraView();
    }
    function viewRight(){
        systemTransform.scale3D = Qt.vector3d(0.004,0.004,0.004)
        systemTransform.rotationX = -90
        systemTransform.rotationY = 0
        systemTransform.rotationZ = -90;
        cm.camera.translateWorld(cm.camera.viewCenter.times(-1))
        cm.camera.translateWorld(Qt.vector3d(0.015,-0.04,0))
        cm.updateCameraView();
    }

    function viewCenter(){
        systemTransform.scale3D = Qt.vector3d(0.004,0.004,0.004)
        systemTransform.rotationX = -70
        systemTransform.rotationY = 0
        systemTransform.rotationZ = -40
        cm.camera.translateWorld(cm.camera.viewCenter.times(-1))
        cm.camera.translateWorld(Qt.vector3d(-0.015,-0.16,0))
        cm.updateCameraView();
    }

}
