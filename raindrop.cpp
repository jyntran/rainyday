//	Raindrop class: what the player needs to catch

#include "raindrop.h"

Raindrop::Raindrop(int x, int y, int fallAmount):
	x(x), y(y), fallAmount(fallAmount) {
}

void Raindrop::paint(XInfo &xinfo) {
	XFillArc(xinfo.display, xinfo.pixmap, xinfo.gc[2],
		x, y, 10, 10, 0, 360*64);
	XFillArc(xinfo.display, xinfo.pixmap, xinfo.gc[2],
		x, y-15, 10, 20, 225*64, 90*64);
}

void Raindrop::move() {
	y += fallAmount;
}

int Raindrop::getX() {
	return x;
}

int Raindrop::getY() {
	return y;
}