#include "PndFtsHoughSpace.h"

#include <iostream>
#include "math.h"

#include "TClonesArray.h"


ClassImp(PndFtsHoughSpace);


PndFtsHoughSpace::PndFtsHoughSpace() :
		fFtsHitArray(0),
		fVerbose(0),

		fZRefPos(0),
		fInterceptZx(0),

		TH2S()

		 {
std::cout << "PndFtsHoughSpace WARNING Do not use the default constructor.\n";
		 }


PndFtsHoughSpace::PndFtsHoughSpace(
		const char *name,

		Int_t nbinsx,
		Double_t xlow,
		Double_t xup,

		Int_t nbinsy,
		Double_t ylow,
		Double_t yup,

		TClonesArray *ftsHitArray,
		Double_t zRefPos,
		Double_t interceptZx
) :
		fFtsHitArray(ftsHitArray),
		fVerbose(0),

		fZRefPos(zRefPos),
		fInterceptZx(interceptZx),

		TH2S(name,name,nbinsx,xlow,xup,nbinsy,ylow,yup)

		 {

		 }

PndFtsHoughSpace::~PndFtsHoughSpace()
{

}




void PndFtsHoughSpace::AddHit(UInt_t detId, UInt_t hitId, Double_t rho)
{
	fHitId.push_back(PndTrackCandHit(detId, hitId, rho));
}

void PndFtsHoughSpace::AddHit(TString branchName, UInt_t hitId, Double_t rho)
{
	FairRootManager* ioman = FairRootManager::Instance();
	UInt_t detId = ioman->GetBranchId(branchName);
	AddHit(detId, hitId, rho);
}

void PndFtsHoughSpace::AddHit(FairLink link, Double_t rho)
{
	fHitId.push_back(PndTrackCandHit(link.GetType(), link.GetIndex(), rho));
}




void PndFtsHoughSpace::Print() {
	std::cout << "=========== PndFtsHoughSpace::Print() ==========" << std::endl;
	std::cout << "fZRefPos = " << fZRefPos << '\n';
	std::cout << "fInterceptZx = " << fInterceptZx << 'n';
	TH2S::Print();
}


