import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1


Hix.Button {
	id: root
    signal entered()
    signal exited()
    signal positionChanged(var mouse)
    // signal clicked(MouseEvent mouse)
	property alias mouseArea: mouseArea
	MouseArea {
		id: mouseArea
		anchors.fill: parent
		hoverEnabled: true
	}
    function qmlClicked(e)
    {
        if(e.button == Qt.LeftButton)
        {
            clicked();
        }
    }
    Component.onCompleted: {
        mouseArea.entered.connect(entered)
        mouseArea.exited.connect(exited)
        mouseArea.clicked.connect(qmlClicked)
        mouseArea.positionChanged.connect(positionChanged)
    }

}

