# run "make" then "make lib" "make bin" (obsolete)

LDROOT := ${shell root-config --libs}

INCROOT := -I ${shell root-config --cflags} -I./include

SOFLAGS       = -shared
LD            = g++

name := PWO25
G4TARGET := $(name)
G4EXLIB := true

#G4WORKDIR :=.

.PHONY: all

all: rcint lib shlib bin

rcint:
	rootcint -f PWO25SimEvent_cint.cc -c $(INCROOT) PWO25SimEvent.hh
	mv PWO25SimEvent_cint.cc ./src
	mv PWO25SimEvent_cint.h ./include
	rootcint -f PWO25SimHit_cint.cc -c $(INCROOT) PWO25SimHit.hh
	mv PWO25SimHit_cint.cc ./src
	mv PWO25SimHit_cint.h ./include

# run make shlib after "make" and "make lib" (obsolete)
shlib:
	$(LD) -g -fPIC $(SOFLAGS) $(LDROOT) -o ./PWO25SimEvent.so $(G4WORKDIR)/tmp/Linux-g++/PWO25/PWO25SimEvent_cint.o $(G4WORKDIR)/tmp/Linux-g++/PWO25/PWO25SimHit_cint.o $(G4WORKDIR)/tmp/Linux-g++/PWO25/PWO25SimEvent.o $(G4WORKDIR)/tmp/Linux-g++/PWO25/PWO25SimHit.o

include $(G4INSTALL)/config/binmake.gmk

CPPFLAGS += $(INCROOT)
CPPFLAGS += -g
LDFLAGS += $(LDROOT)