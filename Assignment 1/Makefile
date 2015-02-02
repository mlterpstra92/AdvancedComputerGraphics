####### Compiler, tools and options
ifeq ("$(shell uname)", "Linux") 
  OS="Linux"
endif

ifeq ("$(shell uname)", "Darwin")
  OS="MacOSX"
endif

ifndef OS
  $(error Unsupported operating system)
endif

NVSDKINC=	 
NVSDKLIB=       -lCgGL -lCg -lglut -lGLU -lGLEW \
		-lpng -lpthread
CC	=	gcc 
CXX	=	g++ 
REGULAR = 	-O3 -pipe -Wall -fno-strength-reduce -fPIC	
APP_FLAGS=	-Wall -DUNIX
CFLAGS	=	$(APP_FLAGS) 
CXXFLAGS=	$(APP_FLAGS) -g 
INCPATH	=	$(NVSDKINC)
LINK	=	g++
LFLAGS	= 	-fPIC 
TAR	=	tar -cf
GZIP	=	gzip -9f
NASM	=	nasm -f elf 
LIBMESA	= 	$(LIB) -lGLU -lglut 
LIBX	=	-L/usr/X11R6/lib -lX11 -lXmu -lXi   
LIBS	=	-L/usr/lib -L/usr/local/lib -lm 
LIBDIR	=	$(LIBMESA) $(LIBX) $(LIBS) $(NVSDKLIB)

####### Files

include Make.inc

all: $(TARGET)

.SUFFIXES: .cpp .c .asm

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<
.c.o:
	$(CC) -c $(CXXFLAGS) $(INCPATH) -o $@ $<
.asm.o:
	$(NASM) -o $@ $<

$(TARGET): $(OBJECTS) 
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBDIR)

clean:
	-rm -f $(OBJECTS)
	-rm -f *~ core
	-rm -f *.so
	-rm -f $(TARGET)





