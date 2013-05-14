#ifndef PNDTRKFITTER_H
#define PNDTRKFITTER_H

#include <vector>
#include "TObject.h"

class PndTrkFitter : public TObject {
  
 public:
  
  PndTrkFitter();
  ~PndTrkFitter();


  void Reset();
  Bool_t SetPointToFit(double x, double y, double sigma);
  Bool_t StraightLineFit(Double_t &fitm, Double_t &fitp);


 private:

  Double_t fSx, fSy, fSxy, fSxx, fS1;
 std::vector<double> fX, fY, fSigma;

  ClassDef(PndTrkFitter, 1)
};


#endif
