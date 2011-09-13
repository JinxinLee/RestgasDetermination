#include <stdlib.h>
#include <TObject.h>

#include "SiHit.h"

SiHit::SiHit()
{
    fBox = 0;
    fChannel = 0;
    fAdc = 0;
}

SiHit::SiHit(UShort_t box, UShort_t channel, Int_t adc, UShort_t numFrames)
{
    fBox = box;
    fChannel = channel;
    fAdc = adc;
    fNumFrames = numFrames;
}

SiHit::~SiHit()
{
}

ClassImp(SiHit);

