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
    property int pastAxis :0
    components: [
    Transform {
        id:rotateSphereTransform
        translation:center
    }
    ]
    Entity {
        id: sphere
        objectName: "rotateSphereSphere"
        SphereMesh{
            id: sphere1
            radius : 7
        }
        Transform {
            id: sphere1Transform
            translation: Qt.vector3d( 0,0,0 )
            scale3D: inputScale
        }
        PhongAlphaMaterial{
            id: sphere1Material
            ambient: Qt.rgba(255/255,255/255,255/255,0.5)
        }

        components: [ sphere1, sphere1Material, sphere1Transform]
    }
    Entity {
        id: torusX
        objectName: "rotateSphereTorusX"
        TorusMesh{
            id: torusXMesh
            radius : 7
            minorRadius: 0.35
            rings: 50
            slices: 10
        }
        Transform {
            id: torusXTransform
            translation: Qt.vector3d( 0,0,0 )
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
             onEntered: {
                 if (rotateAxis == 0){
                 torusXMaterial.ambient = Qt.rgba(0/255,125/255,125/255,0.5)
                 torusYMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.5)
                 torusZMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.5)
                 }
             }
             onExited: {
                 if (rotateAxis == 0){
                torusXMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.5) //rotation end
                 }
             }
             onPressed: { //rotation start
                 var point = world2Screen(Qt.vector3d(0,0,0))
                 mouseOrigin = Qt.vector2d(pick.position.x , pick.position.y)
                 if ( rotateAxis  == 0){
                     rotateAxis = 1 //
                    torusXMaterial.ambient = Qt.rgba(0/255,255/255,255/255,1.0)
                    pastAngle=0;
                 }
             }
             onReleased: {
                 rotateAxis = 0;
                 console.log("releaseX")
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
            radius : 7
            minorRadius: 0.35
            rings: 50
            slices: 10
        }
        Transform {
            id: torusYTransform
            translation: Qt.vector3d( 0,0,0 )
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

             onEntered: {
                 if (rotateAxis == 0){
                 torusYMaterial.ambient = Qt.rgba(125/255,125/255,0/255,0.5)
                 torusXMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.5)
                 torusZMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.5)

                 }
             }
             onExited: {
                 if (rotateAxis == 0){
                     torusYMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.5) //rotation end
                 }
             }
             onPressed: { //rotation start
                 var point = world2Screen(Qt.vector3d(0,0,0))
                 mouseOrigin = Qt.vector2d(pick.position.x , pick.position.y)
                 if ( rotateAxis  == 0){
                    rotateAxis = 2 //
                    torusYMaterial.ambient = Qt.rgba(255/255,255/255,0/255,1.0)
                    pastAngle=0;
                 }
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
            radius : 7
            minorRadius: 0.35
            rings: 50
            slices: 10
        }
        Transform {
            id: torusZTransform
            translation: Qt.vector3d( 0,0,0 )
            scale3D: inputScale
        }
        PhongAlphaMaterial {
            id: torusZMaterial
            ambient: Qt.rgba(0/255,0/255,0/255,0.5)
        }
        ObjectPicker{
            id : objectPickerZ
            hoverEnabled: true
            onEntered: {
                if (rotateAxis == 0){
                    torusZMaterial.ambient = Qt.rgba(125/255,0/255,125/255,0.5)
                    torusXMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.5)
                    torusYMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.5)
                }
            }
            onExited: {
                if (rotateAxis == 0){
                    torusZMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.5) //rotation end
                }
            }
            onPressed: { //rotation start
                var point = world2Screen(Qt.vector3d(0,0,0))
                mouseOrigin = Qt.vector2d(pick.position.x , pick.position.y)
                if ( rotateAxis  == 0 ) {
                    rotateAxis = 3 //
                    torusZMaterial.ambient = Qt.rgba(255/255,0/255,255/255,1.0)
                    pastAngle=0;
                }
            }
            onReleased: {
                rotateAxis = 0;
                torusZMaterial.ambient = Qt.rgba(0/255,0/255,0/255,0.5) //rotation end
            }
         }
        components: [ torusZMesh, torusZMaterial, torusZTransform,objectPickerZ]
    }


    MouseDevice{
        id: rotationmousedevice
        sensitivity: 0.01
    }
    MouseHandler{
        id : rotationMouseHandler
        sourceDevice: rotationmousedevice
        onClicked: {
            if (mouse.buttons == MouseEvent.RightButton){
                rotateAxis = 0;
            }
        }
        onReleased: {
            if (mouse.buttons == MouseEvent.LeftButton){
                torusXMaterial.ambient = Qt.rgba(255/255,0/255,255/255,0.5)
                torusYMaterial.ambient = Qt.rgba(255/255,0/255,255/255,0.5)
                torusZMaterial.ambient = Qt.rgba(255/255,0/255,255/255,0.5)
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
        var tmp = Qt.vector3d(0,0,0)
        tmp = target
        target = tmp.times(systemTransform.scale3D)
        console.log(target)
        var point = target
        var matrix = Qt.matrix4x4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
        matrix = sceneRoot.cm.camera.projectionMatrix.times(sceneRoot.cm.camera.viewMatrix);
        point = matrix.times(point)
        point.x = (point.x+1) * scene3d.width/2;
        point.y = (-1 * point.y+1) * scene3d.height/2;
        return Qt.vector2d(point.x,point.y)
    }
    signal rotateSignal(int Axis, int angle)
    signal rotateDone(int Axis)
    FrameAction{
        id : rotationFrameAction
        onTriggered: {
            if (parent.parent.enabled.toString()=="true"){
                var syszoom = Qt.vector3d(0,0,0)
                syszoom = sceneRoot.systemTransform.scale3D
                rotateSphereTransform.scale3D = Qt.vector3d(0.01/syszoom.x,0.01/syszoom.y,0.01/syszoom.z)
                rotateSphereTransform.translation = center.minus(cm.camera.viewVector)
                //console.log("camera")
                if (rotateAxis == 0 && pastAxis != 0){
                    rotateDone(pastAxis);
                    pastAxis = 0;
                }
                //console.log(center)
                if (rotateAxis != 0){
                    //console.log(cm.camera.viewCenter)
                    //console.log("start")
                    var Origin = world2Screen(center)
                    //console.log("mouse location")
                    //console.log(center)
                    //console.log(Origin)
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
                    pastAxis = rotateAxis
                    pastAngle = degreeangle
                    console.log(degreeangle)

                }
            }
        }
    }
}
