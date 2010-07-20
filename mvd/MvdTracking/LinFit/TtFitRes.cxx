// Simone Bianco
// 15.07.2010

#include "stdlib.h"
#include <iostream>
#include <vector>
#include "TtFitRes.h"


// -----   Default constructor   -------------------------------------------
TtFitRes::TtFitRes() {
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
TtFitRes::TtFitRes(Double_t p0, Double_t p1, Double_t p2, Double_t p3, Double_t chiX, Double_t chiY, Int_t numSens){

  fP0 = p0; fP1 = p1; fP2 = p2; fP3 = p3;
  fChiSqX = chiX;
  fChiSqY = chiY;
  fNsens = numSens;

}


// -----   Destructor   ----------------------------------------------------
TtFitRes::~TtFitRes() {}

// -------------------------------------------------------------------------
ClassImp(TtFitRes);
