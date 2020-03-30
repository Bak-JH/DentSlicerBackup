import QtQuick 2.6
import QtQuick.Controls 2.1	
import QtGraphicalEffects 1.12

Item
{
	id: root
	property string modelName

	Rectangle
	{
		id: fullNameBox
		width: 200
		height: modelnameText.contentHeight + 10
		color: "#FFFFFF"
		border.width: 1
        border.color: "#CCCCCC"
		radius: 1
		Text { 
			id: modelnameText
			text: root.modelName
			width: 190
			layer.enabled: true
			wrapMode: Text.WrapAnywhere
			font.family: openRegular.name
			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter
		}
	}

	DropShadow 
	{
		anchors.fill: fullNameBox
		radius: 8
		samples: 21
		color: "#55000000"
		source: fullNameBox
	}
}