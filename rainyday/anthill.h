#ifndef anthill_h
#define anthill_h

#include "xinfo.h"
#include "displayable.h"

class Anthill : public Displayable {
	public:
		Anthill(int height);
		void paint(XInfo &xinfo);
		int getHeight();
		int getRainLevel();
		void incRainLevel();

	private:
		int height;
		int rainLevel;
};

#endif