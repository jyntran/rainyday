#include "cloud.h"

Cloud::Cloud(int x):
	x(-50) {
	}

void Cloud::paint(XInfo &xinfo) {
	for (int i=0; i < xinfo.width; i++) {
		XDrawArc(xinfo.display, xinfo.pixmap, xinfo.gc[1],
			x+(i*100), y-100, 150, 150, 0, 360*64);
	}
	for (int i=0; i < xinfo.width; i++) {
		XFillArc(xinfo.display, xinfo.pixmap, xinfo.gc[0],
			x+(i*100 + 1), y-101, 149, 149, 0, 360*64);
	}
}

int Cloud::getX() {
	return x;
}

int Cloud::getY() {
	return y;
}

int Cloud::getWidth() {
	return width;
}

int Cloud::getHeight() {
	return height;
}
