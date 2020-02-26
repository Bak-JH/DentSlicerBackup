import QtQuick 2.4
import hix.qml 1.0 as Hix
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12

Hix.ToggleSwitch {
	id: root
	property string leftBtnImgSource
	property string rightBtnImgSource
	property string leftBtnText: "left Item"
	property string rightBtnText: "right Item"

	function setButtonSelected(isSelected)
	{
		if(isSelected)
		{
			rightBtn.color = "#21959e";
			rightBtn.border.color = "transparent";
			rightBtnLabel.color = "#FFFFFF";
			rightBtnLabel.font.family = openSemiBold.name;
			rightBtnImg.source = rightBtnImgSource + "_on.png";

			leftBtn.color = "#f6feff";
			leftBtn.border.color = "#b2c4c6";
			leftBtnLabel.color = "#859597";
			leftBtnLabel.font.family = openRegular.name;
			leftBtnImg.source = leftBtnImgSource + "_off.png";
		}
		else
		{
			leftBtn.color = "#21959e";
			leftBtn.border.color = "transparent";
			leftBtnLabel.color = "#FFFFFF";
			leftBtnLabel.font.family = openSemiBold.name;
			leftBtnImg.source = leftBtnImgSource + "_on.png";

			rightBtn.color = "#f6feff";
			rightBtn.border.color = "#b2c4c6";
			rightBtnLabel.color = "#859597";
			rightBtnLabel.font.family = openRegular.name;
			rightBtnImg.source = rightBtnImgSource + "_off.png";
		}
	}

	Hix.Button {
		id: leftBtn
		width: 104
		height: 96
		radius: 8
		color: "#21959e"
		border.color: "transparent"
		border.width: 1

		//property alias Img : leftBtnImg
		//property alias Label: leftBtnLabel

		Image {
			id: leftBtnImg
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top
			anchors.topMargin: parent.height * 0.15
			source: root.leftBtnImgSource + "_on.png"
		}

		Text {
			id: leftBtnLabel
			text: root.leftBtnText
			font.pointSize: 10
			font.family: openSemiBold.name
			color: "#FFFFFF"
			anchors.bottom: parent.bottom
			anchors.bottomMargin: parent.height * 0.08
			anchors.horizontalCenter: parent.horizontalCenter
		}

		onClicked: { root.isChecked = false; }
	}

	//FreeCutButton {
	Hix.Button {
		id: rightBtn
		width: 104
		height: 96
		radius: 8
		color: "#f6feff"
		border.color: "#b2c4c6"
		border.width: 1
		anchors.right: parent.right

		Image {
			id: rightBtnImg
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top
			anchors.topMargin: parent.height * 0.15
			source: root.rightBtnImgSource + "_off.png"
		}

		Text {
			id: rightBtnLabel
			text: root.rightBtnText
			font.pointSize: 10
			font.family: openRegular.name
			anchors.bottom: parent.bottom
			anchors.bottomMargin: parent.height * 0.08
			anchors.horizontalCenter: parent.horizontalCenter
		}

		onClicked: { root.isChecked = true; }
	}

	onCheckedChanged: {
		setButtonSelected(root.isChecked);
	}

}