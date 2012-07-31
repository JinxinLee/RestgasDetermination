#include "PndEmcPSAFPGAFilterMWD.h"
#include <math.h>

PndEmcPSAFPGAFilterMWD::PndEmcPSAFPGAFilterMWD() :
  buffer(0)
  , bpointer(0)
{
}

PndEmcPSAFPGAFilterMWD::~PndEmcPSAFPGAFilterMWD()
{
  buffer.resize(0);
};

void PndEmcPSAFPGAFilterMWD::set(float tau, unsigned int newBufferSize)
{
  buffer.resize(newBufferSize);
    for(unsigned int i=0; i<newBufferSize; i++)
      buffer[i]=0;
    bpointer=0;
    sum = 0.0;
    lambda = 1.0/tau;
}

float PndEmcPSAFPGAFilterMWD::put(float valueToStore){
    bpointer++;
    bpointer %= buffer.size();
    float diff = valueToStore - buffer[bpointer];
    float mwd = diff + lambda*sum;
    sum += diff;
    buffer[bpointer] = valueToStore;
    return mwd;
}
