#include "anthill.h"

Anthill::Anthill(int height):
	height(height) {
		height = 50;
		rainLevel = 0;
}

void Anthill::paint(XInfo &xinfo) {
	XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[1],
		(xinfo.width/2) - 10, xinfo.height - height,
		20, 20);
	XFillArc(xinfo.display, xinfo.pixmap, xinfo.gc[1],
		(xinfo.width/2) - 50, xinfo.height - (height),
		100, 100,
		0, 180*64);
	//// Rain level
	XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[1], 
		0, xinfo.height - rainLevel,
		xinfo.width, xinfo.height);
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