#ifndef ROOT_Results
#define ROOT_Results

#include "TObject.h"

class Results : public TObject {
  
 private:
  
  Int_t pid, NofInt;
  Double_t etot, ptot, px, py, pz, time, xx, yy, zz;

 public:
  
  Results() : pid(0), NofInt(0), etot(0), ptot(0), px(0), py(0), 
    pz(0), time(0), xx(0), yy(0), zz(0) {}

  Results(Int_t mpid, Int_t mnofint, Double_t metot, Double_t mptot,
	  Double_t mpx, 
	  Double_t mpy, Double_t mpz, Double_t mtime,
	  Double_t mxx, Double_t myy, Double_t mzz);

  virtual ~Results();

  ClassDef(Results,4);

};
#endif
