CC=g++
CFLAGS=-c -Wall
LDFLAGS=-L/usr/X11R6/lib
LIBFLAGS = -lX11 -lstdc++
SOURCES=umbrella.cpp raindrop.cpp anthill.cpp cloud.cpp text.cpp game.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=game

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) ${LIBFLAGS} -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
