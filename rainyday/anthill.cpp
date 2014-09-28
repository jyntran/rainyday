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
			//// Anthill ants - white
			XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[2],
				xinfo.width/2 - 61, xinfo.height - 1,
				xinfo.width/2, xinfo.height - (height + 1));
			XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[2],
				xinfo.width/2, xinfo.height - (height + 1),
				xinfo.width/2 + 61, xinfo.height - 1);
			//// Anthill ants - black
			XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[3],
				xinfo.width/2 - 20, xinfo.height - 1,
				xinfo.width/2, xinfo.height - (height + 1));
			XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[3],
				xinfo.width/2, xinfo.height - (height + 1),
				xinfo.width/2 + 20, xinfo.height - 1);
			//// Switch
			antStep = 0;
		} else {
			//// Ground ants alternating
			XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[2],
				2, xinfo.height - 1,
				xinfo.width + 2, xinfo.height - 1);
			//// Anthill ants alternating - white
			XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[2],
				xinfo.width/2 - 59, xinfo.height - 3,
				xinfo.width/2 + 2, xinfo.height - (height + 2));
			XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[2],
				xinfo.width/2 - 2, xinfo.height - (height + 2),
				xinfo.width/2 + 59, xinfo.height - 3);
			//// Anthill ants alternating - black
			XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[3],
				xinfo.width/2 - 17, xinfo.height - 4,
				xinfo.width/2 + 3, xinfo.height - (height + 4));
			XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[3],
				xinfo.width/2 - 3, xinfo.height - (height + 4),
				xinfo.width/2 + 17, xinfo.height - 4);
			//// Switch
			antStep = 1;
		}

	//// Rain level
	XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[0],
		0, xinfo.height - (rainLevel - 3),
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
