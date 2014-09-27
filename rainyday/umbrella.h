//	Umbrella class: what the player controls.

#ifndef umbrella_h
#define umbrella_h

#include "xinfo.h"
#include "displayable.h"

class Umbrella : public Displayable {
	public:
		Umbrella(int x, int y, int width, int height);
		void paint(XInfo &xinfo);
		void advance(int x, int y, XInfo &xinfo);
		int getX();
		int getY();
		int getWidth();
		int getHeight();
		int getAdvAmount();

	private:
		int x;
		int y;
		int width;
		int height;
		int adv_x;
		int adv_y;
		int advAmount;
};

#endif