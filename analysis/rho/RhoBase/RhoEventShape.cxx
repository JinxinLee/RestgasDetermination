// Implementation of the RhoEventShape class
// Author: Jens Brose, TUD, Aug. 1999
// Ralf Kliemt, HIM/GSI Feb.2013 (Cleanup & Restructuring)
//--------------------------------------------------------------------------
#include "RhoBase/RhoCandList.h"
#include "RhoBase/RhoCandidate.h"
#include "RhoBase/RhoEventShape.h"

ClassImp(RhoEventShape)

TBuffer& operator>>(TBuffer& buf, RhoEventShape *&obj)
{
  obj = (RhoEventShape*) buf.ReadObject(RhoEventShape::Class());
  return buf;
}

RhoEventShape::RhoEventShape()
{
  fTot4Mom = TLorentzVector(0.,0.,0.,0.);
  //_fw2 = 0;
  fTotM = 0;
  fTotE = 0;
  fTotP = 0;
  fTotPt = 0;
  fTotAbsMom = 0;
}

RhoEventShape::RhoEventShape(RhoCandList& l)
{
  fTot4Mom = TLorentzVector(0.,0.,0.,0.);
  fTotAbsMom = 0;
  //Double_t fw2n = 0;
  Int_t nl = l.GetNumberOfTracks();
  for (Int_t i=0; i<nl; ++i) {
    RhoCandidate* c1 = l.Get(i);
    TVector3 p1 = c1->P3();
    fTotAbsMom += p1.Mag();
    fTot4Mom += c1->P4();
//    for (Int_t j=i; j<nl; ++j) {
//      RhoCandidate *c2 = l.Get(j);
//      TVector3 p2 = c2->P3();
//      Double_t cosal = p1.Dot(p2) /(p1.Mag()*p2.Mag());
//      Double_t legendre2 = 3.*cosal*cosal - 1.;
//      if (i == j) legendre2 = legendre2 / 2.;
//      fw2n += legendre2 * p1.Mag() * p2.Mag();
//    }
  }
  //_fw2 = fTotabsmom>0 ? fw2n/(fTotabsmom*fTotabsmom) : 0.0;
  fTotM = fTot4Mom.M();
  fTotE = fTot4Mom.E();
  fTotP = fTot4Mom.Rho();
  fTotPt = fTot4Mom.Perp();
}

RhoEventShape::~RhoEventShape() {}
