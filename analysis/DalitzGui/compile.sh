rootcint -f gui_linkdef.cpp -c DalitzGUI.h linkdef.h
g++ `root-config --cflags --glibs` -o DalitzGUI DalitzGUI.cpp gui_linkdef.cpp
