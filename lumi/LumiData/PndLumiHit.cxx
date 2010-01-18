#include "PndLumiHit.h"

PndLumiHit::PndLumiHit()
{
}

PndLumiHit::PndLumiHit(Int_t detID, TString detName, TVector3& pos, TVector3& dpos,
		     Int_t clindex, Double_t charge, Int_t NDigiHits, Int_t mcindex, Bool_t isdoubleside)
  : FairHit(detID, pos, dpos, mcindex) {
  fDetName = detName;
  fCharge  = charge;
  fNDigiHits = NDigiHits;
  fClusterIndex = clindex;
  fBotIndex = -1;
  fIsDoubleSided = isdoubleside;

}

PndLumiHit::PndLumiHit(Int_t detID, TString detName, Double_t &slope, Double_t &slopeErr,
		Double_t& intercept, Double_t &intErr,Int_t clindex, Double_t charge, Int_t NDigiHits,
		Int_t mcindex, Bool_t isdoubleside)
//: FairHit(detID, TVectorpos, dpos, mcindex) {
{
	fDetName = detName;
	fSlope = slope;
	fIntercept = intercept;
	fCharge  = charge;
	fNDigiHits = NDigiHits;
	fClusterIndex = clindex;
	fBotIndex = -1;
	fIsDoubleSided = isdoubleside;
}

PndLumiHit::~PndLumiHit()
{

}

void PndLumiHit::Print(const Option_t* opt) const
{
  std::cout << *this;
}
ClassImp(PndLumiHit);
