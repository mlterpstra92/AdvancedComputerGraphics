CPPFILES    = $(wildcard *.cpp)
OBJECTS     = $(CPPFILES:%.cpp=obj/%.o)
EXECFILE    = sph
LIBS        = -lglui
#Debug flags
CXX         = g++
CPPFLAGS    = -g -std=c++0x -I./include/glui/include -Wno-deprecated -Wno-deprecated-implementations -Wno-deprecated-declarations -Wno-unused-result -fopenmp
LINKFLAGS   =
#Possible flags for release (werkt 16-12-2013, visualisatie kan het aan)
CPPFLAGS   += -O3 -march=x86-64 -mtune=generic -DNDEBUG -mfpmath=sse -ffast-math -Wall -pipe
LINKFLAGS  += -O3 -fopenmp

OS = $(shell uname -s)
ifeq ($(OS),Linux)
	LIBS += -lCgGL -lCg -lglut -lGLU -lGL  -lGLEW
	LINKFLAGS += -L./include/glui/lib/linux
else
	LIBS += -framework Glut -framework OpenGL -framework Cg -lglew
	LINKFLAGS += -L./include/glui/lib/mac
endif

all: $(EXECFILE)

$(EXECFILE): $(OBJECTS)
	$(CXX) $(LINKFLAGS) $(OBJECTS) -o $(EXECFILE) $(LIBS)

obj/%.o: %.cpp obj/.sentinel
	$(CXX) -MMD $(CPPFLAGS) $(INCLUDEDIRS) -c -o $@ $<

obj/.sentinel:
	mkdir -p $(dir $(OBJECTS))
	touch $@

clean:
	-rm -rf obj $(EXECFILE)


DEPS := $(OBJECTS:.o=.d)

-include $(DEPS)
