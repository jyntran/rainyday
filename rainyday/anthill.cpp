#include "anthill.h"

Anthill::Anthill(int height):
	height(height) {
		height = 50;
		rainLevel = 0;
		antStep = 0;
}

void Anthill::paint(XInfo &xinfo) {

	//// Hill
	XFillArc(xinfo.display, xinfo.pixmap, xinfo.gc[1],
		(xinfo.width/2) - 100, xinfo.height - (height*3),
		200, 200,
		220*64, 100*64);

	//// Ants
		if (antStep) {
			//// Ground ants
			XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[2],
				0, xinfo.height - 1,
				xinfo.width, xinfo.height - 1);
			//// Anthill ants
			XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[2],
				xinfo.width/2 - 61, xinfo.height - 1,
				xinfo.width/2, xinfo.height - (height + 1));
			XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[2],
				xinfo.width/2, xinfo.height - (height + 1),
				xinfo.width/2 + 61, xinfo.height - 1);
			//// Switch
			antStep = 0;
		} else {
			//// Ground ants alternating
			XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[2],
				2, xinfo.height - 1,
				xinfo.width + 2, xinfo.height - 1);
			//// Anthill ants alternating
			XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[2],
				xinfo.width/2 - 59, xinfo.height - 3,
				xinfo.width/2 + 2, xinfo.height - (height + 2));
			XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[2],
				xinfo.width/2 - 2, xinfo.height - (height + 2),
				xinfo.width/2 + 59, xinfo.height - 3);
			//// Switch
			antStep = 1;
		}

	//// Rain level
	XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[0], 
		0, xinfo.height - (rainLevel - 4),
		xinfo.width, xinfo.height);

	//// Waves
	for (int i = 0; i < xinfo.width/100 + 100; i++) {
		XDrawArc(xinfo.display, xinfo.pixmap, xinfo.gc[1],
			 i*100, xinfo.height - (rainLevel + 12),
			100, 15,
			180*64, 180*64);
	}
	for (int i = 0; i < xinfo.width/100 + 100; i++) {
		XDrawArc(xinfo.display, xinfo.pixmap, xinfo.gc[1],
			 -50 + (i*100), xinfo.height - (rainLevel + 1),
			100, 15,
			180*64, 180*64);
	}

/*
	XFillArc(xinfo.display, xinfo.pixmap, xinfo.gc[1],
		xinfo.width/4, xinfo.height - (rainLevel + 5),
		75, 75,
		0, 180*64);
	XFillArc(xinfo.display, xinfo.pixmap, xinfo.gc[1],
		3*xinfo.width/4, xinfo.height - (rainLevel + 5),
		75, 75,
		0, 180*64);
	XFillArc(xinfo.display, xinfo.pixmap, xinfo.gc[1],
		xinfo.width+50, xinfo.height - (rainLevel + 10),
		100, 100,
		0, 180*64);
*/
}

int Anthill::getHeight() {
	return height;
}

int Anthill::getRainLevel() {
	return rainLevel;
}

void Anthill::incRainLevel() {
	rainLevel += 2;
}

int Anthill::getAntStep(){
	return antStep;
}