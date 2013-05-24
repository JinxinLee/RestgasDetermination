////////////////////////////////////////////////////////////
//
// PndTrkFitter
// 
// Class to perform the Least Square analytical 
// Fit with a straight line
//
// authors: Lia Lavezzi - INFN Pavia (2013)
//                                   
////////////////////////////////////////////////////////////

#include "PndTrkFitter.h"

#include <iostream>

using namespace std;


// -----   Default constructor   -------------------------------------------
PndTrkFitter::PndTrkFitter() : fVerbose(0), fSx(0), fSy(0), fSxy(0), fSxx(0), fS1(0)
{
  fX.clear();
  fY.clear();
  fSigma.clear();
}
// -------------------------------------------------------------------------
PndTrkFitter::PndTrkFitter(int verbose) : fVerbose(verbose), fSx(0), fSy(0), fSxy(0), fSxx(0), fS1(0)
{
  fX.clear();
  fY.clear();
  fSigma.clear();
}
// -----   Destructor   ----------------------------------------------------
PndTrkFitter::~PndTrkFitter() { 

}

void PndTrkFitter::Reset() {
  fSx = 0;
  fSy = 0;
  fSxy = 0;
  fSxx = 0;
  fS1 = 0;
  fX.clear();
  fY.clear();
  fSigma.clear();
}

Bool_t PndTrkFitter::SetPointToFit(double x, double y, double sigma) {
  fX.push_back(x);
  fY.push_back(y);
  fSigma.push_back(sigma);
}

Bool_t PndTrkFitter::StraightLineFit(Double_t &fitm, Double_t &fitp) {
 
  int nofPoints = fX.size();
  if(nofPoints == 0) {
    if(fVerbose > 1) cout << "PndTrkFitter::StraightLineFit: no points to fit! fill the array with PndTrkFitter::SetPointToFit()" << endl;
    Reset();
    return kFALSE; // CHECK
  }
  for(int ipnt = 0; ipnt < nofPoints; ipnt++)  
    { 
      fSx += fX[ipnt]/(fSigma[ipnt] * fSigma[ipnt]);
      fSy +=  fY[ipnt]/(fSigma[ipnt] * fSigma[ipnt]);
      
      fSxy += fX[ipnt] * fY[ipnt]/(fSigma[ipnt] * fSigma[ipnt]);
      fSxx += fX[ipnt]* fX[ipnt]/(fSigma[ipnt] * fSigma[ipnt]);
      
      fS1 += 1./(fSigma[ipnt] * fSigma[ipnt]);
    }
  
  Double_t den = fSxx * fS1 - fSx * fSx;
  if(den == 0) {
    if(fVerbose > 1) cout << "PndTrkFitter:StraightLineFit: DEN == 0" << endl; // CHECK
    Reset();
    return kFALSE;
  }
  
  fitm = (fSxy * fS1 - fSx * fSy)/den;
  fitp =  (fSxx * fSy - fSx * fSxy)/den;
  Reset();
  return kTRUE; 

}
ClassImp(PndTrkFitter)

