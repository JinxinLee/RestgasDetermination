#include "PndEmcPSAFPGAFilterMA.h"

PndEmcPSAFPGAFilterMA::PndEmcPSAFPGAFilterMA() : 
	buffer(0)
	, bpointer(0)
{
}

PndEmcPSAFPGAFilterMA::~PndEmcPSAFPGAFilterMA()
{
	buffer.resize(0);
}

void PndEmcPSAFPGAFilterMA::set(unsigned int newBufferSize)
{
	buffer.resize(newBufferSize);
	for(unsigned int i=0; i<newBufferSize; i++)
		buffer[i]=0;
	bpointer=0;
	sum = 0;
}

void PndEmcPSAFPGAFilterMA::resetToZero()
{
	for(unsigned int i=0; i<buffer.size(); i++)
		buffer[i] = 0;
	sum = 0;
	//bpointer =0;
}

float PndEmcPSAFPGAFilterMA::put(float valueToStore)
{
	bpointer %= buffer.size();
	sum += valueToStore - buffer[bpointer];
	buffer[bpointer] = valueToStore;
	bpointer++;
	return sum/buffer.size();
}

