#include "PndFtsHoughSpacePeak.h"

#include <iostream>


ClassImp(PndFtsHoughSpacePeak);

PndFtsHoughSpacePeak::PndFtsHoughSpacePeak( Int_t height, Int_t firstBin, Int_t firstHitIdx )
:
				fHeight(height),
				fFinished(kFALSE)
{
	// warn if not all values were set
	if ( noVal == firstHitIdx ) std::cout << "WARNING FROM PndFtsHoughSpacePeak, not all values set in initialisation!\n";
	addBin(firstBin, firstHitIdx);
}

PndFtsHoughSpacePeak::~PndFtsHoughSpacePeak()
{
}


