==============================================================
In this directory one can find the implementation and interface
definitions of a number of MVA algorithms.
===============================================================

--. A very simple and straight forward implementation of KNN is
provided in the following files. Currently no optimization schemes are
applied. Thus the algorithme scales by the number of events in the
training event example set (O n^2). The result contains PDE for each
specified class type.

PndKnnClassify.cpp
PndKnnClassify.h
PndKnnTrain.cpp
PndKnnTrain.h
