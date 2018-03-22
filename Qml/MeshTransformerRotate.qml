import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import Qt3D.Logic 2.0

Entity {
    id : rotateSphere
    property vector3d center : Qt.vector3d( 0,0,0 )
    //property vector3d center : Qt.vector3d( 20 ,20,20 )
    property vector3d inputScale : Qt.vector3d(1,1,1)
    property int rotateAxis : 0 //0:none, 1:x, 2:y, 3:z
    property vector2d mouseOrigin : Qt.vector2d(0,0)
    property vector2d mouseCurrent : Qt.vector2d(0,0)
    property int pastAngle :0
    Transform {
        rotation : fromAxisAndAngle(Qt.vector3d(1,0, 1), 60)
    }
    Entity {
        id: sphere
        objectName: "rotateSphereSphere"
        SphereMesh{
            id: sphere1
            radius : 14
        }
        Transform {
            id: sphere1Transform
            translation: center
            scale3D: inputScale
        }
        PhongAlphaMaterial{
            id: sphere1Material
            ambient: Qt.rgba(255/255,255/255,255/255,1.0)
            alpha: 1
        }

        components: [ sphere1, sphere1Material, sphere1Transform]
    }
    Entity {
        id: torusX
        objectName: "rotateSphereTorusX"
        TorusMesh{
            id: torusXMesh
            radius : 14.1
            minorRadius: 0.8
            rings: 100
            slices: 20
        }
        Transform {
            id: torusXTransform
            translation: center
            scale3D: Qt.vector3d(1,1,1)
            rotation: fromAxisAndAngle(Qt.vector3d(0,1, 0), 90)
        }
        PhongAlphaMaterial {
            id: torusXMaterial
            ambient: Qt.rgba(0/255,0/255,0/255,0.5)
        }
        ObjectPicker{
             id : objectPickerX
             dragEnabled: true
             hoverEnabled: true
             onPressed: { //rotation start
                 var point = world2Screen(Qt.vector3d(0,0,0))
                 mouseOrigin = Qt.vector2d(pick.position.x , pick.position.y)
                 if ( rotateAxis  == 0) rotateAxis = 1 //
                 torusXMaterial.ambient = Qt.rgba(0/255,255/255,255/255,1.0)
                 pastAngle=0;
             }
             onReleased: {
                 rotateAxis = 0;
                 torusXMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.5) //rotation end
             }
         }
        components: [ torusXMesh, torusXMaterial, torusXTransform,objectPickerX]
    }
    Entity {
        id: torusY
        objectName: "rotateSphereTorusY"
        TorusMesh{
            id: torusYMesh
            radius : 14.1
            minorRadius: 0.8
            rings: 100
            slices: 20
        }
        Transform {
            id: torusYTransform
            translation: center
            scale3D: inputScale
            rotation:fromAxisAndAngle(Qt.vector3d(1,0, 0), 90)
        }
        PhongAlphaMaterial {
            id: torusYMaterial
            ambient: Qt.rgba(0/255,0/255,0/255,0.5)
        }

        ObjectPicker{
            id : objectPickerY
             hoverEnabled: true
             onPressed: { //rotation start
                 var point = world2Screen(Qt.vector3d(0,0,0))
                 mouseOrigin = Qt.vector2d(pick.position.x , pick.position.y)
                 if ( rotateAxis  == 0) rotateAxis = 2 //
                 torusYMaterial.ambient = Qt.rgba(0/255,255/255,255/255,1.0)
                 pastAngle=0;
             }
             onReleased: {
                 rotateAxis = 0;
                 torusYMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.5) //rotation end
             }
         }
        components: [ torusYMesh, torusYMaterial, torusYTransform, objectPickerY]
    }

    Entity {
        id: torusZ
        objectName: "rotateSphereTorusZ"
        TorusMesh{
            id: torusZMesh
            radius : 14.1
            minorRadius: 0.8
            rings: 100
            slices: 20
        }
        Transform {
            id: torusZTransform
            translation: center
            scale3D: inputScale
        }
        PhongAlphaMaterial {
            id: torusZMaterial
            ambient: Qt.rgba(0/255,0/255,0/255,0.5)
        }
        ObjectPicker{
            id : objectPickerZ
            hoverEnabled: true
            onPressed: { //rotation start
                var point = world2Screen(Qt.vector3d(0,0,0))
                mouseOrigin = Qt.vector2d(pick.position.x , pick.position.y)
                if ( rotateAxis  == 0) rotateAxis = 3 //
                torusZMaterial.ambient = Qt.rgba(0/255,255/255,255/255,1.0)
                pastAngle=0;
            }
            onReleased: {
                rotateAxis = 0;
                torusZMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.5) //rotation end
            }
         }
        components: [ torusZMesh, torusZMaterial, torusZTransform,objectPickerZ]
    }

    function perfectPosition(){
        var temp, cameraP, cameraV ;
        cameraP = sceneRoot.cm.camera.position;
        cameraV = sceneRoot.cm.camera.viewCenter;
        temp = cameraP.minus(cameraV).normalized()
        center = cameraP.minus(temp.times(400))
        //sceneRoot.total.mtr.center = camera.position.minus(temp.times(400))
    }
    MouseDevice{
        id: rotationmousedevice
        sensitivity: 0.1
    }
    MouseHandler{
        id : rotationMouseHandler
        sourceDevice: rotationmousedevice
        onClicked: {
            if (mouse.buttons == MouseEvent.RightButton){
                rotateAxis = 0;
            }
        }
        onPositionChanged: {
            //console.log(mouse.x,mouse.y)
            mouseCurrent = Qt.vector2d(mouse.x,mouse.y)
        }
    }
    function vertexccw(x1,y1,x2,y2,x3,y3){
        return (x1*y2+x2*y3+x3*y1) - (x1*y3+x2*y1+x3*y2)
    }
    function world2Screen(target){
        //var point = Qt.vector3d(0,0,0);
        var point = target
        var matrix = Qt.matrix4x4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
        matrix = sceneRoot.cm.camera.projectionMatrix.times(sceneRoot.cm.camera.viewMatrix);
        point = matrix.times(point)
        point.x = (point.x+1) * scene3d.width/2;
        point.y = (-1 * point.y+1) * scene3d.height/2;
        return Qt.vector2d(point.x,point.y)
    }


    signal rotateSignal(int Axis, int angle)
    FrameAction{
        id : rotationFrameAction
        onTriggered: {
            var Origin = ui.world2Screen(Qt.vector3d(0,0,0))
            //console.log("mouse location")
            var a = Qt.vector2d(0,0)
            var b = Qt.vector2d(1,1)
            var ccwvar = vertexccw(mouseOrigin.x,mouseOrigin.y,Origin.x,Origin.y,mouseCurrent.x,mouseCurrent.y)
            var mouseOrigin_Origin = mouseOrigin.minus(Origin).length()
            var mouseCurrent_Origin = mouseCurrent.minus(Origin).length()
            var mouseCurrent_mouseOrigin = mouseOrigin.minus(mouseCurrent).length()
            var sinangle = ccwvar/(mouseOrigin_Origin * mouseCurrent_Origin)
            var degreeangle = Math.asin(sinangle) * 180 / Math.PI
            if (Math.pow(mouseCurrent_mouseOrigin,2) > Math.pow(mouseOrigin_Origin,2)+Math.pow(mouseCurrent_Origin,2)){
                degreeangle = (Math.abs(degreeangle)/degreeangle)*(180-Math.abs(degreeangle))
            }

            switch(rotateAxis){
            case 0: //cancle rotation
                break;
            case 1: // xAxis
                rotateSignal(1,degreeangle-pastAngle)
                break;
            case 2: // yAxis
                rotateSignal(2,degreeangle-pastAngle)
            break;
            case 3: // zAxis
                rotateSignal(3,degreeangle-pastAngle)
            break;
            }
            pastAngle = degreeangle

        }
    }
}
