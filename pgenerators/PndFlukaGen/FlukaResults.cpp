#include "FlukaResults.h"
#include "iostream"

ClassImp(Results)

Results::Results(Int_t mpid, Int_t mnofint, Double_t metot, Double_t mptot,
		 Double_t mpx, Double_t mpy, Double_t mpz,
	         Double_t mtime,
		 Double_t mxx, Double_t myy, Double_t mzz)
             : pid(mpid), NofInt(mnofint), etot(metot), ptot(mptot), px(mpx),
               py(mpy), pz(mpz), time(mtime), xx(mxx), yy(myy), zz(mzz)
{
}

Results::~Results()
{
}

