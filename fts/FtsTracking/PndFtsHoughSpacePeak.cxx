#include "PndFtsHoughSpacePeak.h"

#include <iostream>


ClassImp(PndFtsHoughSpacePeak);

PndFtsHoughSpacePeak::PndFtsHoughSpacePeak( Int_t height, Int_t firstBin )
:
		fHeight(height),
		fFinished(kFALSE)
{
  addBin(firstBin);
}

PndFtsHoughSpacePeak::~PndFtsHoughSpacePeak()
{
}


