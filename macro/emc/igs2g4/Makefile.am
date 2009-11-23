EXTRA_DIST = configure
#SUBDIRS = src include data
# path to the include folder
INCLUDES = -I./include
# name of the executable
bin_PROGRAMS = igs2g4
# main source for the executable
igs2g4_SOURCES = src/executioner.cc 
# reading part of the IGES data
igs2g4_SOURCES += src/reading.cc include/reading.hh
igs2g4_SOURCES += src/data_igs.cc include/data_igs.hh
# converting part from IGES data into G4 data
igs2g4_SOURCES += src/converting.cc include/converting.hh
igs2g4_SOURCES += src/data_g4.cc include/data_g4.hh
# writing the G4 data for G4Trap class
igs2g4_SOURCES += src/writing.cc include/writing.hh
# project manager
igs2g4_SOURCES += src/manager.cc include/manager.hh
# general classes
igs2g4_SOURCES += src/math.cc include/math.hh
igs2g4_SOURCES += src/algebra.cc include/algebra.hh
igs2g4_SOURCES += src/trigonometry.cc include/trigonometry.hh
igs2g4_SOURCES += src/point.cc include/point.hh
igs2g4_SOURCES += src/angles.cc include/angles.hh
igs2g4_SOURCES += src/line.cc include/line.hh
igs2g4_SOURCES += src/plan.cc include/plan.hh
igs2g4_SOURCES += src/crystal.cc include/crystal.hh
# data file
igs2g4_SOURCES += data/module1.igs
#igs2g4_SOURCES += src/.cc include/.hh
