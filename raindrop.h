//	Raindrop class: what the player needs to catch

#ifndef raindrop_h
#define raindrop_h

#include "xinfo.h"
#include "displayable.h"

class Raindrop : public Displayable {
	public:
		Raindrop(int x, int y, int fallAmount);
		void paint(XInfo &xinfo);		
      		void move();
		int getX();
		int getY();

	private:
		int x;
		int y;
		int fallAmount;
};

#endif