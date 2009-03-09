#include "PndLumiHit.h"
using std::cout;
using std::endl;

PndLumiHit::PndLumiHit()
    : FairHit()
{
    fWasLost = false;
    fELoss = 0.;
}

PndLumiHit::PndLumiHit(Int_t detID, TVector3& pos, TVector3& dpos,
		Int_t index, Double_t eloss, Int_t wasLost)
  : FairHit(detID, pos, dpos, index)
{
    fWasLost = wasLost;
    fELoss = eloss;
}


PndLumiHit::~PndLumiHit()
{
}

void PndLumiHit::Print(const Option_t* opt) const
{
    std::cout << "PndLumiHit: " << std::endl
	      << " Detector unique identifier        : " << fDetectorID << std::endl
	      << " Position of hit                   : (" << fX << ", " << fY << ", " << fZ << ") [cm]" << std::endl
	      << " Errors of position                : (" << fDx << ", " << fDy << ", " << fDz << ") [cm]" << std::endl
	      << " Energy Loss                       : " << fELoss << endl
	      << " Index of FairMCPoint for this hit : " << fRefIndex << std::endl
	      << " Hit lost due to inefficency       : " << fWasLost << std::endl;
}

ClassImp(PndLumiHit)
