import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtGraphicalEffects 1.12
import QtQuick.Controls 2.1
import "../Controls" 

FeaturePopup {
	title: qsTr("Printer")
	height: 180

	CtrDropdownBox {
		id: printerDrop
		label.text: "Printer Servers"
		width: parent.width - 14	// 16 = triangle height
		height: 26
		color: "transparent"
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.topMargin: 44
		anchors.leftMargin: 4
	}
}
