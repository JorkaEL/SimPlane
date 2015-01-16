$SHELL       = /bin/sh

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
ifeq "$(M64)" "-m64"
    ARCH=x64
endif
  
EXAMPLE     = ImportScene
BINDIR      = ../../bin/$(ARCH)/$(VARIANT)/$(EXAMPLE)
OBJDIR      = ../../obj/$(ARCH)/$(VARIANT)/$(EXAMPLE)
LIBDIR      = ../../lib/gcc4/$(ARCH)/$(VARIANT)
INCDIR      = ../../include
COMMONDIR   = ../Common

TARGET      = $(BINDIR)/$(EXAMPLE)
OBJS  = \
	$(COMMONDIR)/Common.o\
	./DisplayAnimation.o\
	./DisplayCache.o\
	./DisplayCamera.o\
	./DisplayCommon.o\
	./DisplayGlobalSettings.o\
	./DisplayHierarchy.o\
	./DisplayLight.o\
	./DisplayLink.o\
	./DisplayMarker.o\
	./DisplayMaterial.o\
	./DisplayMesh.o\
	./DisplayNurb.o\
	./DisplayPatch.o\
	./DisplayPivotsAndLimits.o\
	./DisplayPose.o\
	./DisplayShape.o\
	./DisplaySkeleton.o\
	./DisplayTexture.o\
	./DisplayUserProperties.o\
	./DisplayGenericInfo.o\
	./DisplayLodGroup.o\
	./main.o

OBJSFINAL  = \
	$(OBJDIR)/Common.o\
	$(OBJDIR)/DisplayAnimation.o\
	$(OBJDIR)/DisplayCache.o\
	$(OBJDIR)/DisplayCamera.o\
	$(OBJDIR)/DisplayCommon.o\
	$(OBJDIR)/DisplayGlobalSettings.o\
	$(OBJDIR)/DisplayHierarchy.o\
	$(OBJDIR)/DisplayLight.o\
	$(OBJDIR)/DisplayLink.o\
	$(OBJDIR)/DisplayMarker.o\
	$(OBJDIR)/DisplayMaterial.o\
	$(OBJDIR)/DisplayMesh.o\
	$(OBJDIR)/DisplayNurb.o\
	$(OBJDIR)/DisplayPatch.o\
	$(OBJDIR)/DisplayPivotsAndLimits.o\
	$(OBJDIR)/DisplayPose.o\
	$(OBJDIR)/DisplayShape.o\
	$(OBJDIR)/DisplaySkeleton.o\
	$(OBJDIR)/DisplayTexture.o\
	$(OBJDIR)/DisplayUserProperties.o\
	$(OBJDIR)/DisplayGenericInfo.o\
	$(OBJDIR)/DisplayLodGroup.o\
	$(OBJDIR)/main.o

CC          = gcc4 
LD          = gcc4 
RM          = /bin/rm -f

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

LIBS = $(FBXSDK_LIB) -lm -lrt -luuid -lstdc++ -lpthread -ldl

.SUFFIXES: .cxx

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p $(BINDIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJSFINAL) -L$(LIBDIR) $(LIBS) $(RPATH_DEF)

$(COMMONDIR)/Common.o: $(COMMONDIR)/Common.cxx
	mkdir -p $(OBJDIR)
	$(CC) $(CXXFLAGS) -I$(INCDIR) -c $(COMMONDIR)/Common.cxx -o $(OBJDIR)/Common.o

.cxx.o:
	mkdir -p $(OBJDIR)
	$(CC) $(CXXFLAGS) -I$(INCDIR) -c $< -o $*.o
	mv $*.o $(OBJDIR)

clean:
	rm -rf $(BINDIR)
	rm -rf $(OBJDIR)

