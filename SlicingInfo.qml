import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3

BlurRectangle{
    inputWidth: 420
    inputHeight: 72
    inputRadius : 36

    BlurRectangle{ //Refresh Button
        inputWidth: 60
        inputHeight: 60
        inputRadius: 30
        inputColor : "#67D1DD"

        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 12



        Image {
            anchors.centerIn: parent
            source: "qrc:/Resource/refresh.png"
        }
    }

    GridLayout{
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 106

        rows: 2
        columns: 2

        rowSpacing: 10
        columnSpacing: 20

        SlicingInfoData{
            inputSource: "qrc:/Resource/ic-slicing-1.png"
        }
        SlicingInfoData{
            inputSource: "qrc:/Resource/ic-slicing-2.png"
        }
        SlicingInfoData{
            inputSource: "qrc:/Resource/ic-slicing-3.png"
        }
        SlicingInfoData{
            inputSource: "qrc:/Resource/ic-slicing-4.png"
        }
    }


}
