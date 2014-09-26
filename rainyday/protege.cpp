#include "protege.h"

Protege::Protege(int height):
	height(height) {
		height = 50;
}

void Protege::paint(XInfo &xinfo) {
	XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[1], 
		0, xinfo.height - height,
		xinfo.width, xinfo.height);
}

int Protege::getHeight() {
	return height;
}