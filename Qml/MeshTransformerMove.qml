import QtQuick 2.0
import QtQuick 2.0
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import Qt3D.Logic 2.0
import QtGraphicalEffects 1.0

Entity{
    id : moveArrow
    property vector3d center : Qt.vector3d( 0,0,0 )
    property vector3d inputScale : Qt.vector3d(0.6,0.6,0.6)
    property vector3d initScale : Qt.vector3d(0.3,0.3,0.3)

    property vector3d intersect : Qt.vector3d(0,0,0)
    property int moveAxis : 0 //0:none, 1:x, 2:y, 3:z
    property vector2d mouseOrigin : Qt.vector2d(0,0)
    property vector2d mouseCurrent : Qt.vector2d(0,0)
    property int moveDirection : 0

    property int pastDistance :0
    property int pastAxis :0
    components: [
        Transform {
            id:moveArrowTransform
            translation:center
            scale3D: Qt.vector3d(1,1,1)
        }
    ]
    Mesh{
        id: moveArrowMesh
        source: "qrc:/Resource/mesh/arrow.stl"
    }

    Entity {
        id: moveArrowX
        objectName: "moveArrowX"
        Transform{
            id: movearrowXTransform
            translation: Qt.vector3d(0,0,0)
            scale3D: initScale
            rotation: fromAxisAndAngle(Qt.vector3d(0,1, 0), 90)
        }
        PhongMaterial{
            id : moveArrowXMaterial
            ambient: Qt.rgba(150/255, 150/255, 150/255, 1)
            diffuse: Qt.rgba(150/255, 150/255, 150/255, 1)
            specular:  Qt.rgba(150/255, 150/255, 150/255, 1)
            shininess: 0
        }
        ObjectPicker{
            id : moveArrowobjectPickerX
            isShareable: false
            hoverEnabled: true
            onEntered: {
                if (moveAxis == 0){
                    //rotation end
                    moveArrowXMaterial.ambient = Qt.rgba(237/255, 217/255, 41/255, 1)
                    moveArrowXMaterial.diffuse = Qt.rgba(237/255, 217/255, 41/255, 1)
                    moveArrowXMaterial.specular =  Qt.rgba(237/255, 217/255, 41/255, 1)
                }
            }
            onExited: {
                if (moveAxis == 0){
                    moveArrowXMaterial.ambient = Qt.rgba(150/255,150/255,150/255,1)
                    moveArrowXMaterial.diffuse = Qt.rgba(150/255,150/255,150/255,1)
                    moveArrowXMaterial.specular = Qt.rgba(150/255,150/255,150/255,1)
                }
            }
            onPressed: { //move start
                if (moveAxis  == 0){
                    intersect = Qt.vector3d(pick.localIntersection.x,pick.localIntersection.y,pick.localIntersection.z)
                    if (pick.localIntersection.z > 0){
                        moveDirection = 1;
                    }else{
                        moveDirection = -1;
                    }
                    mouseOrigin = Qt.vector2d(pick.position.x , pick.position.y)
                    moveAxis = 1 //X
                    moveArrowY.setEnabled(false)
                    pastDistance=0;
                }
            }
            onReleased: {
                arrowRelease();
            }
        }
        components: [ moveArrowMesh, movearrowXTransform, moveArrowXMaterial,moveArrowobjectPickerX ]
    }
    Entity {
        id: moveArrowY
        objectName: "moveArrowY"
        Transform{
            id: movearrowYTransform
            translation: Qt.vector3d(0,0,0)
            scale3D: initScale
            rotation: fromAxisAndAngle(Qt.vector3d(1,0,0), 90)
        }
        PhongMaterial{
            id : moveArrowYMaterial
            ambient: Qt.rgba(150/255, 150/255, 150/255, 1)
            diffuse: Qt.rgba(150/255, 150/255, 150/255, 1)
            specular:  Qt.rgba(150/255, 150/255, 150/255, 1)
            shininess: 0
        }
        ObjectPicker{
            id : moveArrowobjectPickerY
            dragEnabled: true
            hoverEnabled: true
            onEntered: {
                if (moveAxis == 0){
                    //rotation end
                    moveArrowYMaterial.ambient = Qt.rgba(237/255, 217/255, 41/255, 1)
                    moveArrowYMaterial.diffuse = Qt.rgba(237/255, 217/255, 41/255, 1)
                    moveArrowYMaterial.specular =  Qt.rgba(237/255, 217/255, 41/255, 1)
                }
            }
            onExited: {
                if (moveAxis == 0){
                    moveArrowYMaterial.ambient = Qt.rgba(150/255,150/255,150/255,1)
                    moveArrowYMaterial.diffuse = Qt.rgba(150/255,150/255,150/255,1)
                    moveArrowYMaterial.specular = Qt.rgba(150/255,150/255,150/255,1)
                }
            }
            onPressed: { //move start
                if (moveAxis  == 0){
                    intersect = Qt.vector3d(pick.localIntersection.x,pick.localIntersection.y,pick.localIntersection.z)

                    if (pick.localIntersection.z > 0){
                        moveDirection = -1;
                    }else{
                        moveDirection = 1;
                    }
                    mouseOrigin = Qt.vector2d(pick.position.x , pick.position.y)
                    moveAxis = 2 //Y
                    //moveArrowYMaterial.ambient = Qt.rgba(200/255,200/255,0/255,1)
                    moveArrowX.setEnabled(false)
                    pastDistance=0;
                }
            }
            onReleased: {
                arrowRelease();
            }
        }
        components: [ moveArrowMesh, movearrowYTransform, moveArrowYMaterial,moveArrowobjectPickerY]
    }
    function arrowRelease(){
        if (moveAxis == 1){
            moveAxis = 0;
            moveArrowY.setEnabled(true)
            moveArrowXMaterial.ambient = Qt.rgba(150/255,150/255,150/255,1)
            moveArrowXMaterial.diffuse = Qt.rgba(150/255,150/255,150/255,1)
            moveArrowXMaterial.specular = Qt.rgba(150/255,150/255,150/255,1)
        }else if (moveAxis == 2){
            moveAxis = 0;
            moveArrowX.setEnabled(true)
            moveArrowYMaterial.ambient = Qt.rgba(150/255,150/255,150/255,1)
            moveArrowYMaterial.diffuse = Qt.rgba(150/255,150/255,150/255,1)
            moveArrowYMaterial.specular = Qt.rgba(150/255,150/255,150/255,1)
        }
    }

    MouseDevice{
        id: movemousedevice
        sensitivity: 0.01

    }
    MouseHandler{
        id : moveMouseHandler
        sourceDevice: movemousedevice
        onClicked: {
            if (mouse.buttons == MouseEvent.RightButton){
                moveAxis = 0;
            }
        }
        onReleased: {
            arrowRelease();
            //moveAxis = 0;

            //moveArrowXMaterial.ambient = Qt.rgba(150/255,150/255,150/255,1)
            //moveArrowYMaterial.ambient = Qt.rgba(150/255,150/255,150/255,1)
        }
        onPositionChanged: {
            mouseCurrent = Qt.vector2d(mouse.x,mouse.y)
        }
    }
    function world2Screen(target){
        var tmp = Qt.vector3d(0,0,0)
        tmp = target
        target = tmp.times(systemTransform.scale3D)
        var theta = (-1)*sceneRoot.systemTransform.rotationX/180.0*Math.PI
        var alpha = (-1)*sceneRoot.systemTransform.rotationZ/180.0*Math.PI
        target = Qt.vector3d(target.x * Math.cos(alpha)+target.y*Math.sin(alpha),
                             target.x * (-1) * Math.sin(alpha) + target.y*Math.cos(alpha),
                             target.z)
        var target3 = Qt.vector3d(target.x,
                             target.y * Math.cos(theta)+target.z*Math.sin(theta),
                             (-1)*target.y*Math.sin(theta)+target.z*Math.cos(theta))
        var point2 = target3
        var matrix = Qt.matrix4x4(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
        matrix = sceneRoot.cm.camera.projectionMatrix.times(sceneRoot.cm.camera.viewMatrix);//
        point2 = matrix.times(point2)
        point2.x = (point2.x+1) * scene3d.width/2;
        point2.y = (-1 * point2.y+1) * scene3d.height/2;
        return Qt.vector2d(point2.x,point2.y)
    }

    signal moveSignal(int Axis, int distance)
    signal moveDone(int Axis)
    FrameAction{
        id : moveFrameAction
        onTriggered: {
            if (parent.parent.enabled.toString()=="true"){
                var syszoom = Qt.vector3d(0,0,0)
                syszoom = sceneRoot.systemTransform.scale3D
                if (moveAxis == 0 && pastAxis != 0){
                    moveDone(pastAxis);
                    pastAxis = 0;
                }
                pastAxis = moveAxis
                if (moveAxis != 0){
                    var Origin = world2Screen(center)
                    var mouseOrigin_Origin = mouseOrigin.minus(Origin).length() //a
                    var mouseCurrent_Origin = mouseCurrent.minus(Origin).length() //b
                    var mouseCurrent_mouseOrigin = mouseOrigin.minus(mouseCurrent).length() //c
                    var distance = ((Math.pow(mouseCurrent_Origin,2)-Math.pow(mouseCurrent_mouseOrigin,2)-Math.pow(mouseOrigin_Origin,2))/(2*mouseOrigin_Origin))
                    distance*=( 0.0006 / syszoom.x)
                    switch(moveAxis){
                    case 1: // xAxis
                        moveSignal(1,moveDirection*(distance - pastDistance))
                        break;
                    case 2: // yAxis
                        moveSignal(2,moveDirection*(distance - pastDistance))
                        break;
                    }
                    pastDistance = distance
                    //sphere3Transform.translation = Qt.vector3d(distance,0,0)
                    //console.log(distance)
                }
                moveArrowTransform.scale3D = Qt.vector3d(0.01/syszoom.x,0.01/syszoom.y,0.01/syszoom.z)

                //moveArrowTransform.translation = center.minus(cm.camera.viewVector) // When camera rotating mode
                var theta = sceneRoot.systemTransform.rotationX/180.0*Math.PI
                var alpha = sceneRoot.systemTransform.rotationZ/180.0*Math.PI
                moveArrowTransform.translation = Qt.vector3d(center.x+100*Math.sin(theta)*Math.sin(alpha),
                                                                center.y+100*Math.sin(theta)*Math.cos(alpha),
                                                                center.z+100*Math.cos(theta))

            }else{
                moveAxis=0;
            }
        }
    }
}
