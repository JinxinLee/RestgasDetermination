
#include"energyLoss.h"
#include"math.h"
#include"assert.h"
#include<iostream>

float energyLoss(float beta,float charge,float density,float ZoverA,float MEE){
  //std::cout << "beta " << beta << " charge " << charge << " density " << density << " ZoverA " << ZoverA<< " " << MEE << std::endl;
  float retVal = 0.3071*ZoverA*density/(beta*beta)*charge*charge;
  //std::cout << retVal << std::endl;
  //std::cout << (log(beta*beta*0.510999*2./(1.E-6*MEE*(1-beta*beta)))-beta*beta) << std::endl;
  retVal *= (log(beta*beta*0.510999*2./(1.E-6*MEE*(1-beta*beta)))-beta*beta);
  assert(retVal>0);
  //in GeV/cm, hence 1.e-3
  return 1.E-3*retVal;
}
