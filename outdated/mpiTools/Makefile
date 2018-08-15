#
# JGM, April 2008
#
INSTALLDIR  = ~/

CC          = $(MPI)/bin/mpicc
CCC         = $(MPI)/bin/mpicxx
F77         = $(MPI)/bin/mpif77
CLINKER     = $(MPI)/bin/mpicc -lm -lc -pthread # `$(MPI)/bin/mpicc --showme:link`  # -mpilog
CCLINKER    = $(MPI)/bin/mpicxx -lm -lc -pthread # `$(MPI)/bin/mpicxx --showme:link`  # -mpilog
FLINKER     = $(MPI)/bin/mpif77
F90         = $(MPI)/bin/mpif90
F90LINKER   = $(MPI)/bin/mpif90	  
MAKE        = make --no-print-directory
SHELL       = /bin/sh
CFLAGS	    = -Wall # `$(MPI)/bin/mpicc --showme:compile`
CCFLAGS	    = -Wall # `$(MPI)/bin/mpicxx --showme:compile`
#

### End User configurable options ###

LIBS =
FLIBS =

default: boss_worker_mpi

boss_worker_mpi: src/boss_worker_mpi.o src/killprocesstree.o
	$(CLINKER) -o bin/boss_worker_mpi boss_worker_mpi.o killprocesstree.o
	-rm boss_worker_mpi.o

install_bin:
	-cp -rf bin $(INSTALLDIR)

install_scripts:
	-cp -rf scripts $(INSTALLDIR)

install_macros:
	-cp -rf macros $(INSTALLDIR)

install_jobs:
	-cp -rf jobs $(INSTALLDIR)

clean:
	-rm -f `find ./ -name "*~"`
	-rm -f `find ./ -name "*.o"`
	-rm -f bin/boss_worker_mpi* 

.c.o:
	$(CC) $(CFLAGS) -c $<
.cc.o:
	$(CCC) $(CCFLAGS) -c $<
.f.o:
	$(F77) $(FFLAGS) -c $<
.f90.o:
	$(F90) $(F90FLAGS) -c $<
.SUFFIXES: .f90
