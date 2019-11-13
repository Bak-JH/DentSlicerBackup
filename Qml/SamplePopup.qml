import QtQuick 2.4
import hix.qml 1.0

PopupShell
{
	width:244
	height:300
	
    color: "#ffffff"

	XButton
	{
		width:20
		height:20
		color: "#fff000"

		anchors.right: parent.right
		anchors.top: parent.top
		anchors.topMargin: 5
		anchors.rightMargin: 5
	}

	Text
	{
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.top
		anchors.topMargin: 12
		
		text: title
	}
}