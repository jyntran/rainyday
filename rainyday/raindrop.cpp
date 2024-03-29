//	Raindrop class: what the player needs to catch

#include "raindrop.h"

Raindrop::Raindrop(int x, int y, int tutormode):
	x(x), y(y) {
		if (tutormode)
			fallAmount = 2;
		else
			fallAmount = 10;
}

void Raindrop::paint(XInfo &xinfo) {
	XFillArc(xinfo.display, xinfo.pixmap, xinfo.gc[0],
		x, y, 10, 10, 0, 360*64);
	XDrawArc(xinfo.display, xinfo.pixmap, xinfo.gc[1],
		x, y, 10, 10, 0, 360*64);
/*	XFillArc(xinfo.display, xinfo.pixmap, xinfo.gc[2],
		x, y, 10, 10, 0, 360*64);
	XDrawArc(xinfo.display, xinfo.pixmap, xinfo.gc[1],
		x, y-15, 10, 20, 225*64, 90*64);
*/	XFillArc(xinfo.display, xinfo.pixmap, xinfo.gc[2],
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

void Raindrop::setX(int new_x) {
	x = new_x;
}

void Raindrop::setY(int new_y) {
	y = new_y;
}

void Raindrop::setFallAmount(int tutormode) {
	if (tutormode)
		fallAmount = 2;
	else
		fallAmount = 10;
}