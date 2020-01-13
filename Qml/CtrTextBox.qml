import QtQuick 2.6
import hix.qml 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

LabelTextBox {
	Text {
		text: qsTr("Text")
		font.family: openRegular.name
		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter
		color: "#666666"
	}

	TextField {
		id: control
		width: parent.width * 0.56 //0.6
		height: 26
		placeholderText: qsTr("Enter text here")
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		background: Rectangle {
			id: bgrect
			color: control.hovered ? "#ffffff" : "#f6feff"
			border.color: control.hovered ? "#b3bfc0": "#d3dfe0"
			radius: 2
			//border.color: control.enabled ? "#21be2b" : "transparent"
			/*
			layer.enabled: true
			layer.effect: DropShadow {
				visible: control.hovered ? true : false
				radius: 4.0
				samples: 9
				color: "#30000000"
			}
			*/
		}
	}
	/*
	DropShadow {
		visible: control.hovered ? true : false
		anchors.fill: bgrect
		radius: 4.0
		samples: 9
		color: "#30000000"
		source: bgrect
	}
	*/
}