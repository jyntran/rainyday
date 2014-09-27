/*
 * An abstract class representing displayable things. 
 */

#ifndef displayable_h
#define displayable_h

#include "xinfo.h"

class Displayable {
	public:
		virtual void paint(XInfo &xinfo) = 0;
};

#endif