import QtQuick 2.4
import hix.qml 1.0
import QtQuick.Controls 2.1

InputBox {
	Text {
		text: parent.propName
	}
	
	SpinBox{
		anchors.right: parent.right
		//from: 1
		//to: 10
		from: parent.inputRect.from
		to: parent.inputRect.to
	}
	
	/*
	inputRect: SpinBox{
		anchors.right: parent.right
		from: 0
		to: 10
	}
	*/
}