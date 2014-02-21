#include "PndFtsHoughSpace.h"

#include <iostream>
#include "math.h"

#include "TClonesArray.h"


ClassImp(PndFtsHoughSpace);

PndFtsHoughSpace::PndFtsHoughSpace(
		TClonesArray *ftsHitArray,
		Double_t zRefPos,
		Double_t interceptZx
) :
fFtsHitArray(ftsHitArray),
fVerbose(0),

fZRefPos(zRefPos),
fInterceptZx(interceptZx)
//fFtsHitArray(0),
//fVerbose(0),
//
//fZRefPos(0.),
//fInterceptZx(0.)
{

}

PndFtsHoughSpace::~PndFtsHoughSpace()
{

}









void PndFtsHoughSpace::Print() {
	std::cout << "=========== PndFtsHoughSpace::Print() ==========" << std::endl;
	std::cout << "fZRefPos = " << fZRefPos << '\n';
	std::cout << "fInterceptZx = " << fInterceptZx << 'n';
	TH2S::Print();
}


