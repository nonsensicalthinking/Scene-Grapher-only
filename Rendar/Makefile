CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =	src/*.cpp
INCS = -I"/usr/include/GL" -I"./headers/"
LIBS = 	-lGL -lGLU -lglut

TARGET = rendarar

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) $(INCS)

all:	$(TARGET)

clean:
	rm -f src/*.o $(TARGET)

