/*
  CameraController.qml

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


import Qt3D.Core 2.0
import Qt3D.Input 2.0
import Qt3D.Logic 2.0
import Qt3D.Render 2.0

Entity {
    id: root

    property Camera camera

    property vector3d _originalUpVector

    onCameraChanged: {
        if (root.camera) {
            // we don't want to create a binding, but a copy of the initial value
            _originalUpVector = root.camera.upVector;
        }
    }




    KeyboardDevice {
        id: keyboard
    }

    MouseDevice {
        id: mouse
        sensitivity: 0.1
    }

    components: [
        LogicalDevice {
              id: logicalDevice;

              actions: [
                  Action {
                      id: moveAction
                      inputs: [
                          ActionInput {
                              sourceDevice: mouse
                              //buttons: [ MouseEvent.LeftButton ]
                              buttons: [ MouseEvent.MiddleButton ]
                          }
                      ]
                  },
                  Action {
                      id: rotateAction
                      inputs: [
                          ActionInput {
                              sourceDevice: mouse
                              buttons: [ MouseEvent.RightButton ]
                          }

                      ]
                  },
                  Action {
                      id: zoomInAction
                      inputs: [
                          ActionInput {
                              //sourceDevice: keyboard
                              //buttons: [ Qt.Key_Plus ]
                              sourceDevice: keyboard
                              buttons: [ Qt.Key_Plus ]

                          }
                      ]
                  },
                  Action {
                      id: zoomOutAction
                      inputs: [
                          ActionInput {
                              sourceDevice: keyboard
                              buttons: [ Qt.Key_Minus ]
                          }
                      ]
                  }
              ]


              axes: [
                  Axis {
                      id: rotateXAxis
                      inputs: [
                          AnalogAxisInput {
                              sourceDevice: mouse
                              axis: MouseDevice.X
                          }
                      ]
                  },
                  Axis {
                      id: rotateYAxis
                      inputs: [
                          AnalogAxisInput {
                              sourceDevice: mouse
                              axis: MouseDevice.Y
                          }

                      ]
                  },

                  Axis {
                      id: translateXAxis
                      inputs: [
                          AnalogAxisInput {
                              sourceDevice: mouse
                              axis: MouseDevice.X
                          }
                      ]
                  },
                  Axis {
                      id: translateYAxis
                      inputs: [
                          AnalogAxisInput {
                              sourceDevice: mouse
                              axis: MouseDevice.Y
                          }
                      ]
                  }
              ]
          },

        FrameAction {
            property real translationSpeed : 150.0
            property real rotationSpeed : 260.0
            property real zoomFactor : 0.1
            property vector3d angleVector

            onTriggered: {
                if (zoomInAction.active || zoomOutAction.active) { // not use
                    var zoom = 1.0 - zoomFactor;
                    if (zoomOutAction.active) {
                        zoom = 1.0 + zoomFactor;
                    }
                    var viewVector = root.camera.position.minus(root.camera.viewCenter)
                    viewVector = viewVector.times(zoom);
                    root.camera.position = root.camera.viewCenter.plus(viewVector);


                } else if (moveAction.active) { // not use
                    /*
                    window.moveH(translateXAxis.value);
                    window.moveV(translateYAxis.value);
                    */

                } else if (rotateAction.active) {
                    //root.camera.panAboutViewCenter(rotationSpeed * rotateXAxis.value * dt,Qt.vector3d( 0.0, 0.0, 1.0 ));
                    angleVector = root.camera.position.minus(Qt.vector3d(0,0,0)).normalized();

                    console.log("Zzzz " +angleVector.z)
                    if(angleVector.z>0.999){

                        if(rotateYAxis.value<=0){
                            return;
                        }
                        root.camera.setUpVector(_originalUpVector);
                    }
                    if(angleVector.z<-0.999){

                        if(rotateYAxis.value>=0){
                            return;
                        }
                        root.camera.setUpVector(_originalUpVector.times(-1));
                    }

                    root.camera.tiltAboutViewCenter(rotationSpeed * rotateYAxis.value*(-1) * dt,Qt.vector3d( -1.0, 0.0, 0.0 ));

                    // lock the camera roll angle
                    //root.camera.setUpVector(_originalUpVector);

                    //sceneRoot.total.mtr.perfectPosition();
                }

            }
        }
    ]
}
