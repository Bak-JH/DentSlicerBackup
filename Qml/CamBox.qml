import QtQuick 2.6
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.Button {
	width: 58
	height: width
	radius: 6
	color: "#ffffff"

	onEntered: { color = "#eeeeee" }
	onExited: { color = "#ffffff" }

}