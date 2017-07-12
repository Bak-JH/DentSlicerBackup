import QtQuick 2.0


import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick 2.7 as QQ2


Entity {
    property real xLookAtOffset: 0
    property real yLookAtOffset: 0
    property real zLookAtOffset: 0
    property real xCameraOffset: 0
    property real yCameraOffset: 0
    property real zCameraOffset: 0

    property vector3d defaultUp: Qt.vector3d(0, 0, 1)
    property vector3d defaultCameraPosition: Qt.vector3d(40,0,40)


    Camera {
        id: camera

        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        nearPlane : 0.1
        farPlane : 1000.0

        position: defaultCameraPosition
        upVector: defaultUp
        viewCenter: Qt.vector3d( xLookAtOffset, yLookAtOffset, zLookAtOffset )

        property vector3d temp : Qt.vector3d( 0.0, 0.0, 0.0 )




    }
    CameraController{
        camera:camera
    }

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                camera: camera
                clearColor: "transparent"
            }
        },
        InputSettings {     }
    ]

    QQ2.Vector3dAnimation{
        id : zoomAnimation
        target: camera
        property : "position"

        from : camera.position
        //to: camera.position.minus(camera.temp)
        duration: 100
        easing.type: "Linear"


    }

    function zoomGap(){
        var Camera_position, average, temp, cameraRadius

        Camera_position = camera.position;
        average = camera.viewCenter
        temp = Camera_position.minus(average.times(10))
        cameraRadius = Math.sqrt(Math.pow(temp.x,2) + Math.pow(temp.y,2) + Math.pow(temp.z,2))
        console.log(cameraRadius)
        camera.temp = temp.times(0.25)

        return cameraRadius
    }

    function zoomUp(){
        var cameraRadius = zoomGap()
        if(cameraRadius > 10){
            //camera.position = camera.position.minus(temp.times(0.25))

            zoomAnimation.to = camera.position.minus(camera.temp)
            zoomAnimation.start()


        }
    }

    function zoomDown(){
        var cameraRadius = zoomGap()

        if(cameraRadius < 1600){
            //camera.position = camera.position.minus(temp.times(0.25))
            zoomAnimation.to = camera.position.plus(camera.temp)
            zoomAnimation.start()


        }
    }

    MouseDevice{
        id : mouseDevice

    }

    MouseHandler{
        id : mouseHandler
        sourceDevice: mouseDevice
        /*
        onPressed: {
            console.log("aaaaaa")
            console.log("pos  " + camera.position + " / cent  " + camera.viewCenter + " / rot" + camera.upVector)
            camera.viewCenter = Qt.vector3d( xLookAtOffset, yLookAtOffset, zLookAtOffset )

        }
        */
    }

    KeyboardDevice{
        id : keyboardDevice
    }
    KeyboardHandler{
        focus : true
        id : keyboardHandler
        sourceDevice: keyboardDevice

        onPressed: {

            if (event.key === Qt.Key_T) {
                camera.viewCenter = Qt.vector3d( xLookAtOffset, yLookAtOffset, zLookAtOffset )
                camera.upVector = defaultUp


            }
            if (event.key === Qt.Key_P) {
                console.log("pos  " + camera.position + " / cent  " + camera.viewCenter + " / rot" + camera.upVector)

            }
        }
    }



    /*
    KeyboardDevice{
        id : keyboardDevice
    }

    KeyboardHandler{
        id : keyboardHandler
        Keys.onPressed: {
                if (event.key == Qt.Key_T) {
                    camera.viewCenter = Qt.vector3d( xLookAtOffset, yLookAtOffset, zLookAtOffset )
                }
                if (event.key == Qt.Key_P) {
                    console.log("pos  " + camera.position + " / cent  " + camera.viewCenter + " / rot" + camera.rotation)
                }
        }
    }
    */
}
