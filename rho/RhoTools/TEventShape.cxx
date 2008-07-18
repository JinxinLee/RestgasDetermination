// Implementation of the TEventShape class 
// Author: Jens Brose, TUD, Aug. 1999
//--------------------------------------------------------------------------
#include "RhoBase/TCandList.h"
#include "RhoBase/TCandidate.h"
#include "RhoTools/TEventShape.h"

ClassImp(TEventShape)

TBuffer &operator>>(TBuffer &buf, TEventShape *&obj)
{
   obj = (TEventShape *) buf.ReadObject(TEventShape::Class());
   return buf;
}

TEventShape::TEventShape() 
{
  _tot4mom = TLorentzVector(0.,0.,0.,0.);
  _fw2 = 0;
  _totm = 0;
  _totE = 0;
  _totp = 0;
  _totpt = 0;
  _totabsmom = 0;
}

TEventShape::TEventShape(TCandList &l)
{
  _tot4mom = TLorentzVector(0.,0.,0.,0.);
  _totabsmom = 0;
  Double_t fw2n = 0;
  Int_t nl = l.GetNumberOfTracks();
  for (Int_t i=0; i<nl; ++i) {
    TCandidate *c1 = l.Get(i);
    TVector3 p1 = c1->P3();
    _totabsmom += p1.Mag();
    _tot4mom += c1->P4();
    for (Int_t j=i; j<nl; ++j) {
      TCandidate *c2 = l.Get(j);
      TVector3 p2 = c2->P3();
      Double_t cosal = p1.Dot(p2) /(p1.Mag()*p2.Mag());
      Double_t legendre2 = 3.*cosal*cosal - 1.;
      if (i == j) legendre2 = legendre2 / 2.;
      fw2n += legendre2 * p1.Mag() * p2.Mag();
    }
  }
  _fw2 = _totabsmom>0 ? fw2n/(_totabsmom*_totabsmom) : 0.0;
  _totm = _tot4mom.M();
  _totE = _tot4mom.E();
  _totp = _tot4mom.Rho();
  _totpt = _tot4mom.Perp();
}

TEventShape::~TEventShape() {}
