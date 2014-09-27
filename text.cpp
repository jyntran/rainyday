#include "text.h"

Text::Text(int x, int y, std::string s):x(x), y(y), s(s)  {}

void Text::paint(XInfo &xinfo){
	XDrawString(xinfo.display, xinfo.pixmap, xinfo.gc[1],
			x, y, s.c_str(), s.length());
}