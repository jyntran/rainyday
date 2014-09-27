/*
 * Information to draw on the window.
 */

#ifndef xinfo_h
#define xinfo_h

//	Header files for X functions

#include <X11/Xlib.h>
#include <X11/Xutil.h>

struct XInfo {
	Display *display;
	int screen;
	Window window;
	GC gc[3];
	Pixmap pixmap;		// double buffer
	int width;		// size of window
	int height;
};

#endif