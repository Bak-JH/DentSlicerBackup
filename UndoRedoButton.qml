import QtQuick 2.0
import QtGraphicalEffects 1.0


BlurRectangle{
    inputWidth: 60
    inputHeight: 60
    inputRadius: 30

    property string inputSource

    Image {
        anchors.centerIn: parent
        source: inputSource
    }
}
