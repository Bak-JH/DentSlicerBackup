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

    property color yellow : Qt.rgba(237/255,217/255,41/255,1)
    property color grey : Qt.rgba(150/255,150/255,150/255,1)

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
        PhongMaterial {
            id: torusXMaterial
            ambient: grey
            diffuse: grey
            specular: grey
            shininess: 0
        }
        ObjectPicker{
             id : objectPickerX
             isShareable: false
             hoverEnabled: true
             onEntered: {
                 if (rotateAxis == 0){
                     torusXMaterial.ambient = yellow
                     torusXMaterial.diffuse = yellow
                     torusXMaterial.specular = yellow

                     torusYMaterial.ambient = grey
                     torusYMaterial.diffuse = grey
                     torusYMaterial.specular = grey

                     torusZMaterial.ambient = grey
                     torusZMaterial.diffuse = grey
                     torusZMaterial.specular = grey
                 }
             }
             onExited: {
                 if (rotateAxis == 0){
                     torusXMaterial.ambient = grey
                     torusXMaterial.diffuse = grey
                     torusXMaterial.specular = grey
                 }
             }
             onPressed: { //rotation start
                 if (parent.parent.enabled == true){
                     var point = world2Screen(Qt.vector3d(0,0,0))
                     mouseOrigin = Qt.vector2d(pick.position.x , pick.position.y)
                     if ( rotateAxis  == 0){
                         rotateAxis = 1 //
                         torusY.setEnabled(false)
                         torusZ.setEnabled(false)
                         torusXMaterial.ambient = yellow
                         torusXMaterial.diffuse = yellow
                         torusXMaterial.specular = yellow
                         pastAngle=0;
                     }
                 }
             }
             onReleased: {
                 torusRelease();
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
        PhongMaterial {
            id: torusYMaterial
            ambient: grey
            diffuse: grey
            specular: grey
            shininess: 0
        }

        ObjectPicker{
            id : objectPickerY
             hoverEnabled: true
            isShareable: false
             onEntered: {
                 if (rotateAxis == 0){
                     torusXMaterial.ambient = grey
                     torusXMaterial.diffuse = grey
                     torusXMaterial.specular = grey

                     torusYMaterial.ambient = yellow
                     torusYMaterial.diffuse = yellow
                     torusYMaterial.specular = yellow

                     torusZMaterial.ambient = grey
                     torusZMaterial.diffuse = grey
                     torusZMaterial.specular = grey

                 }
             }
             onExited: {
                 if (rotateAxis == 0){
                     torusYMaterial.ambient = grey
                     torusYMaterial.diffuse = grey
                     torusYMaterial.specular = grey
                 }
             }
             onPressed: { //rotation start
                 if (parent.parent.enabled == true){
                     var point = world2Screen(Qt.vector3d(0,0,0))
                     mouseOrigin = Qt.vector2d(pick.position.x , pick.position.y)
                     if ( rotateAxis  == 0){
                        rotateAxis = 2 //
                         torusX.setEnabled(false)
                         torusZ.setEnabled(false)
                         torusYMaterial.ambient = yellow
                         torusYMaterial.diffuse = yellow
                         torusYMaterial.specular = yellow
                        pastAngle=0;
                     }
                 }
             }
             onReleased: {
                 torusRelease()
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
        PhongMaterial {
            id: torusZMaterial
            ambient: grey
            diffuse: grey
            specular: grey
            shininess: 0
        }
        ObjectPicker{
            id : objectPickerZ
            hoverEnabled: true
            isShareable: false
            onEntered: {
                if (rotateAxis == 0){
                    torusXMaterial.ambient = grey
                    torusXMaterial.diffuse = grey
                    torusXMaterial.specular = grey

                    torusYMaterial.ambient = grey
                    torusYMaterial.diffuse = grey
                    torusYMaterial.specular = grey

                    torusZMaterial.ambient = yellow
                    torusZMaterial.diffuse = yellow
                    torusZMaterial.specular = yellow
                }
            }
            onExited: {
                if (rotateAxis == 0){
                    torusZMaterial.ambient = grey
                    torusZMaterial.diffuse = grey
                    torusZMaterial.specular = grey
                }
            }
            onPressed: { //rotation start
                if (parent.parent.enabled==true){
                    var point = world2Screen(Qt.vector3d(0,0,0))
                    mouseOrigin = Qt.vector2d(pick.position.x , pick.position.y)
                    if ( rotateAxis  == 0 ) {
                        rotateAxis = 3 //
                        torusX.setEnabled(false)
                        torusY.setEnabled(false)
                        torusZMaterial.ambient = yellow
                        torusZMaterial.diffuse = yellow
                        torusZMaterial.specular = yellow
                        pastAngle=0;
                    }
                }
            }
            onReleased: {
                torusRelease();
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
        onPressed: {
            if (mouse.buttons == MouseEvent.RightButton){
                rotateAxis = 0;
            }
        }
        onReleased: {
                torusRelease();
                rotateAxis = 0;
        }
        onPositionChanged: {
            mouseCurrent = Qt.vector2d(mouse.x,mouse.y)
        }
    }
    function torusRelease(){
        if (rotateAxis == 1){
            rotateAxis = 0;
            torusY.setEnabled(true)
            torusZ.setEnabled(true)
            torusXMaterial.ambient = grey
            torusXMaterial.diffuse = grey
            torusXMaterial.specular = grey
        }else if (rotateAxis == 2){
            rotateAxis = 0;
            torusX.setEnabled(true)
            torusZ.setEnabled(true)
            torusYMaterial.ambient = grey
            torusYMaterial.diffuse = grey
            torusYMaterial.specular = grey
        }else if (rotateAxis == 3){
            rotateAxis = 0;
            torusX.setEnabled(true)
            torusY.setEnabled(true)
            torusZMaterial.ambient = grey
            torusZMaterial.diffuse = grey
            torusZMaterial.specular = grey
        }
        mttab.updatePosition();
    }

    function vertexccw(x1,y1,x2,y2,x3,y3){
        return (x1*y2+x2*y3+x3*y1) - (x1*y3+x2*y1+x3*y2)
    }
    function world2Screen(target){
        //var point = Qt.vector3d(0,0,0);
        var tmp = Qt.vector3d(0,0,0)
        tmp = target
        target = tmp.times(systemTransform.scale3D)

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

    signal rotateInit();
    signal rotateSignal(int Axis, int angle)
    signal rotateDone(int Axis)
    FrameAction{
        id : rotationFrameAction
        onTriggered: {
            if (parent.parent.enabled.toString()=="true"){
                var syszoom = Qt.vector3d(0,0,0)
                syszoom = sceneRoot.systemTransform.scale3D
                if (rotateAxis == 0 && pastAxis != 0){
                    console.log("rotate done qml");
                    rotateDone(pastAxis);
                    pastAxis = 0;
                }
                if (rotateAxis != 0 && pastAxis == 0){
                    console.log("rotate start qml")
                    rotateInit();
                }

                pastAxis = rotateAxis
                if (rotateAxis != 0){
                    var Origin = world2Screen(center)
                    var ccwvar = vertexccw(mouseOrigin.x,mouseOrigin.y,Origin.x,Origin.y,mouseCurrent.x,mouseCurrent.y)
                    var mouseOrigin_Origin = mouseOrigin.minus(Origin).length()
                    var mouseCurrent_Origin = mouseCurrent.minus(Origin).length()
                    var mouseCurrent_mouseOrigin = mouseOrigin.minus(mouseCurrent).length()
                    var sinangle = ccwvar/(mouseOrigin_Origin * mouseCurrent_Origin)
                    var degreeangle = Math.asin(sinangle) * 180 / Math.PI
                    if (Math.pow(mouseCurrent_mouseOrigin,2) > Math.pow(mouseOrigin_Origin,2)+Math.pow(mouseCurrent_Origin,2)){
                        degreeangle = (Math.abs(degreeangle)/degreeangle)*(180-Math.abs(degreeangle))
                    }
                    var updown=0
                    if (Math.sin((systemTransform.rotationX)/ 180 * Math.PI) > 0){
                        updown = -1
                    }else{
                        updown = 1
                    }
                    switch(rotateAxis){
                    case 1: // xAxis
                        if (Math.sin(systemTransform.rotationZ / 180 * Math.PI) < 0){
                            rotateSignal(1,updown * (degreeangle-pastAngle))
                        }else{
                            rotateSignal(1,updown * (-degreeangle+pastAngle))
                        }
                        break;
                    case 2: // yAxis
                        if (Math.sin((systemTransform.rotationZ-90) / 180 * Math.PI) >0){
                            rotateSignal(2,updown * (degreeangle-pastAngle))
                        }else{
                            rotateSignal(2,updown * (-degreeangle+pastAngle))
                        }
                    break;
                    case 3: // zAxis
                        if (Math.sin((systemTransform.rotationX+90)/ 180 * Math.PI) > 0){
                            rotateSignal(3,degreeangle-pastAngle)
                        }else{
                            rotateSignal(3,-degreeangle+pastAngle)
                        }
                    break;
                    }
                    pastAngle = degreeangle
                }
                rotateSphereTransform.scale3D = Qt.vector3d(0.01/syszoom.x,0.01/syszoom.y,0.01/syszoom.z)

                //console.log(sceneRoot.systemTransform.rotationX )
                var theta = sceneRoot.systemTransform.rotationX/180.0*Math.PI
                var alpha = sceneRoot.systemTransform.rotationZ/180.0*Math.PI
                rotateSphereTransform.translation = Qt.vector3d(center.x+100*Math.sin(theta)*Math.sin(alpha),
                                                                center.y+100*Math.sin(theta)*Math.cos(alpha),
                                                                center.z+100*Math.cos(theta))

            }else{
                rotateAxis=0;
            }
        }
    }
}
