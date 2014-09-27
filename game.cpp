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

//	Header files for X functions

#include <X11/Xlib.h>
#include <X11/Xutil.h>

//	Header files for classes

#include "xinfo.h"
#include "umbrella.h"
#include "raindrop.h"
#include "protege.h"
#include "text.h"

using namespace std;

const int Border = 10;
const int BufferSize = 10;
const int FPS = 24;


//	Function to put out a message on error exits.

void error( string str ) {
  cerr << str << endl;
  exit(0);
}


//	Convert int to string

string intToString(int num){
	stringstream strs;
	strs << num;
	string temp_str = strs.str();
//	char* char_type = (char*) temp_str.c_str();
//	return char_type;
	return temp_str;
}


//	Game variables

Umbrella umbrella(100, 100, 50, 50);
Protege protege(50);
vector<Displayable *> dList;
vector<Raindrop *> raindropList;

int score = 0;
Text uiScore(10, 10, "Score: ");
Text uiLives(10, 20, "Lives: ");


//	Initialize X and create a window

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
		xinfo.display,				// display where window appears
		DefaultRootWindow( xinfo.display ), // window's parent in window tree
		hints.x, hints.y,			// upper left corner location
		hints.width, hints.height,	// size of the window
		Border,						// width of window's border
		black,						// window border colour
		white );					// window background colour

	XSetStandardProperties(
		xinfo.display,		// display containing the window
		xinfo.window,		// window whose properties are set
		"rainyday",		// window's title
		"rainyday",			// icon's title
		None,				// pixmap for the icon
		argv, argc,			// applications command line args
		&hints );			// size hints for the window

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

	// Reverse Video
	i = 1;
	xinfo.gc[i] = XCreateGC(xinfo.display, xinfo.window, 0, 0);
	XSetForeground(xinfo.display, xinfo.gc[i], WhitePixel(xinfo.display, xinfo.screen));
	XSetBackground(xinfo.display, xinfo.gc[i], BlackPixel(xinfo.display, xinfo.screen));
	XSetFillStyle(xinfo.display, xinfo.gc[i], FillSolid);
	XSetLineAttributes(xinfo.display, xinfo.gc[i],
	                     1, LineSolid, CapButt, JoinRound);

	// Reverse Video - for raindrop tops
	i = 2;
	xinfo.gc[i] = XCreateGC(xinfo.display, xinfo.window, 0, 0);
	XSetForeground(xinfo.display, xinfo.gc[i], WhitePixel(xinfo.display, xinfo.screen));
	XSetBackground(xinfo.display, xinfo.gc[i], BlackPixel(xinfo.display, xinfo.screen));
	XSetFillStyle(xinfo.display, xinfo.gc[i], FillSolid);
	XSetLineAttributes(xinfo.display, xinfo.gc[i],
	                     1, LineSolid, CapButt, JoinMiter);

	// create off screen buffer
	int depth = DefaultDepth(xinfo.display, DefaultScreen(xinfo.display));
	xinfo.pixmap = XCreatePixmap(xinfo.display, xinfo.window,
			hints.width, hints.height, depth); // size and *depth* of pixmap
	xinfo.width = hints.width;
	xinfo.height = hints.height;

	XSelectInput(xinfo.display, xinfo.window,
		KeyPressMask |
		PointerMotionMask |
//		EnterWindowMask | LeaveWindowMask |
		ExposureMask |
		StructureNotifyMask);  // for resize events

	// turn off keyboard autorepeat
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
	//sleep(2);	// let server get set up before sending drawing commands
}


//	Function to repaint a display list

void repaint( XInfo &xinfo) {
	//XClearWindow( xinfo.display, xinfo.window );

	// get height and width of window (might have changed since last repaint)
	// XWindowAttributes windowInfo;
	// XGetWindowAttributes(xinfo.display, xinfo.window, &windowInfo);
	//unsigned int height = windowInfo.height;
	//unsigned int width = windowInfo.width;

	// draw into the buffer
	// big black rectangle to clear the background
	XFillRectangle(xinfo.display, xinfo.pixmap, xinfo.gc[0], 0, 0, xinfo.width, xinfo.height);

	// draw display list
	for (unsigned int i=0; i < dList.size(); i++) {
		Displayable *d = dList.at(i);
		d->paint(xinfo);
	}

	// draw raindrops list
	for (unsigned int i=0; i < raindropList.size(); i++) {
		Raindrop *r = raindropList.at(i);
		r->paint(xinfo);
	}

	// draw text
	Text uiScoreValue(50, 10, intToString(score));
	Text uiLivesValue(50, 20, intToString(umbrella.getLives()));

	uiScore.paint(xinfo);
	uiScoreValue.paint(xinfo);
	uiLives.paint(xinfo);
	uiLivesValue.paint(xinfo);

	// copy buffer to window
	XCopyArea(xinfo.display, xinfo.pixmap, xinfo.window, xinfo.gc[0],
			0, 0, xinfo.width, xinfo.height, // pixmap region to copy
			0, 0);

	XFlush( xinfo.display );
}


void handleButtonPress(XInfo &xinfo, XEvent &event) {
	//printf("Got button press!\n");
	// dList.push_front(new Text(event.xbutton.x, event.xbutton.y, "Urrp!"));
	// repaint( dList, xinfo );
}


void handleKeyPress(XInfo &xinfo, XEvent &event) {
	KeySym key;
	char text[BufferSize];
	/*
	 * Exit when 'q' is typed.
	 * This is a simplified approach that does NOT use localization.
	 */
	int i = XLookupString(
		(XKeyEvent *)&event, 	// the keyboard event
		text, 					// buffer when text will be written
		BufferSize, 			// size of the text buffer
		&key, 					// workstation-independent key symbol
		NULL );					// pointer to a composeStatus structure (unused)
	if (i == 1) {
		printf("Got key press -- %c\n", text[0]);
		if (text[0] == 'q') {
			error("Terminating normally.");
		}
	}
}


void handleMotion(XInfo &xinfo, XEvent &event, int inside) {
//	if (inside) {
		umbrella.advance(event.xbutton.x, event.xbutton.y, xinfo);
		dList.push_back(&umbrella);
//	}
}


void handleRaindrops(XInfo &xinfo) {
	if (raindropList.size() < 5 && rand() % 15 == 0) {
		Raindrop* r = new Raindrop(abs((rand() * 100) % xinfo.width), 0,
				5 + (5 * floor((score / 10))));
		raindropList.push_back(r);
	}

	for (unsigned int i=0; i < raindropList.size(); i++) {
		Raindrop *r = raindropList.at(i);
		r->move();
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


void handleCollisions(XInfo &xinfo) {
	for (unsigned int i=0; i < raindropList.size(); i++) {
		Raindrop *r = raindropList.at(i);
		if (r->getY() > umbrella.getY()
				&& r->getY() < (umbrella.getY() + (umbrella.getHeight() / 2))
				&& r->getX() > umbrella.getX()
				&& r->getX() < (umbrella.getX() + umbrella.getWidth())) {
			score++;
			printf("Caught! Score: %d\n", score);
			handleLives(0);
			raindropList.erase(raindropList.begin() + i);
		} else
		if (r->getY() > xinfo.height - 50) {
			raindropList.erase(raindropList.begin() + i);
			umbrella.decLives();
			handleLives(1);
		}
	}
}


//	update width and height when window is resized
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
	dList.push_back(&protege);
	handleRaindrops(xinfo);
	handleCollisions(xinfo);
	if (!inside) {
		umbrella.advance(umbrella.getX(), umbrella.getY(), xinfo);
	}
}


//	get microseconds
unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}


void eventLoop(XInfo &xinfo) {
	// Add stuff to paint to the display list
	XEvent event;
	unsigned long lastRepaint = 0;
	int inside = 0;

	while( true ) {
		/*
		 * This is NOT a performant event loop!
		 * It needs help!
		 */
		if (XPending(xinfo.display) > 0) {
			XNextEvent( xinfo.display, &event );
			printf("New event: %d\n", event.type);
			switch( event.type ) {
				case KeyPress:
					handleKeyPress(xinfo, event);
					printf("Event: KeyPress %d\n", event.type);
					break;
				case MotionNotify:
					handleMotion(xinfo, event, inside);
					printf("Event: MotionNotify %d\n", event.type);
					break;
/*				case EnterNotify:
					inside = 1;
					printf("Event: EnterNotify %d\n", event.type);
					break;
				case LeaveNotify:
					inside = 0;
					printf("Event: LeaveNotify %d\n", event.type);
					break;
*/				case Expose:
					printf("Event: Expose %d\n", event.type);
					break;
				case ConfigureNotify:
					handleResize(xinfo, event);
					printf("Event: ConfigureNotify %d\n", event.type);
					break;
			}
		}
		unsigned int currentTime = now();
		if ((currentTime - lastRepaint) > (1000000/FPS)) {
			handleAnimation(xinfo, inside);
			repaint(xinfo);
			lastRepaint = now();
		}
		if (XPending(xinfo.display) == 0) {
			repaint(xinfo);
			usleep(1000000/FPS);
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
