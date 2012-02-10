#include "PndLumiHit.h"

PndLumiHit::PndLumiHit()
: FairHit()
{
}

PndLumiHit::PndLumiHit(Int_t detID, TVector3 &hitPos, TVector3 &hitErr, Int_t mcindex, TString &detname,
		Double_t meancharge, Int_t clindex, Int_t NDigiHits)
:FairHit(detID, hitPos, hitErr, mcindex)
 {
	 fDetName = detname;
	 fQ  = meancharge;
	 fNDigiHits = NDigiHits;
	 fClusterIndex = clindex;
}
PndLumiHit::~PndLumiHit()
{
}
ClassImp(PndLumiHit);
