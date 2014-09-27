#include <iostream>
#include <sstream>
#include <list>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <string>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

//// Header files for X functions

#include <X11/Xlib.h>
#include <X11/Xutil.h>

//// Header files for classes

#include "xinfo.h"
#include "umbrella.h"
#include "raindrop.h"
#include "protege.h"
#include "text.h"

using namespace std;

const int Border = 10;
const int BufferSize = 10;
const int FPS = 30;


//// Function to put out a message on error exits.

void error( string str ) {
  cerr << str << endl;
  exit(0);
}


//// Function to convert int to string

string intToString(int num){
	stringstream strs;
	strs << num;
	string result = strs.str();
	return result;
}


//// Game variables

Umbrella umbrella(100, 100, 50, 50);
Protege protege(50);
vector<Displayable *> dList;
vector<Raindrop *> raindropList;

int score = 0;
Text uiScore(10, 10, "Score: ");
Text uiLives(10, 20, "Lives: ");


//// Initialize X and create a window

void initX(int argc, char *argv[], XInfo &xinfo) {
	XSizeHints hints;
	unsigned long white, black;

   /*
	* Display opening uses the DISPLAY	environment variable.
	* It can go wrong if DISPLAY isn't set, or you don't have permission.
	*/
	xinfo.display = XOpenDisplay( "" );
	if ( !xinfo.display )	{
		error( "Can't open display." );
	}

   /*
	* Find out some things about the display you're using.
	*/
	xinfo.screen = DefaultScreen( xinfo.display );

	white = XWhitePixel( xinfo.display, xinfo.screen );
	black = XBlackPixel( xinfo.display, xinfo.screen );

	hints.x = 100;
	hints.y = 100;
	hints.width = 400;
	hints.height = 500;
	hints.flags = PPosition | PSize;

	xinfo.window = XCreateSimpleWindow(
		xinfo.display,				//// Display where window appears
		DefaultRootWindow( xinfo.display ), //// Window's parent in window tree
		hints.x, hints.y,			//// Upper left corner location
		hints.width, hints.height,	//// Size of the window
		Border,						//// Width of window's border
		black,						//// Window border colour
		white );					//// Window background colour

	XSetStandardProperties(
		xinfo.display,		//// Display containing the window
		xinfo.window,		//// Window whose properties are set
		"rainyday",		//// Window's title
		"rainyday",			//// Icon's title
		None,				//// Pixmap for the icon
		argv, argc,			//// Applications command line args
		&hints );			//// Size hints for the window

	XDefineCursor(xinfo.display, xinfo.window, None);

	/*
	 * Create Graphics Contexts
	 */
	int i = 0;
	xinfo.gc[i] = XCreateGC(xinfo.display, xinfo.window, 0, 0);
	XSetForeground(xinfo.display, xinfo.gc[i], BlackPixel(xinfo.display, xinfo.screen));
	XSetBackground(xinfo.display, xinfo.gc[i], WhitePixel(xinfo.display, xinfo.screen));
	XSetFillStyle(xinfo.display, xinfo.gc[i], FillSolid);
	XSetLineAttributes(xinfo.display, xinfo.gc[i],
	                     1, LineSolid, CapButt, JoinRound);

	//// Reverse Video
	i = 1;
	xinfo.gc[i] = XCreateGC(xinfo.display, xinfo.window, 0, 0);
	XSetForeground(xinfo.display, xinfo.gc[i], WhitePixel(xinfo.display, xinfo.screen));
	XSetBackground(xinfo.display, xinfo.gc[i], BlackPixel(xinfo.display, xinfo.screen));
	XSetFillStyle(xinfo.display, xinfo.gc[i], FillSolid);
	XSetLineAttributes(xinfo.display, xinfo.gc[i],
	                     1, LineSolid, CapButt, JoinRound);

	//// Reverse Video - for raindrop tops
	i = 2;
	xinfo.gc[i] = XCreateGC(xinfo.display, xinfo.window, 0, 0);
	XSetForeground(xinfo.display, xinfo.gc[i], WhitePixel(xinfo.display, xinfo.screen));
	XSetBackground(xinfo.display, xinfo.gc[i], BlackPixel(xinfo.display, xinfo.screen));
	XSetFillStyle(xinfo.display, xinfo.gc[i], FillSolid);
	XSetLineAttributes(xinfo.display, xinfo.gc[i],
	                     1, LineSolid, CapButt, JoinMiter);

	//// Create off screen buffer
	int depth = DefaultDepth(xinfo.display, DefaultScreen(xinfo.display));
	xinfo.pixmap = XCreatePixmap(xinfo.display, xinfo.window,
			hints.width, hints.height, depth); //// Size and *depth* of pixmap
	xinfo.width = hints.width;
	xinfo.height = hints.height;

	XSelectInput(xinfo.display, xinfo.window,
		KeyReleaseMask |
		PointerMotionMask |
		EnterWindowMask | LeaveWindowMask |
		ExposureMask |
		StructureNotifyMask);  //// For resize events

	//// Disable autorepeat keyboard input
	XAutoRepeatOff(xinfo.display);

	/*
	 * Don't paint the background -- reduce flickering
	 */
	XSetWindowBackgroundPixmap(xinfo.display, xinfo.window, None);

	/*
	 * Put the window on the screen.
	 */
	XMapRaised( xinfo.display, xinfo.window );

	XFlush(xinfo.display);
	//sleep(2);	//// Let server get set up before sending drawing commands
}


//// Function to repaint a display list

void repaint( XInfo &xinfo) {
	//XClearWindow( xinfo.display, xinfo.window );

	//// Get height and width of window (might have changed since last repaint)
	XWindowAttributes windowInfo;
	XGetWindowAttributes(xinfo.display, xinfo.window, &windowInfo);
	//unsigned int height = windowInfo.height;
	//unsigned int width = windowInfo.width;

	//// Draw into the buffer
	//// Big black rectangle to clear the background
	XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[0], 0, 0, xinfo.width, xinfo.height);

	//// Draw display list then delete it for next repaint
	for (unsigned int i=0; i < dList.size(); i++) {
		Displayable *d = dList.at(i);
		d->paint(xinfo);
	}
	dList.clear();

	//// Draw raindrops list
	for (unsigned int i=0; i < raindropList.size(); i++) {
		Raindrop *r = raindropList.at(i);
		r->paint(xinfo);
	}

	//// Draw text
	Text uiScoreValue(50, 10, intToString(score));
	Text uiLivesValue(50, 20, intToString(umbrella.getLives()));
	uiScore.paint(xinfo);
	uiScoreValue.paint(xinfo);
	uiLives.paint(xinfo);
	uiLivesValue.paint(xinfo);

	//// Copy buffer to window
	XCopyArea(xinfo.display, xinfo.pixmap, xinfo.window, xinfo.gc[0],
			0, 0, xinfo.width, xinfo.height, //// pixmap region to copy
			0, 0);

	XFlush( xinfo.display );
}


void handleKeyPress(XInfo &xinfo, XEvent &event) {
	KeySym key;
	char text[BufferSize];
	/*
	 * Exit when 'q' is typed.
	 * This is a simplified approach that does NOT use localization.
	 */
	int i = XLookupString(
		(XKeyEvent *)&event, 	//// the keyboard event
		text, 					//// buffer when text will be written
		BufferSize, 			//// size of the text buffer
		&key, 					//// workstation-independent key symbol
		NULL );					//// pointer to a composeStatus structure (unused)
	if (i == 1) {
		printf("Got key press -- %c\n", text[0]);
		if (text[0] == 'q') {
			error("Terminating normally.");
		}
	}
}


void handleMotion(XInfo &xinfo, XEvent &event, int inside) {
	if (inside) {
		umbrella.advance(event.xbutton.x, event.xbutton.y, xinfo);
		dList.push_back(&umbrella);
	}
}


void handleLives(int isMissed) {
	if (isMissed == 1) {
		if (umbrella.getLives() <= 0) {
			error("You lose!\n");
		} else {
			printf("You lose a life! Lives: %d\n", umbrella.getLives());
		}
	} else {
		if (score % 10 == 0) {
			umbrella.incLives();
			printf("You got a life! Lives: %d\n", umbrella.getLives());
		}
	}
}


void handleRaindrops(XInfo &xinfo) {
	//printf("raindropList size: %d\n", raindropList.size());
	if (raindropList.size() < 4 && rand() % 15 == 0) {
		Raindrop* r = new Raindrop(abs((rand() * 100) % xinfo.width), 0,
				10);
		raindropList.push_back(r);
	}

	for (unsigned int i=0; i < raindropList.size(); i++) {
		Raindrop *r = raindropList.at(i);
		r->move();

		if (r->getY() > umbrella.getY()
				&& r->getY() < (umbrella.getY() + (umbrella.getHeight() / 2))
				&& r->getX() > umbrella.getX()
				&& r->getX() < (umbrella.getX() + umbrella.getWidth())) {
			score++;
			printf("Caught! %d Score: %d\n", i, score);
			r->setX(abs((rand() * 100) % xinfo.width));
			r->setY(0);
			handleLives(0);
		} else
		if (r->getY() > xinfo.height - 50) {
			r->setX(abs((rand() * 100) % xinfo.width));
			r->setY(0);
			umbrella.decLives();
			handleLives(1);
		}

	}
}


//// Update width and height when window is resized
void handleResize(XInfo &xinfo, XEvent &event) {
	XConfigureEvent xce = event.xconfigure;
	//fprintf(stderr, "Handling resize  w=%d  h=%d\n", xce.width, xce.height);
	if (xce.width != xinfo.width || xce.height != xinfo.height) {
		XFreePixmap(xinfo.display, xinfo.pixmap);
		int depth = DefaultDepth(xinfo.display, DefaultScreen(xinfo.display));
		xinfo.pixmap = XCreatePixmap(xinfo.display, xinfo.window, xce.width, xce.height, depth);
		xinfo.width = xce.width;
		xinfo.height = xce.height;
	}
}


void handleAnimation(XInfo &xinfo, int inside) {
	dList.push_back(&umbrella);
	dList.push_back(&protege);
	handleRaindrops(xinfo);
	if (!inside) {
		umbrella.advance(umbrella.getX(), umbrella.getY(), xinfo);
	}
	//printf("dList size: %d\n", dList.size());
}


//// Get microseconds
unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}


void eventLoop(XInfo &xinfo) {
	//// Add stuff to paint to the display list
	XEvent event;
	unsigned long lastRepaint = 0;
	int inside = 0;
	int splash = 0;

	while(splash == 0) {
		if (XPending(xinfo.display) > 0) {
			XNextEvent( xinfo.display, &event );
			switch( event.type ) {
				case KeyRelease:
					handleKeyPress(xinfo, event);
					//printf("Event: KeyRelease %d\n", event.type);
					break;
				case MotionNotify:
					handleMotion(xinfo, event, inside);
					//printf("Event: MotionNotify %d\n", event.type);
					break;
				case EnterNotify:
					inside = 1;
					//printf("Event: EnterNotify %d\n", event.type);
					break;
				case LeaveNotify:
					inside = 0;
					//printf("Event: LeaveNotify %d\n", event.type);
					break;
				case ConfigureNotify:
					//printf("Event: Expose %d\n", event.type);
					handleResize(xinfo, event);
					break;
			}
		}
		unsigned long currentTime = now();
		//// Repaint when more than 1/30 seconds have passed
		if ((currentTime - lastRepaint) >= (1000000/FPS)) {
			usleep(1000000/FPS);
			handleAnimation(xinfo, inside);
			repaint(xinfo);
			lastRepaint = now();
		}
		//// Snooze when no events are left
		if (XPending(xinfo.display) == 0) {
			usleep((1000000/FPS) - (currentTime - lastRepaint));
		}
	}

}


/*
 * Start executing here.
 *	 First initialize window.
 *	 Next loop responding to events.
 *	 Exit forcing window manager to clean up - cheesy, but easy.
 */
int main ( int argc, char *argv[] ) {
	XInfo xinfo;
	initX(argc, argv, xinfo);
	eventLoop(xinfo);
	XCloseDisplay(xinfo.display);
}
