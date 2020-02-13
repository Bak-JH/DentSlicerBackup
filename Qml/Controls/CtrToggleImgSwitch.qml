import QtQuick 2.4
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.ToggleSwitch {
	id: root

	Hix.Button {
		id: flatbtn
		width: 104
		height: 96
		radius: 8
		border.color: "#b2c4c6"
		border.width: 1
		state: "flatOff"

		Image {
			id: flatimg
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top
			anchors.topMargin: parent.height * 0.15
			source: "qrc:/Resource/cut_flat_off.png"
		}

		Text {
			id: flattext
			font.pointSize: 10
			font.family: openRegular.name
			text: qsTr("Flat Cut")
			anchors.bottom: parent.bottom
			anchors.bottomMargin: parent.height * 0.08
			anchors.horizontalCenter: parent.horizontalCenter
		}

		onClicked: { root.isChecked = false; }

		states: [
			State {
				name: "flatOn"
				PropertyChanges { target: flatbtn; color: "#21959e" }
				PropertyChanges { target: flatbtn; border.color: "transparent" }
				PropertyChanges { target: flattext; color: "#ffffff" }
				PropertyChanges { target: flattext; font.family: openSemiBold.name }
				PropertyChanges { target: flatimg; source: "qrc:/Resource/cut_flat_on.png" }
			},
			State {
				name: "flatOff"
				PropertyChanges { target: flatbtn; color: "#f6feff" }
				PropertyChanges { target: flatbtn; border.color: "#b2c4c6" }
				PropertyChanges { target: flattext; color: "#859597" }
				PropertyChanges { target: flattext; font.family: openRegular.name }
				PropertyChanges { target: flatimg; source: "qrc:/Resource/cut_flat_off.png" }
			}
		]
	}

	//FreeCutButton {
	Hix.Button {
		id: freebtn
		width: 104
		height: 96
		radius: 8
		border.color: "#d3dfe0"
		border.width: 1
		anchors.right: parent.right
		state: "freeOff"

		Image {
			id: freeimg
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top
			anchors.topMargin: parent.height * 0.15
			source: "qrc:/Resource/cut_free_off.png"
		}

		Text {
			id: freetext
			text: qsTr("Free Cut")
			font.pointSize: 10
			font.family: openRegular.name
			color: "#859597"
			anchors.bottom: parent.bottom
			anchors.bottomMargin: parent.height * 0.08
			anchors.horizontalCenter: parent.horizontalCenter
		}

		onClicked: { root.isChecked = true; }

		states: [
			State {
				name: "freeOn"
				PropertyChanges { target: freebtn; color: "#21959e" }
				PropertyChanges { target: freebtn; border.color: "transparent" }
				PropertyChanges { target: freetext; color: "#ffffff" }
				PropertyChanges { target: freetext; font.family: openSemiBold.name }
				PropertyChanges { target: freeimg; source: "qrc:/Resource/cut_free_on.png" }
			},
			State {
				name: "freeOff"
				PropertyChanges { target: freebtn; color: "#f6feff" }
				PropertyChanges { target: freebtn; border.color: "#b2c4c6" }
				PropertyChanges { target: freetext; color: "#859597" }
				PropertyChanges { target: freetext; font.family: openRegular.name }
				PropertyChanges { target: freeimg; source: "qrc:/Resource/cut_free_off.png" }
			}
		]
	}

	onCheckedChanged: {
		if(root.isChecked)
		{
			flatbtn.state = "flatOff"; 
			freebtn.state = "freeOn"; 
		}
		else
		{
			flatbtn.state = "flatOn"; 
			freebtn.state = "freeOff";
		}
	}

}