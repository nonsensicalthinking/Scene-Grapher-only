CXXFLAGS = -g
# -Wall -fmessage-length=0 -O2

OBJS =	src/*.cpp
INCS = -I"C:\MinGW\include\GL" -I"./headers/" -I"./game/"
LIBS = -lglu32 -lfreeglut -lopengl32 -Wl,--subsystem,windows

TARGET = rendarar.exe

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) $(INCS) $(CXXFLAGS) 

mod:
	$(CXX) -c -DMyGame_dll -I"./headers/" gameMod/MyGame.cpp -o MyGame.o
	$(CXX) -c -I"./headers/" src/Game.cpp -o Game.o
	$(CXX) -c -I"./headers/" src/bsptree.cpp -o bsptree.o
	$(CXX) -shared -o MyGame.dll MyGame.o Game.o bsptree.o $(LIBS)

all:	$(TARGET) mod

clean:
	rm -f src/*.o $(TARGET)
	rm -f Game.o MyGame.o MyGame.dll

	