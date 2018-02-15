#include $(ROOTSYS)/etc/Makefile.arch
include $(ROOTSYS)/share/root/etc/Makefile.arch
include Makefile.Panda

INCLUDE        += -I.

ROOTCFLAGS := $(shell root-config --cflags)
ROOTGLIBS  := $(shell root-config --glibs)
CXXFLAGS        += $(INCLUDE) $(ROOTCFLAGS) -fPIC 
#-pg GLIBS           = $(ROOTGLIBS) $(LIBS)
# -lMathMore -lMemStat
#LIBS           += -lMathMore
LIBS += $(ROOTLIBS) -lMathMore -lMemStat

#------------------------------------------------------------------------------

all: sim_radmap plot_radmap

sim_radmap.exe: sim_radmap.o
	 $(CXX) $? -o $@ $(LIBS) 

PndRadMapBoxMesh.o: PndRadMapBoxMesh.cxx
	 $(CXX) $? -c -o $@ $(INCLUDE) 

plot_radmap.exe: plot_radmap.o PndRadMapBoxMesh.o
	 $(CXX) $? -o $@ $(LIBS) 

histodivider: histodivider.C
	$(CXX) -o $@ $(ROOTCFLAGS) $(ROOTGLIBS) -lHistPainter $?

print-%: ; @echo $* = $($*)
clean:
	rm *.o *.exe

