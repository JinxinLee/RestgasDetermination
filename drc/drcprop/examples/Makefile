#vpath %.h ../
#vpath %.cxx ../ 
#vpath %.h $(ROOTSYS)/include

INCLFLAGS  = -I$(ROOTSYS)/include -I../
LDLIBS     = $(shell root-config --glibs) \
		-lMathCore
#		-L/home/carsten/src/panda/cbmsoft/tools/root/lib -lMathCore -lMathMore

#LDLIBS     = $(shell root-config --glibs) 
# -lMinuit


SOURCES = $(wildcard ../*.cxx)
HEADERS = $(wildcard ../*.h)
OBJECTS = $(patsubst ../%.cxx,../%.o,$(wildcard ../*.cxx))


CFLAGS = -O2 -DNDEBUG -Wall


all:			test_simple_bar test_simple_bar2 test_disk test_sheet test_cylinder \
			test_focus test_focus2 test_absorption test_test test_titti \
			test_top test_air_lens test_hook test_barrel1 test_barrel1a test_barrel2  \
			test_lens beamtest0809_sim mppc

test_simple_bar:	$(OBJECTS) test_simple_bar.o
			g++ -o test_simple_bar test_simple_bar.o $(OBJECTS) $(LDLIBS)

test_simple_bar.o:	test_simple_bar.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_simple_bar.o \
			-c test_simple_bar.cc

test_simple_bar2:	$(OBJECTS) test_simple_bar2.o
			g++ -o test_simple_bar2 test_simple_bar2.o $(OBJECTS) $(LDLIBS)

test_simple_bar2.o:	test_simple_bar2.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_simple_bar2.o \
			-c test_simple_bar2.cc

test_disk:		$(OBJECTS) test_disk.o
			g++ -o test_disk test_disk.o $(OBJECTS) $(LDLIBS)

test_disk.o:		test_disk.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_disk.o \
			-c test_disk.cc

test_sheet:		$(OBJECTS) test_sheet.o
			g++ -o test_sheet test_sheet.o $(OBJECTS) $(LDLIBS)

test_sheet.o:		test_sheet.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_sheet.o \
			-c test_sheet.cc


test_cylinder:		$(OBJECTS) test_cylinder.o
			g++ -o test_cylinder test_cylinder.o $(OBJECTS) $(LDLIBS)

test_cylinder.o:	test_cylinder.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_cylinder.o \
			-c test_cylinder.cc

test_focus:		$(OBJECTS) test_focus.o
			g++ -o test_focus test_focus.o $(OBJECTS) $(LDLIBS)

test_focus.o:		test_focus.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_focus.o \
			-c test_focus.cc


test_focus2:		$(OBJECTS) test_focus2.o
			g++ -o test_focus2 test_focus2.o $(OBJECTS) $(LDLIBS)

test_focus2.o:		test_focus2.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_focus2.o \
			-c test_focus2.cc


test_absorption:	$(OBJECTS) test_absorption.o
			g++ -o test_absorption test_absorption.o $(OBJECTS) $(LDLIBS)

test_absorption.o:	test_absorption.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_absorption.o \
			-c test_absorption.cc


test_test:		$(OBJECTS) test_test.o
			g++ -o test_test test_test.o $(OBJECTS) $(LDLIBS)

test_test.o:		test_test.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_test.o \
			-c test_test.cc

test_titti:		$(OBJECTS) test_titti.o
			g++ -o test_titti test_titti.o $(OBJECTS) $(LDLIBS)

test_titti.o:		test_titti.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_titti.o \
			-c test_titti.cc

test_top:		$(OBJECTS) test_top.o
			g++ -o test_top test_top.o $(OBJECTS) $(LDLIBS)

test_top.o:		test_top.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_top.o \
			-c test_top.cc

test_air_lens:		$(OBJECTS) test_air_lens.o
			g++ -o test_air_lens test_air_lens.o $(OBJECTS) $(LDLIBS)

test_air_lens.o:	test_air_lens.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_air_lens.o \
			-c test_air_lens.cc

test_hook:		$(OBJECTS) test_hook.o
			g++ -o test_hook test_hook.o $(OBJECTS) $(LDLIBS)

test_hook.o:		test_hook.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_hook.o \
			-c test_hook.cc

test_barrel1:		$(OBJECTS) test_barrel1.o
			g++ -o test_barrel1 test_barrel1.o $(OBJECTS) $(LDLIBS)

test_barrel1.o:		test_barrel1.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_barrel1.o \
			-c test_barrel1.cc

test_barrel1a:		$(OBJECTS) test_barrel1a.o
			g++ -o test_barrel1a test_barrel1a.o $(OBJECTS) $(LDLIBS)

test_barrel1a.o:	test_barrel1a.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_barrel1a.o \
			-c test_barrel1a.cc

test_barrel2:		$(OBJECTS) test_barrel2.o
			g++ -o test_barrel2 test_barrel2.o $(OBJECTS) $(LDLIBS)

test_barrel2.o:		test_barrel2.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_barrel2.o \
			-c test_barrel2.cc


test_lens:		$(OBJECTS) test_lens.o
			g++ -o test_lens test_lens.o $(OBJECTS) $(LDLIBS)

test_lens.o:		test_lens.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o test_lens.o \
			-c test_lens.cc


beamtest0809_sim:   	$(OBJECTS) beamtest0809_sim.o
			g++ -o beamtest0809_sim beamtest0809_sim.o $(OBJECTS) $(LDLIBS)

beamtest0809_sim.o:	beamtest0809_sim.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o beamtest0809_sim.o \
			-c beamtest0809_sim.cc


mppc:			$(OBJECTS) mppc.o
			g++ -o mppc mppc.o $(OBJECTS) $(LDLIBS)

mppc.o:			mppc.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o mppc.o \
			-c mppc.cc


../%.o : ../%.cxx	
			g++  $(CFLAGS) $(INCLFLAGS) -c $< -o $@


../%.d : ../%.cxx	
			@set -e; rm -f $@; \
			g++ -MM $(CFLAGS) $< > $@.$$$$; \
			sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; 

#			$(SHELL) -ec '$(CXX) -MM $< \
#
#			| sed '\''s/\($*\.o\)[ :]*/\1 $@ : /g'\'' > $@'
#     %.d: %.cxx
#             @set -e; rm -f $@; \
#              $(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
#              sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
#              rm -f $@.$$$$


#include $(SOURCES:.cxx=.d)

.PHONY :        	clean

clean :
			rm -f $(OBJECTS) test*.o ../*.d.*

#print: 			$(SOURCES)
#			echo $(CFLAGS)
#			echo $(OBJECTS)
