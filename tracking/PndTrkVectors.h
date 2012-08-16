#ifndef PndTrkVectors_H
#define PndTrkVectors_H 1

#include <stdlib.h>
#include <iostream>

using namespace std ;

template <class T> class Vec{

 public:

 int dimension;

 T *array;

 Vec(T *p, int dim){
	dimension = dim;
	array = p;
 };

 ~Vec(){;};

 T& operator[] (int index){

  if(index>=0 && index<dimension){
   T& alias = array[index];
   return  alias ;
  }
  cout<< "index "<<index<<" is outofbounds!\n";
  exit(-1);
 };

};




#endif
