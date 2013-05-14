////////////////////////////////////////////////////////////
//
// PndTrkConformalTransform
// 
// Class to perform the Conformal Transformation
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//                                   
////////////////////////////////////////////////////////////

#include "PndTrkConformalTransform.h"

#include "PndTrkConformalHit.h"
#include <iostream>

using namespace std;


// -----   Default constructor   -------------------------------------------
PndTrkConformalTransform::PndTrkConformalTransform() : fTrasl(0., 0.), fAngle(0.) {}

// -------------------------------------------------------------------------
PndTrkConformalTransform::PndTrkConformalTransform(double x, double y, double delta) : fTrasl(x, y), fAngle(delta) {}



// -----   Destructor   ----------------------------------------------------
PndTrkConformalTransform::~PndTrkConformalTransform() { }
// -------------------------------------------------------------------------

void PndTrkConformalTransform::SetTranslation(double x, double y) {
  fTrasl.Set(x, y);
}
void PndTrkConformalTransform::SetRotation(double delta){
  fAngle = delta;
}

void PndTrkConformalTransform::PerformConformalTransformation(double x, double y, double rd, double &u, double &v, double &rc)
{
  Double_t xtrasl, ytrasl;
  // traslation
  xtrasl = x - fTrasl.X();
  ytrasl = y - fTrasl.Y();

  // CHECK and the rotation??

  // change coordinate of the center
  u = GetXConf(xtrasl, ytrasl, rd);
  v = GetYConf(xtrasl, ytrasl, rd);
  rc = GetRConf(xtrasl, ytrasl, rd);
  
}

PndTrkConformalHit *PndTrkConformalTransform::GetConformalSttHit(PndTrkHit *hit) {
  TVector3 position = hit->GetPosition();
  Double_t rd = hit->GetIsochrone();
  double u, v, rc;
  PerformConformalTransformation(position.X(), position.Y(), rd, u, v, rc);
  PndTrkConformalHit *chit = new PndTrkConformalHit(hit, u, v, rc);
  return chit;
}

PndTrkConformalHit * PndTrkConformalTransform::GetConformalHit(PndTrkHit *hit) {
  TVector3 position = hit->GetPosition();
  Double_t rd = 0.;
  double u, v, rc;
  PerformConformalTransformation(position.X(), position.Y(), rd, u, v, rc);
  PndTrkConformalHit *chit = new PndTrkConformalHit(hit, u, v);
  return chit;
}

double PndTrkConformalTransform::GetXConf(double x, double y, double rd) {
  return x / (x * x + y * y - rd * rd);
}

double PndTrkConformalTransform::GetYConf(double x, double y, double rd) {
  return y / (x * x + y * y - rd * rd);
}

double PndTrkConformalTransform::GetRConf(double x, double y, double rd) {
  return rd / (x * x + y * y - rd * rd);
}



ClassImp(PndTrkConformalTransform)

