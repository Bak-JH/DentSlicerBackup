import QtQuick 2.6
import hix.qml 1.0
import QtQuick.Controls 2.1
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.12


Rectangle {
    width: 298
    height: 224
	color: "transparent"

    Canvas {
		id: canvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();

            var centreX = width / 2;
            var centreY = height / 2;

			var windowRadius = 8;
			var startX = width - windowRadius;
            var startY = windowRadius;

			//282*224
			var w0 = 282
			var h0 = 224
			var w = w0 - windowRadius * 2;	//straight part
			var h = h0 - windowRadius * 2;	//straight part
			var s = 20;		//triangle part height
			var h1 = h - s - 24;
			

			//ctx.shadowBlur = 9;
			//ctx.shadowColor = "#30000000";

			ctx.beginPath();
			ctx.fillStyle = "#f6feff";
			ctx.moveTo(startX, startY-windowRadius);
			ctx.arc(startX, startY, windowRadius, Math.PI * 1.5, Math.PI * 2, false);
			//ctx.moveTo(startX+windowRadius, startY);
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