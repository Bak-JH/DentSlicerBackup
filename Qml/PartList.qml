import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.12
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12
import QtQuick.Layouts 1.12

Hix.PartList {
	id: partList
	width: 256
	height: 320
	property var myPadding: 16
	Rectangle {
		id: shell
		anchors.fill: parent
		radius: 8
		Text {
			id: listtitle
			text: qsTr("Part List")
			font.family: openSemiBold.name
			font.pointSize: 11
			anchors.top: parent.top
			anchors.topMargin: myPadding
			anchors.left: parent.left
			anchors.leftMargin: myPadding
		}
		ScrollView {

			MouseArea {
				id: wheelArea
				anchors.fill: parent
				hoverEnabled: true
				onClicked: {
					console.log("sdffsf");
				}
			}
			id: scrollView
			width: 256 - 16;
			height: 212;
			anchors.right: parent.right
			anchors.top: listtitle.bottom
			anchors.topMargin: 8
			clip: true
			focus: true
			contentWidth: itemContainer.width 
			contentHeight: itemContainer.height
			Column 
			{
				id: itemContainer
				width: parent.width


			}
			ScrollBar.vertical: ScrollBar{
				id: control
				policy: ScrollBar.AsNeeded
				interactive: true
				implicitHeight: scrollView.height
			}
		}

		Hix.Button {
			id: deleteButton
			width: 14
			height: width
			anchors.right: parent.right
			anchors.rightMargin: myPadding
			anchors.bottom: parent.bottom
			anchors.bottomMargin: myPadding
			Image {
				anchors.right: parent.right
				anchors.bottom: parent.bottom
				source: "qrc:/Resource/part_remove_1.png"
				//sourceSize.width: width
			}
		}

	}
	DropShadow {
		anchors.fill: shell
		radius: 10.0
		samples: 21
		color: "#55000000"
		source: shell
	}
}