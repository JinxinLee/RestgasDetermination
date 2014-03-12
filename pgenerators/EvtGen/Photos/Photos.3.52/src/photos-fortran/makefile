include ../make.inc

SOURCES_F=photos.f
OBJECTS_F=$(SOURCES_F:.f=.o)

%.o: %.f
	$(F77) $(FFLAGS) $(DEBUG) -I. -c $< -o $@

all: $(OBJECTS_F)

clean:
	rm -f *.o *~;
