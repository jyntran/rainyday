//	Umbrella class: what the player controls.

#include "umbrella.h"

Umbrella::Umbrella(int x, int y, int width, int height):
	x(x), y(y), width(width), height(height)  {
		adv_x = x;
		adv_y = y;
		lives = 3;
		advAmount = 30;
}

void Umbrella::paint(XInfo &xinfo) {
	XFillArc(xinfo.display, xinfo.pixmap, xinfo.gc[1],
			adv_x, adv_y,
			width, height, 0, 180*64);
	XDrawLine(xinfo.display, xinfo.pixmap, xinfo.gc[1],
			adv_x + (width/2), adv_y + (height/2),
			adv_x + (width/2), adv_y + height);
	XDrawArc(xinfo.display, xinfo.pixmap, xinfo.gc[1],
			adv_x + (width/2), adv_y + (3.5*height)/4,
			width/4, height/4, 180*64, 180*64);
}

void Umbrella::advance(int x, int y, XInfo &xinfo) {
	if (x > (xinfo.width - width))
		adv_x = xinfo.width - width;
	else if (x < 0)
		adv_x = 0;
	else
		adv_x = x;

	if (y > (xinfo.height - height))
		adv_y = xinfo.height - height;
	else if (y < 0)
		adv_y = 0;
	else
		adv_y = y;
}

int Umbrella::getX() {
	return adv_x;
}

int Umbrella::getY() {
	return adv_y;
}

int Umbrella::getWidth() {
	return width;
}

int Umbrella::getHeight() {
	return height;
}

int Umbrella::getLives() {
	return lives;
}

void Umbrella::incLives() {
	lives++;
}

void Umbrella::decLives() {
	lives--;
}

int Umbrella::getAdvAmount() {
	return advAmount;
}