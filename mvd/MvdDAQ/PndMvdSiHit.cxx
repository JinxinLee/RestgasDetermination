#include <stdlib.h>
#include <TObject.h>

#include "PndMvdSiHit.h"

PndMvdSiHit::PndMvdSiHit()
{
    fBox = 0;
    fChannel = 0;
    fAdc = 0;
}

PndMvdSiHit::PndMvdSiHit(UShort_t box, UShort_t channel, Int_t adc, UShort_t numFrames)
{
    fBox = box;
    fChannel = channel;
    fAdc = adc;
    fNumFrames = numFrames;
}

PndMvdSiHit::~PndMvdSiHit()
{
}

ClassImp(PndMvdSiHit);

