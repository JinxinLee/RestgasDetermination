INCLFLAGS  = -I$(shell root-config --incdir) -I../ -I.

# ROOT version format X.XX/XX
ROOTVERFULL = $(shell root-config --version)
ROOTVERMAIN = $(shell echo $(ROOTVERFULL) | sed 's/\/.*//' | sed 's/\./''/')


ifeq ($(shell expr $(ROOTVERMAIN) '>=' 516),0)
$(error Require ROOT version >= 5.16)
endif

ifeq ($(shell expr $(ROOTVERMAIN) '<=' 520),1)
LDLIBS	= $(shell root-config --glibs) -lMathCore
else
LDLIBS  = $(shell root-config --glibs) -lGenVector
endif


OBJECTS = $(patsubst ../%.cxx,../%.o,$(wildcard ../*.cxx))


#debugger ddd: compile everything with option -g
DEBUG =
#DEBUG = -g

CFLAGS = -O2 -Wall $(DEBUG)
#CFLAGS = -g -O0 -Wall

# shared library
SOFLAGS = -shared -Wall $(DEBUG)

# for dynamic linking
CFLAGSDYM = $(CFLAGS) -fPIC



all:			test_simple_bar test_simple_bar2 test_simple_bar2a test_simple_bar2b test_simple_bar2c \
			test_disk test_sheet test_cylinder \
			test_focus test_focus2 test_absorption test_test test_titti \
			test_top test_air_lens test_hook test_barrel1 test_barrel1a test_barrel2  \
			test_barrel3 test_lens mppc test_system prototype test_cyl_lens \
			test_simple_lens_sheet lib_DetEffi.so lib_KBarAnalysis.so test_coating \
			test_barrel3_noairgap test_reco_simple test_reco_full_generate_lut test_reco_full \
			test_simple_mirror_sheet


test_coating:		$(OBJECTS) test_coating.o
			g++ -o test_coating test_coating.o $(OBJECTS) $(LDLIBS)

test_simple_lens_sheet:	$(OBJECTS) test_simple_lens_sheet.o
			g++ -o test_simple_lens_sheet test_simple_lens_sheet.o $(OBJECTS) $(LDLIBS)

test_simple_mirror_sheet:	$(OBJECTS) test_simple_mirror_sheet.o
			g++ -o test_simple_mirror_sheet test_simple_mirror_sheet.o $(OBJECTS) $(LDLIBS)

test_simple_bar:	$(OBJECTS) test_simple_bar.o
			g++ -o test_simple_bar test_simple_bar.o $(OBJECTS) $(LDLIBS)

test_simple_bar2:	$(OBJECTS) test_simple_bar2.o
			g++ -o test_simple_bar2 test_simple_bar2.o $(OBJECTS) $(LDLIBS)
test_simple_bar2a:	$(OBJECTS) test_simple_bar2a.o
			g++ -o test_simple_bar2a test_simple_bar2a.o $(OBJECTS) $(LDLIBS)
test_simple_bar2b:	$(OBJECTS) test_simple_bar2b.o
			g++ -o test_simple_bar2b test_simple_bar2b.o $(OBJECTS) $(LDLIBS)
test_simple_bar2c:	$(OBJECTS) test_simple_bar2c.o
			g++ -o test_simple_bar2c test_simple_bar2c.o $(OBJECTS) $(LDLIBS)

test_disk:		$(OBJECTS) test_disk.o
			g++ -o test_disk test_disk.o $(OBJECTS) $(LDLIBS)

test_sheet:		$(OBJECTS) test_sheet.o
			g++ -o test_sheet test_sheet.o $(OBJECTS) $(LDLIBS)

test_cylinder:		$(OBJECTS) test_cylinder.o
			g++ -o test_cylinder test_cylinder.o $(OBJECTS) $(LDLIBS)

test_focus:		$(OBJECTS) test_focus.o
			g++ -o test_focus test_focus.o $(OBJECTS) $(LDLIBS)

test_focus2:		$(OBJECTS) test_focus2.o
			g++ -o test_focus2 test_focus2.o $(OBJECTS) $(LDLIBS)

test_absorption:	$(OBJECTS) test_absorption.o
			g++ -o test_absorption test_absorption.o $(OBJECTS) $(LDLIBS)

test_test:		$(OBJECTS) test_test.o
			g++ -o test_test test_test.o $(OBJECTS) $(LDLIBS)

test_system:		$(OBJECTS) test_system.o
			g++ -o test_system test_system.o $(OBJECTS) $(LDLIBS)

test_titti:		$(OBJECTS) test_titti.o
			g++ -o test_titti test_titti.o $(OBJECTS) $(LDLIBS)

test_top:		$(OBJECTS) test_top.o
			g++ -o test_top test_top.o $(OBJECTS) $(LDLIBS)

test_air_lens:		$(OBJECTS) test_air_lens.o
			g++ -o test_air_lens test_air_lens.o $(OBJECTS) $(LDLIBS)

test_hook:		$(OBJECTS) test_hook.o
			g++ -o test_hook test_hook.o $(OBJECTS) $(LDLIBS)

test_barrel1:		$(OBJECTS) test_barrel1.o
			g++ -o test_barrel1 test_barrel1.o $(OBJECTS) $(LDLIBS)

test_barrel1a:		$(OBJECTS) test_barrel1a.o
			g++ -o test_barrel1a test_barrel1a.o $(OBJECTS) $(LDLIBS)

test_barrel2:		$(OBJECTS) test_barrel2.o
			g++ -o test_barrel2 test_barrel2.o $(OBJECTS) $(LDLIBS)

test_barrel3:		$(OBJECTS) test_barrel3.o
			g++ -o test_barrel3 test_barrel3.o $(OBJECTS) $(LDLIBS)

test_barrel3_noairgap:	$(OBJECTS) test_barrel3_noairgap.o
			g++ -o test_barrel3_noairgap test_barrel3_noairgap.o $(OBJECTS) $(LDLIBS)

test_cyl_lens:		$(OBJECTS) test_cyl_lens.o
			g++ -o test_cyl_lens test_cyl_lens.o $(OBJECTS) $(LDLIBS)

test_lens:		$(OBJECTS) test_lens.o
			g++ -o test_lens test_lens.o $(OBJECTS) $(LDLIBS)

test_reco_simple:	$(OBJECTS) test_reco_simple.o
			g++ -o test_reco_simple test_reco_simple.o $(OBJECTS) $(LDLIBS)

test_reco_full_generate_lut:	$(OBJECTS) test_reco_full_generate_lut.o test_reco_full_setup.o
			g++ -o test_reco_full_generate_lut test_reco_full_generate_lut.o \
			test_reco_full_setup.o  $(OBJECTS) $(LDLIBS)

test_reco_full:		$(OBJECTS) test_reco_full.o test_reco_full_setup.o
			g++ -o test_reco_full test_reco_full.o \
			test_reco_full_setup.o  $(OBJECTS) $(LDLIBS)

mppc:			$(OBJECTS) mppc.o
			g++ -o mppc mppc.o $(OBJECTS) $(LDLIBS)

prototype:		$(OBJECTS) prototype.o
			g++ -o prototype prototype.o $(OBJECTS) $(LDLIBS)


%.o:			%.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o $@ -c $<

../%.o: 		../%.cxx
			g++ $(CFLAGS) $(INCLFLAGS) -o $@ -c $<



lib_DetEffi.so:		dict_libDetEffi.o DetEffi.o
			g++ $(SOFLAGS) -o $@ $(LDLIBS) $^

lib_KBarAnalysis.so:	dict_libKBarAnalysis.o KBarAnalysis.o DetEffi.o
			g++ $(SOFLAGS) -o $@ $(LDLIBS) $^

dict_lib%.o:		dict_lib%.cxx
			g++ $(CFLAGSDYM) $(INCLFLAGS) -o $@ -c $^

dict_lib%.cxx:		%.h DetEffi.h %_LinkDef.hh
			rootcint -f $@ -c $(INCLFLAGS) $^


%.o:			%.C
			g++ $(CFLAGSDYM) $(INCLFLAGS) -o $@ -c $<



.PHONY :		clean cleandict

clean:
			rm -f $(OBJECTS) *.o ../*.o dict_* lib_*.so

cleandict:
			rm -f dict_lib*.o dict_lib*.cxx DetEffi.o KBarAnalysis.o
