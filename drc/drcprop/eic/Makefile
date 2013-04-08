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
#DEBUG =
#DEBUG = -g

CFLAGS = -O2 -Wall $(DEBUG)
#CFLAGS = -pg  -g -O0 -Wall

# shared library
SOFLAGS = -shared -Wall $(DEBUG)

# for dynamic linking
CFLAGSDYM = $(CFLAGS) -fPIC



all:			test_reco_full_generate_lut test_reco_full_setup_lens_modf_50dgupwal_40dgscr



test_reco_full_generate_lut:	$(OBJECTS) test_reco_full_generate_lut.o test_reco_full_setup_lens.o
			g++ -o test_reco_full_generate_lut test_reco_full_generate_lut.o \
			test_reco_full_setup_lens.o  $(OBJECTS) $(LDLIBS)

test_reco_full_setup_lens_modf_50dgupwal_40dgscr:		$(OBJECTS) test_reco_full_time_modf_50dgupwal_40dgscr.o \
								test_reco_full_setup_lens.o
			g++ -o test_reco_full_time_modf_50dgupwal_40dgscr test_reco_full_time_modf_50dgupwal_40dgscr.o \
			test_reco_full_setup_lens.o  $(OBJECTS) $(LDLIBS)




%.o:			%.cc
			g++ $(CFLAGS) $(INCLFLAGS) -o $@ -c $<

../%.o: 		../%.cxx
			g++ $(CFLAGS) $(INCLFLAGS) -o $@ -c $<




%.o:			%.C
			g++ $(CFLAGSDYM) $(INCLFLAGS) -o $@ -c $<



.PHONY :		clean cleandict

clean:
			rm -f $(OBJECTS) *.o ../*.o 

