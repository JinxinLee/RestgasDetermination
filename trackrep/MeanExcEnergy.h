#ifndef MEANEXCENERGY_H
#define MEANEXCENERGY_H

#include<iostream>

class TGeoMaterial;

class MeanExcEnergy{
 private:
  MeanExcEnergy(){};
  static const int NELEMENTS = 92;
  static const float vals[];

 public:
  static float get(int Z){
    assert(Z>0&&Z<=NELEMENTS);
    return vals[Z-1];
  }
  static float get(TGeoMaterial*);

};


#endif
