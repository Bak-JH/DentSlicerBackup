import QtQuick 2.0
import QtQuick.Layouts 1.3

Item{
    width: 295
    height: 310

    anchors.top : parent.top
    anchors.left: parent.left
    anchors.leftMargin: 288
    anchors.topMargin: 430

    Image{
        width: parent.width
        height: parent.height
        source: "qrc:/Resource/tab-slicingoption.png"
    }

    ColumnLayout{  // Update to ScrollView
        width: 240
        height: 268
        anchors.verticalCenter: parent.verticalCenter
        anchors.right : parent.right
        anchors.rightMargin: 16
        spacing : 12

        SlicingOptionElement{
            inputTitle: "Resolution"
        }
        SlicingOptionElement{
            inputTitle: "Layer Height"
        }
        SlicingOptionElement{
            inputTitle: "Raft"
        }
        SlicingOptionElement{
            inputTitle: "Support Type"
        }
        SlicingOptionElement{
            inputTitle: "Support Density"
        }
        SlicingOptionElement{
            inputTitle: "Infill Type"
        }
        SlicingOptionElement{
            inputTitle: "Infill Density`"
        }

    }
}
