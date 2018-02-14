#include "PndEmcPSAFPGAFilterCF.h"

PndEmcPSAFPGAFilterCF::PndEmcPSAFPGAFilterCF() :
  buffer(0)
  ,bpointer(0)
{
}

PndEmcPSAFPGAFilterCF::~PndEmcPSAFPGAFilterCF()
{
  buffer.resize(0);
}

void PndEmcPSAFPGAFilterCF::set(unsigned int newBufferSize, float newRatio)
{
  buffer.resize(newBufferSize);
  for(unsigned int i=0; i<newBufferSize; i++)
    buffer[i]=0;
  ratio = newRatio;
  bpointer=0;
}

void PndEmcPSAFPGAFilterCF::resetToZero()
{
	bpointer = 0;
  for(unsigned int i=0; i<buffer.size(); i++)
    buffer[i]=0;
}

float PndEmcPSAFPGAFilterCF::put(float valueToStore){
  bpointer %= buffer.size();
  float cfd = buffer[bpointer] - ratio * valueToStore;
  buffer[bpointer] = valueToStore;
  bpointer++;
  return cfd;
};

