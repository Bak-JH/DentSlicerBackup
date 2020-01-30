import QtQuick 2.0
import QtGraphicalEffects 1.12
	
Rectangle {
    id: sidePopup
    color: "transparent"
    anchors.left: shell.right
    anchors.leftMargin: 12
    anchors.top: parent.top
    visible: false

	property string popupColor: "#f6feff"

    Canvas {
        id: canvas
        anchors.fill: parent
        width: parent.width
        height: parent.height
        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();

            var centreX = width / 2;
            var centreY = height / 2;

            var windowRadius = 8;
            var startX = width - windowRadius;
            var startY = windowRadius;

            //280*224 168*88
            var w0 = width - 16
            var h0 = height
            var w = w0 - windowRadius * 2;	//straight part
            var h = h0 - windowRadius * 2;	//straight part
            var s = 20;		//triangle part height
            var h1 = h - s - 8;


            ctx.beginPath();
            ctx.fillStyle = popupColor;
            ctx.moveTo(startX, startY-windowRadius);
            ctx.arc(startX, startY, windowRadius, Math.PI * 1.5, Math.PI * 2, false);
            ctx.lineTo(startX+windowRadius, startY+h);
            ctx.arc(startX, startY+h, windowRadius, 0, Math.PI * 0.5, false);
            ctx.lineTo(startX-w, startY+h+windowRadius);
            ctx.arc(startX-w, startY+h, windowRadius, Math.PI * 0.5, Math.PI, false);
            ctx.lineTo(startX-w-windowRadius, startY+h-h1);
            ctx.lineTo(startX-w-windowRadius-16, startY+h-h1-s/2);
            ctx.lineTo(startX-w-windowRadius, startY+h-h1-s);
            ctx.lineTo(startX-w-windowRadius, startY);
            ctx.arc(startX-w, startY, windowRadius, Math.PI, Math.PI * 1.5, false);
            ctx.lineTo(startX, startY-windowRadius);
            ctx.fill();
        }
	}

	DropShadow {
		anchors.fill: canvas
		radius: 10.0
		samples:21
		color: "#55000000"
		source: canvas
	}
}