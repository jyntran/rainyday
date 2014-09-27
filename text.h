#ifndef text_h
#define text_h

#include <string>
#include "xinfo.h"
#include "displayable.h"

class Text : public Displayable {  
	public:
		Text(int x, int y, std::string s);
		void paint(XInfo &xinfo);

	private:
		int x;
		int y;
		std::string s;
};

#endif
