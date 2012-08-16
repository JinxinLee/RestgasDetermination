#ifndef PndTrkVectors_H
#define PndTrkVectors_H 1
// Root includes
#include "TROOT.h"

#include <stdlib.h>
#include <iostream>

using namespace std ;

template <class T> class Vec{

 public:

 int dimension;
 TString name;
 T *array;

 Vec(T *p, int dim, TString nam){
	dimension = dim;
	array = p;
	name = nam;
 };

 ~Vec(){;};

 T& at(int index){

  if(index>=0 && index<dimension){
   T& alias = array[index];
   return  alias ;
  }
  cout<< "Array "<<name<<": index = "<<index<<" and it is out of bounds! Exiting the job.\n";
  exit(-1);
 };

 T& operator[] (int index){

  if(index>=0 && index<dimension){
   T& alias = array[index];
   return  alias ;
  }
  cout<< "Array "<<name<<": index = "<<index<<" and it is out of bounds! Exiting the job.\n";
  exit(-1);
 };

};




#endif
