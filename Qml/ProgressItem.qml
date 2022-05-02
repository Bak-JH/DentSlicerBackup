import QtQuick 2.0

Rectangle {
	id: contactDelegate
    width: 200
    height: 28
    //color: "#000000"

	property string featureName;
    FontLoader{ id: openRegular; source: "qrc:/Resource/font/OpenSans-Regular.ttf" }

	Text {
        id: item
        text: featureName
        font.family: openRegular.name
        font.pointSize: 10
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 8
    }

    Image {
        id: showhideimg
        source: "qrc:/Resource/progress_check.png"
        anchors.verticalCenter: parent.verticalCenter
        width: 20
        height: 20
        anchors.right: parent.right
        anchors.rightMargin: 8
    }
}
