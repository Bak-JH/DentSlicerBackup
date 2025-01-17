import QtQuick 2.0

import Qt3D.Core 2.0
import Qt3D.Render 2.11
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

    property vector3d defaultUp: Qt.vector3d(0, 1, 0)
    property vector3d defaultDown: Qt.vector3d(0,-1, 0)
    property vector3d defaultCameraPosition: Qt.vector3d(0,0,100)
    property real aspectRatioScene: scene3d.height/scene3d.width * 0.5
    property real bottomPlaneMargin: 0.2
    property vector3d inputViewCenter : Qt.vector3d( 0, 0, 0)
    property alias camera: camera
    property alias cameraLens: cameraLens

    signal cameraRotated();

    Camera {
        objectName: "camera"
        id: camera

        //projectionType: CameraLens.OrthographicProjection

//        aspectRatio: (scene3d.width/scene3d.height)
        //aspectRatio: 4

        position: defaultCameraPosition
        upVector: defaultUp

        viewCenter: inputViewCenter
        components:[
            CameraLens{
                id: cameraLens
                fieldOfView: 100
                nearPlane : 10
                farPlane : 1000
                projectionType: CameraLens.OrthographicProjection
                left: -0.5
                right: 0.5
                top:  aspectRatioScene +bottomPlaneMargin
                bottom: -aspectRatioScene +bottomPlaneMargin
            },
            RenderSettings {
                id : rd
                renderPolicy: RenderSettings.OnDemand
                pickingSettings.pickMethod: PickingSettings.TrianglePicking
                pickingSettings.pickResultMode: PickingSettings.NearestPick
                pickingSettings.faceOrientationPickingMode: PickingSettings.FrontFace

                activeFrameGraph: RenderSurfaceSelector {
                    Viewport {
                        id: mainViewport
                        normalizedRect: Qt.rect(0, 0, 1, 1)
                        CameraSelector {
                            camera:camera
                            ClearBuffers {
                                buffers: ClearBuffers.ColorDepthBuffer
                                clearColor: "#00000000"
                                // clearColor: "#FF0000FF"

                            }
                            // RenderStateSet {
                            //     renderStates: [
                            //         CullFace { mode : CullFace.Back },       
                            //         DepthTest { depthFunction: DepthTest.Less },         
                            //         NoDepthMask { },         
                            //         BlendEquationArguments {         
                            //             sourceRgb: BlendEquationArguments.Alpha        
                            //             destinationRgb: BlendEquationArguments.Alpha       
                            //             },
                            //         BlendEquation {blendFunction: BlendEquation.Add}
                            //     ]
                            // }
                            SortPolicy {
                                sortTypes: [
                                    SortPolicy.BackToFront
                                ]
                            }
                        }
                    }
                }
            }

        ]

    }

    CameraController{
        camera:camera
        onCameraChanged: {
            cameraRotated();
        }
    }

    components: [
        InputSettings {}
    ]


    function initCamera(){
        sceneRoot.cm.camera.translateWorld(sceneRoot.cm.camera.viewCenter.times(-1))
        sceneRoot.cm.camera.translateWorld(Qt.vector3d(-0.015,-0.16,0))
    }
}
