CPP = g++
OPT = -O3
INC = -I inc

OBJ = obj/ExifTool.o obj/ExifToolPipe.o obj/TagInfo.o
HDR = inc/ExifTool.h inc/ExifToolPipe.h inc/TagInfo.h

all: anitags

debug: DEBUG = -DDEBUG -g
debug: anitags

anitags: obj/anitags.o $(OBJ)
	$(CPP) $(DEBUG) $(OPT) -o anitags obj/anitags.o $(OBJ)

obj/anitags.o: src/* $(HDR)
	$(CPP) $(DEBUG) $(OPT) $(INC) -o $@ -c src/anitags.cpp

obj/%.o: lib/%.cpp $(HDR)
	$(CPP) $(OPT) $(INC) -o $@ -c $<

clean:
	rm -f anitags obj/*.o

install:
	cp anitags /opt/anitags

remove:
	rm /opt/anitags
