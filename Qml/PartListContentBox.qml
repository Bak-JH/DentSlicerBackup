import QtQuick 2.6
import hix.qml 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

PartListContent {
	width: 224
	height: 28
	radius: 2
	color: "#f5f5f5"
	Text {
		text: parent.modelname
		font.family: openRegular.name
		anchors.verticalCenter: parent.verticalCenter
	}
	Rectangle {
		id: showhidebtn
		anchors.right: parent.right
		anchors.rightMargin: width /2
		anchors.verticalCenter: parent.verticalCenter
		width: 14
		height: 14
		color: "#ffaa00"
	}
}