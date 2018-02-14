// Simone Bianco
// 15.07.2010

#include "stdlib.h"
#include <iostream>
#include <vector>
#include "TtFitRes.h"


// -----   Default constructor   -------------------------------------------
TtFitRes::TtFitRes() :
   fP0(0.), fP1(0.), fP2(0.), fP3(0.),
   fEloss(0.), fChiSqX(0.), fChiSqY(0.),
   fNsens(0)
{
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
TtFitRes::TtFitRes(Double_t p0, Double_t p1, Double_t p2, Double_t p3, Double_t eloss, Double_t chiX, Double_t chiY, Int_t numSens) :
  fP0(p0), fP1(p1), fP2(p2), fP3(p3),
  fEloss(eloss), fChiSqX(chiX), fChiSqY(chiY),
  fNsens(numSens)
{
}


// -----   Destructor   ----------------------------------------------------
TtFitRes::~TtFitRes() {}

// -------------------------------------------------------------------------
ClassImp(TtFitRes);
