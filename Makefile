SHELL       = /bin/sh

# default config:
#    release
#    x86
#    shared
#    clang
#    
#    to build 64 bit:                make -e M64=-m64
#    to build static:                make -e STATIC_LINK=1
#    to build 64 bit static debug:   make -e M64=-m64 STATIC_LINK=1 VARIANT=debug
#    to build using gcc:             make -e GCC_COMPILER=1
#

VARIANT = release
ifeq "$(VARIANT)" "debug"
	GCC_DEBUG_FLAGS += -g -D_DEBUG
endif 

# Don't leave M64 empty on Mac OS 10.5 and above, the default link architecture 
# is X86_64 but the GLEW lib is only compiled for either X86 or X64 and will
# fail to link
M64 = 
ARCH = x64#x86
ifeq "$(M64)" "-m64"
    ARCH=x64
endif
    
EXAMPLE     = SimPlane
BINDIR      = .
OBJDIR      = .

##### Mettre le chemin vers les include et les libs du sdk
INCDIR      = /home/xavier/Téléchargements/fbx20151_fbxsdk_linux/include
LIBDIR      = /home/xavier/Téléchargements/fbx20151_fbxsdk_linux/lib/gcc4/x64/release

COMMONDIR   = Common

TARGET      = $(BINDIR)/$(EXAMPLE)
OBJS  = \
	$(COMMONDIR)/Common.o\
	./DrawScene.o\
	./GetPosition.o\
	./SceneCache.o\
	./SceneContext.o\
	./glimage.o\
	./Projetwindows.o

CP          = cp -f
RM          = /bin/rm 


UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	CC          = cc 
	LD          = cc 
	STDC        = c++
	COMPILER = clang
	MACOSX_DEPLOYMENT_TARGET = 10.8
	MAC_CFLAGS += -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
	MAC_LDFLAGS +=  -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
	COPY_LIB = $(CP) $(LIBDIR)/libfbxsdk.dylib $(BINDIR)
else
	COMPILER = gcc4
	CC       = gcc
	LD       = gcc
	STDC     = stdc++
	COPY_LIB = $(CP) $(LIBDIR)/libfbxsdk.so $(BINDIR)
endif

SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image

CXXFLAGS    = -pipe $(M64) $(GCC_DEBUG_FLAGS)  $(SDL_CFLAGS) $(MAC_CFLAGS)
CFLAGS      = $(CXXFLAGS) 
LDFLAGS     = $(M64) $(SDL_LDFLAGS) $(MAC_LDFLAGS)


STATIC_LINK  =
FBXSDK_LIB   = -lfbxsdk
ifeq "$(STATIC_LINK)" ""
    CXXFLAGS += -DFBXSDK_SHARED
endif

ifeq ($(UNAME), Darwin)
	LIBS = $(FBXSDK_LIB) -lm -l$(STDC) -liconv -fexceptions -lz -framework GLUT -framework OpenGL -framework Cocoa -framework SystemConfiguration
else
	LIBS = $(FBXSDK_LIB) -lm -lrt -lstdc++ -lpthread -ldl $(EXTRA_LIBS) -lGLU -lGL -lglut 
endif
.SUFFIXES: .cxx

all: $(TARGET)

$(TARGET): $(OBJS)
	@if [ ! -d $(@D) ]; then mkdir -p $(@D) ; fi
	@if [ ! -d $(OBJDIR) ]; then mkdir -p $(OBJDIR) ; fi
	$(LD)  -o $@ $(OBJS) -L$(LIBDIR) -L. $(LIBS) $(LDFLAGS)
#	mv *.o $(OBJDIR)
#	mv $(COMMONDIR)/*.o $(OBJDIR)
#	$(CP) FBX_ASCII_TEXTURE $(BINDIR)
	$(COPY_LIB)

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) -c $< -o $*.o 
    
.cxx.o:
	$(CC) $(CXXFLAGS) -I$(INCDIR) -c $< -o $*.o 

clean:
	$(RM) $(OBJS)	
	$(RM) $(TARGET)
