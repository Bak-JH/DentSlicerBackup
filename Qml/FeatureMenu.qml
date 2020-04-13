import QtQuick 2.4
import hix.qml 1.0 as Hix
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.12

Hix.FeatureMenu {

	property string activeNameColor: "#1db2c4"
	
	Rectangle {
		id: featureMenuRect
		// width: 1390
		width: 1230
		height: 90
		radius: 45
		color: "#FFFFFF"
		RowLayout {
		    id: featureItems
			objectName: "featureItems"
			anchors.left: parent.left
			anchors.leftMargin: parent.radius
        }
	}

	DropShadow {
		anchors.fill: featureMenuRect
		radius: 10.0
        samples: 21
        color: "#55000000"
		source: featureMenuRect
	}

}

