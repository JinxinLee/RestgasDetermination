#include $(ROOTSYS)/etc/Makefile.arch
include $(ROOTSYS)/share/root/etc/Makefile.arch
include Makefile.Panda

INCLUDE        += -I.


CXXFLAGS        += $(INCLUDE) $(ROOTCFLAGS) -fPIC 
#-pg GLIBS           = $(ROOTGLIBS) $(LIBS)
# -lMathMore -lMemStat
#LIBS           += -lMathMore
LIBS += $(ROOTLIBS) -lMathMore -lMemStat

#------------------------------------------------------------------------------

all: sim_radmap.exe plot_radmap.exe

sim_radmap.exe: sim_radmap.o
	 $(CXX) $? -o $@ $(LIBS) 

Mesh.o: Mesh.cpp
	 $(CXX) $? -c -o $@ $(INCLUDE) 

plot_radmap.exe: plot_radmap.o Mesh.o
	 $(CXX) $? -o $@ $(LIBS) 



clean:
	rm *.o *.exe

