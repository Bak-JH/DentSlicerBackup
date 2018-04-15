import Qt3D.Core 2.0

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Rectangle {
    color: "#EFEFEF"
    property var options: []
    property int ftrExport : 3

    Text{
        text : "Slicing Setup Check"
        id : title

        anchors.top:  parent.top
        anchors.topMargin: 32
        anchors.horizontalCenter: parent.horizontalCenter

        font.family: mainFont.name

        font.bold: true
        color: "#3D3D3D"
        font.pixelSize: 16
    }

    Rectangle{
        id: rightline
        width: 1
        anchors.right : parent.right
        anchors.top : parent.top
        anchors.bottom: parent.bottom

        color: "#CCCCCC"
    }

    Rectangle{
        id : content
        width: parent.width
        height: 300
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top :  title.bottom
        anchors.topMargin: 36

        color:"transparent"

        Item {
            width: 224
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top : parent.top
            ColumnLayout {
                spacing: 26
                LeftTabExportElement{
                    columnName: "Resolution"
                    columnContent : lefttab.ltso.option_resolution.currentText
                }
                LeftTabExportElement{
                    columnName: "Layer Height"
                    columnContent : lefttab.ltso.option_layer_height.currentText
                }
                LeftTabExportElement{
                    columnName: "Bed Number"
                    columnContent : lefttab.ltso.option_bed_number.currentText
                }
                LeftTabExportElement{
                    columnName: "Raft"
                    columnContent : lefttab.ltso.option_raft.currentText
                }
                LeftTabExportElement{
                    columnName: "Support"
                    columnContent : lefttab.ltso.option_support.currentText + " / " + lefttab.ltso.option_support_density.currentText + "%"
                }
                LeftTabExportElement{
                    columnName: "Infill"
                    columnContent : lefttab.ltso.option_infill.currentText + " / " + lefttab.ltso.option_infill_density.currentText + "%"
                }
            }
        }
    }

    Rectangle {
        id: okButton
        objectName: "exportOKBtn"
        width: 104
        height: 28
        color: "#999999"
        anchors.left: parent.left
        anchors.top: content.bottom
        anchors.leftMargin: 20

        Text {
            id: okButton_text
            anchors.centerIn: parent
            text: "OK"
            color: "#ffffff"
            font.pixelSize: 16
            font.family: mainFont.name
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: parent.color = "#b5b5b5"
            onExited: parent.color = "#999999"
            onPressed: parent.color = "#3ea6b7"
            onReleased: {
                parent.color = "#999999"

                function collectConfigurations(){
                    var configurations = {};

                    // do collecting things
                    // configurations[key] = value;
                    configurations["resolution"] = options[0];
                    configurations["layer_height"] = options[1];
                    configurations["support_type"] = options[2];
                    configurations["infill_type"] = options[3];
                    configurations["raft_type"] = options[4];
                    return configurations;
                }

                console.log(parent.id);
                console.log(parent.parent.id);

                console.log("exporting");
                // collect configurations
                var cfg = collectConfigurations();
                //se.slice(cfg);
                parent.runFeature(ftrExport, cfg);

            }
        }
        signal runFeature(int type, var config);
    }


    Rectangle {
        id: cancelButton
        width: 104
        height: 28
        color: "#999999"
        anchors.right: parent.right
        anchors.top: content.bottom
        anchors.rightMargin: 20

        Text {
            id: cancelButton_text
            anchors.centerIn: parent
            text: "Cancel"
            color: "#ffffff"
            font.pixelSize: 16
            font.family: mainFont.name

        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: parent.color = "#b5b5b5"
            onExited: parent.color = "#999999"
            onPressed: parent.color = "#3ea6b7"
            onReleased: {
                lefttabExport.visible = false;
                parent.color = "#999999"
            }
        }
    }

}
