#include "PndEmcPSAFPGAFilterDelay.h"

PndEmcPSAFPGAFilterDelay::PndEmcPSAFPGAFilterDelay() : 
	buffer(0)
	, bpointer(0)
{
}

PndEmcPSAFPGAFilterDelay::~PndEmcPSAFPGAFilterDelay() {
	buffer.resize(0);
}

void PndEmcPSAFPGAFilterDelay::set(unsigned int newBufferSize) {
	buffer.resize(newBufferSize);
	for(unsigned int i=0; i<newBufferSize; i++)
		buffer[i]=0;
	bpointer=0;
}

float PndEmcPSAFPGAFilterDelay::put(float valueToStore) {
	bpointer %= buffer.size();
	float out = buffer[bpointer];
	buffer[bpointer] = valueToStore;
	bpointer++;
	return out;
}

void PndEmcPSAFPGAFilterDelay::resetToZero() {
	for(unsigned int i=0; i<buffer.size(); i++)
		buffer[i] = 0;
	bpointer = 0;
}
