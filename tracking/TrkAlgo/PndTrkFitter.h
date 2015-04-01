#ifndef PNDTRKFITTER_H
#define PNDTRKFITTER_H

#include <vector>
#include "TObject.h"

class PndTrkFitter : public TObject {
  
 public:
  
  PndTrkFitter();
  PndTrkFitter(int verbose);
 ~PndTrkFitter();


  void Reset();
  Bool_t SetPointToFit(double x, double y, double sigma);
  Bool_t StraightLineFit(Double_t &fitm, Double_t &fitp);
  Double_t StraightLineFitWithChi2(Double_t &fitm, Double_t &fitp);

  Bool_t ConstrainedStraightLineFit(Double_t x0, Double_t y0, Double_t &fitm, Double_t &fitp);

  Bool_t ParabolaFit(Double_t &fita, Double_t &fitb, Double_t &fitc);


  void SetVerbose(int ver) { fVerbose = ver; } 

 private:

  Double_t fSx, fSy, fSxy, fSxx, fSxxy, fSxxx, fSxxxx, fS1;
  std::vector<double> fX, fY, fSigma;
  Int_t fVerbose; 
  ClassDef(PndTrkFitter, 1)
};


#endif
