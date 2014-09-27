#ifndef protege_h
#define protege_h

#include "xinfo.h"
#include "displayable.h"

class Protege : public Displayable {
	public:
		Protege(int height);
		void paint(XInfo &xinfo);
		int getHeight();

	private:
		int height;
};

#endif