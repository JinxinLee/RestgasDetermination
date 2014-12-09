#include "PndFtsHoughSpacePeak.h"

#include <iostream>


ClassImp(PndFtsHoughSpacePeak);

PndFtsHoughSpacePeak::PndFtsHoughSpacePeak( Int_t height, Int_t firstBin, Int_t firstHitIdx )
:
		fHeight(height),
		fFinished(kFALSE)
{
  addBin(firstBin, firstHitIdx);
}

PndFtsHoughSpacePeak::~PndFtsHoughSpacePeak()
{
}


