////
//// rainyday
//// CS 349 Assignment 1
//// Jen Tran - j44tran
////


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
#include "anthill.h"
#include "cloud.h"
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


//// Get microseconds

unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}


//// Game variables

Umbrella umbrella(100, 100, 50, 50);
Anthill anthill(50);
Cloud cloud(0);
vector<Displayable *> dList;
vector<Raindrop *> raindropList;

int score = 0;
Text uiScore(20, 20, "Score: ");

int splash = 1;
int splashPainted = 0;

int tutormode = 0;


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

	//// Reverse Video - dashed line for ants - white
	i = 2;
	xinfo.gc[i] = XCreateGC(xinfo.display, xinfo.window, 0, 0);
	XSetForeground(xinfo.display, xinfo.gc[i], WhitePixel(xinfo.display, xinfo.screen));
	XSetBackground(xinfo.display, xinfo.gc[i], BlackPixel(xinfo.display, xinfo.screen));
	XSetFillStyle(xinfo.display, xinfo.gc[i], FillSolid);
	XSetLineAttributes(xinfo.display, xinfo.gc[i],
	                     1, LineOnOffDash, CapButt, JoinMiter);

	//// Reverse Video - dashed line for ants - black
	i = 3;
	xinfo.gc[i] = XCreateGC(xinfo.display, xinfo.window, 0, 0);
	XSetForeground(xinfo.display, xinfo.gc[i], BlackPixel(xinfo.display, xinfo.screen));
	XSetBackground(xinfo.display, xinfo.gc[i], WhitePixel(xinfo.display, xinfo.screen));
	XSetFillStyle(xinfo.display, xinfo.gc[i], FillSolid);
	XSetLineAttributes(xinfo.display, xinfo.gc[i],
	                     1, LineOnOffDash, CapButt, JoinMiter);

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
}


//// Function to repaint a display list

void repaint( XInfo &xinfo) {
	//// Get height and width of window (might have changed since last repaint)
	XWindowAttributes windowInfo;
	XGetWindowAttributes(xinfo.display, xinfo.window, &windowInfo);

	//// Draw into the buffer
	//// Big black rectangle to clear the background
	XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[0], 0, 0, xinfo.width, xinfo.height);

	//// Draw display list then delete it for next repaint
	//// Umbrella, Anthill, Clouds
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
	Text uiScoreValue(100, 20, intToString(score));
	uiScore.paint(xinfo);
	uiScoreValue.paint(xinfo);

	//// Copy buffer to window
	XCopyArea(xinfo.display, xinfo.pixmap, xinfo.window, xinfo.gc[0],
			0, 0, xinfo.width, xinfo.height, //// pixmap region to copy
			0, 0);

	XFlush( xinfo.display );
}


//// Function to paint the splash screen

void splashPaint( XInfo &xinfo) {
	//// Get height and width of window (might have changed since last repaint)
	XWindowAttributes windowInfo;
	XGetWindowAttributes(xinfo.display, xinfo.window, &windowInfo);

	//// Big black rectangle to clear the background
	XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[0], 0, 0, xinfo.width, xinfo.height);

	//// Splash and instructions
	Text uiSplashLine1(20, 20, "rainyday");
	Text uiSplashLine2(40, 60, "Protect the anthills from drowning in the rain.");
	Text uiSplashLine3(40, 80, "Use the mouse to control the umbrella.");
	Text uiSplashLine4(40, 100, "If the water reaches the top of the anthills, game over.");
	Text uiSplashLine5(40, 140, "Press B to begin.");
	Text uiSplashLine6(40, 160, "Press T to toggle tutor grade mode.");
	Text uiSplashLine7(40, 180, "Press Q to close the game.");
	Text uiSplashLine8(20, 220, "Created by Jen Tran - j44tran");

	uiSplashLine1.paint(xinfo);
	uiSplashLine2.paint(xinfo);
	uiSplashLine3.paint(xinfo);
	uiSplashLine4.paint(xinfo);
	uiSplashLine5.paint(xinfo);
	uiSplashLine6.paint(xinfo);
	uiSplashLine7.paint(xinfo);
	uiSplashLine8.paint(xinfo);

	//// Copy buffer to window
	XCopyArea(xinfo.display, xinfo.pixmap, xinfo.window, xinfo.gc[0],
			0, 0, xinfo.width, xinfo.height, //// pixmap region to copy
			0, 0);

	printf("Window loaded.\n");

	XFlush( xinfo.display );
}


//// Handle key presses

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
		if (text[0] == 'q') {
			error("Terminating normally.");
		}
		if (text[0] == 'b') {
			printf("Begin game.\n");
			splash = 0;
		}
		if (text[0] == 't') {
			tutormode = tutormode ? 0 : 1;
			printf("Tutor mode %s.\n", tutormode ? "on" : "off");
			//// Update all raindrops to appropriate speed
			for (unsigned int i=0; i < raindropList.size(); i++) {
				Raindrop *r = raindropList.at(i);
				r->setFallAmount(tutormode);
			}
		}
	}
}


//// Handle mouse motion and umbrella position

void handleMotion(XInfo &xinfo, XEvent &event, int inside) {
	if (inside) {
		umbrella.advance(event.xbutton.x, event.xbutton.y, xinfo);
		dList.push_back(&umbrella);
	}
}


//// Function to check whether the game is won or lost

void handleWinLose(XInfo &xinfo) {
	if (score >= 100) {
		printf("The sun finally came out.\n");
		printf("You are the saviour of the ants.\nYour score: %d\n", score);
		error("Thanks for playing!");
	} else if (anthill.getRainLevel() > anthill.getHeight()) {
		printf("You couldn't save the ants.\nYour score: %d\n", score);
		error("Game over.");
	}
}


//// Handle collision detection for raindrops

void handleRaindropCollision(XInfo &xinfo, Raindrop *r) {
	//// Collision detection with umbrella
	if (r->getY() > umbrella.getY()
			&& r->getY() < (umbrella.getY() + (umbrella.getHeight() / 2))
			&& r->getX() > umbrella.getX()
			&& r->getX() < (umbrella.getX() + umbrella.getWidth())) {
		score++;
		handleWinLose(xinfo);
		//// Reassign x,y values to reuse raindrop
		r->setX(abs((rand() * 100) % xinfo.width));
		r->setY(0);
	} else
		//// Collision detection with ground
		if (r->getY() > xinfo.height - anthill.getRainLevel()) {
			//// Reassign x,y values to reuse raindrop
			r->setX(abs((rand() * 100) % xinfo.width));
			r->setY(0);
			anthill.incRainLevel();
			handleWinLose(xinfo);
		}
}


//// Main function to handle raindrops

void handleRaindrops(XInfo &xinfo) {
	//// Add raindrop until there are 6 onscreen simultaneously
	if (raindropList.size() < 6 && rand() % 15 == 0) {
		Raindrop* r = new Raindrop(abs((rand() * 100) % xinfo.width), 0, tutormode);
		raindropList.push_back(r);
	}
	//// Iterate through each raindrop
	for (unsigned int i=0; i < raindropList.size(); i++) {
		Raindrop *r = raindropList.at(i);
		r->move();
		handleRaindropCollision(xinfo, r);
	}
}


//// Update width and height when window is resized
void handleResize(XInfo &xinfo, XEvent &event) {
	XConfigureEvent xce = event.xconfigure;
	if (xce.width != xinfo.width || xce.height != xinfo.height) {
		XFreePixmap(xinfo.display, xinfo.pixmap);
		int depth = DefaultDepth(xinfo.display, DefaultScreen(xinfo.display));
		xinfo.pixmap = XCreatePixmap(xinfo.display, xinfo.window, xce.width, xce.height, depth);
		xinfo.width = xce.width;
		xinfo.height = xce.height;
	}
}


//// Animation setup and calls

void handleAnimation(XInfo &xinfo, int inside) {
	dList.push_back(&cloud);
	dList.push_back(&umbrella);
	dList.push_back(&anthill);
	handleRaindrops(xinfo);
	if (!inside) {
		umbrella.advance(umbrella.getX(), umbrella.getY(), xinfo);
	}
}


//// Main event loop

void eventLoop(XInfo &xinfo) {
	//// Add stuff to paint to the display list
	XEvent event;
	unsigned long lastRepaint = 0;
	int inside = 0;

	while(true) {
		if (XPending(xinfo.display) > 0) {
			XNextEvent( xinfo.display, &event );
			switch( event.type ) {
				case KeyRelease:
					handleKeyPress(xinfo, event);
					break;
				case MotionNotify:
					handleMotion(xinfo, event, inside);
					break;
				case EnterNotify:
					inside = 1;
					break;
				case LeaveNotify:
					inside = 0;
					break;
				case ConfigureNotify:
					handleResize(xinfo, event);
					break;
			}
		}

		unsigned long currentTime = now();
		//// Repaint when more than 1/30 seconds have passed
		if ((currentTime - lastRepaint) >= (1000000/FPS)) {
			usleep(1000000/FPS);
			if (splashPainted == 0) {
				splashPaint(xinfo);
				splashPainted = 1;
			} else if (splash == 0) {
				handleAnimation(xinfo, inside);
				repaint(xinfo);
			}
		}
		lastRepaint = now();
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
	printf("Loading window...\n");
	eventLoop(xinfo);
	XCloseDisplay(xinfo.display);
}
