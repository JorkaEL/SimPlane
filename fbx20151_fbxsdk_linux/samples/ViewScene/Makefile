SHELL       = /bin/sh

# default config:
#    release
#    x86
#    shared
#    
#    to build 64 bit:                make -e M64=-m64
#    to build static:                make -e STATIC_LINK=1
#    to build 64 bit static debug:   make -e M64=-m64 STATIC_LINK=1 VARIANT=debug
#

VARIANT      = release
ifeq "$(VARIANT)" "debug"
    GCC_DEBUG_FLAGS += -g -D_DEBUG
endif

M64 = -m32
ARCH = x86
S64  =
EXTRA_LIBS=-lglut_gcc32 -lGLEW

ifeq "$(M64)" "-m64"
    ARCH=x64
    S64 = 64
    EXTRA_LIBS=-lglut_gcc34-amd64 -lGLEW_amd64 
endif
  
EXAMPLE     = ViewScene
BINDIR      = ../../bin/$(ARCH)/$(VARIANT)/$(EXAMPLE)
OBJDIR      = ../../obj/$(ARCH)/$(VARIANT)/$(EXAMPLE)
LIBDIR      = ../../lib/gcc4/$(ARCH)/$(VARIANT)/ -L. -L/usr/lib$(S64) -L/usr/X11R6/lib$(S64)
INCDIR      = ../../include
COMMONDIR   = ../Common

TARGET      = $(BINDIR)/$(EXAMPLE)
OBJS  = \
	$(COMMONDIR)/Common.o\
	./DrawScene.o\
	./DrawText.o\
	./GetPosition.o\
	./SceneCache.o\
	./SceneContext.o\
	./GlFunctions.o\
	./SetCamera.o\
	./targa.o\
	./main.o

OBJSFINAL  = \
	$(OBJDIR)/Common.o\
	$(OBJDIR)/DrawScene.o\
	$(OBJDIR)/DrawText.o\
	$(OBJDIR)/GetPosition.o\
	$(OBJDIR)/SceneCache.o\
	$(OBJDIR)/SceneContext.o\
	$(OBJDIR)/GlFunctions.o\
	$(OBJDIR)/SetCamera.o\
	$(OBJDIR)/targa.o\
	$(OBJDIR)/main.o

CC          = gcc4 
LD          = gcc4 
RM          = /bin/rm -rf

CXXFLAGS    = $(M64) $(GCC_DEBUG_FLAGS)
LDFLAGS     = $(M64) $(GCC_LINK_FLAGS)

RPATH_DEF    =
STATIC_LINK  =
FBXSDK_LIB   = -lfbxsdk
ifeq "$(STATIC_LINK)" ""
    CXXFLAGS += -DFBXSDK_SHARED
    RPATH_DEF = -Wl,-rpath $(PWD)/$(LIBDIR)
else
    FBXSDK_LIB = $(LIBDIR)/libfbxsdk.a
endif

LIBS = $(FBXSDK_LIB) -lm -lrt -luuid -lstdc++ -lpthread -ldl $(EXTRA_LIBS) -lGLU -lGL -lXmu 

.SUFFIXES: .cxx

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(BINDIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJSFINAL) -L$(LIBDIR) $(LIBS) $(RPATH_DEF)
	cp *.fbx $(BINDIR)
	cp FBX_ASCII_TEXTURE $(BINDIR)

$(COMMONDIR)/Common.o: $(COMMONDIR)/Common.cxx
	mkdir -p $(OBJDIR)
	$(CC) $(CXXFLAGS) -I$(INCDIR) -c $(COMMONDIR)/Common.cxx -o $(OBJDIR)/Common.o

.c.o:
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $*.o 
	mv $*.o $(OBJDIR)

.cxx.o:
	mkdir -p $(OBJDIR)
	$(CC) $(CXXFLAGS) -I$(INCDIR) -c $< -o $*.o
	mv $*.o $(OBJDIR)

clean:
	rm -rf $(BINDIR)
	rm -rf $(OBJDIR)

