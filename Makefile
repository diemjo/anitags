CPP = g++
OPT = -O3
INC = -I inc

OBJ = obj/ExifTool.o obj/ExifToolPipe.o obj/TagInfo.o
HDR = inc/ExifTool.h inc/ExifToolPipe.h inc/TagInfo.h

all: anitags

clean:
	rm -f anitags obj/*.o

anitags: obj/anitags.o $(OBJ)
	$(CPP) $(OPT) -o anitags obj/anitags.o $(OBJ)

obj/anitags.o: src/anitags.cpp $(HDR)
	$(CPP) $(OPT) $(INC) -o $@ -c $<

obj/%.o: lib/%.cpp $(HDR)
	$(CPP) $(OPT) $(INC) -o $@ -c $<
