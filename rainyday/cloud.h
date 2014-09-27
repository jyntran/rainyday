#ifndef cloud_h
#define cloud_h

#include "xinfo.h"
#include "displayable.h"

class Cloud : public Displayable {
	public:
		Cloud(int x);
		void paint(XInfo &xinfo);
		int getX();
		int getY();
		int getWidth();
		int getHeight();

	private:
		int x;
		int y;
		int width;
		int height;
};

#endif