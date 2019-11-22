CPP = g++
OPT = -O3 -std=c++17
INC = -I inc

OBJEXIF = obj/ExifTool.o obj/ExifToolPipe.o obj/TagInfo.o
OBJ = obj/command_line.o obj/psqlConnection.o
HDREXIF = inc/ExifTool.h inc/ExifToolPipe.h inc/TagInfo.h
HDR = inc/anitags.h inc/command_line.h inc/config.h inc/error_codes.h inc/psqlConnection.h

all: anitags

debug: DEBUG = -DDEBUG -g
debug: anitags

anitags: obj/anitags.o $(OBJ) $(OBJEXIF)
	$(CPP) $(DEBUG) $(OPT) -o anitags obj/anitags.o $(OBJ) $(OBJEXIF) -lstdc++fs -l pqxx

obj/anitags.o: src/anitags.cpp $(HDR) $(HDREXIF)
	$(CPP) $(DEBUG) $(OPT) $(INC) -o $@ -c src/anitags.cpp -lstdc++fs -l pqxx

obj/%.o: lib/%.cpp $(HDREXIF)
	$(CPP) $(OPT) $(INC) -o $@ -c $<

obj/%.o: src/%.cpp $(HDR)
	$(CPP) $(OPT) $(INC) -o $@ -c $<

clean:
	rm -f anitags obj/*.o

install:
	cp anitags /opt/anitags

remove:
	rm /opt/anitags
