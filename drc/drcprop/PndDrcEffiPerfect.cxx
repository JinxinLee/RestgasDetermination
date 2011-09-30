// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcEffiAbs.h"
#include "PndDrcEffiPerfect.h"
#include <algorithm>
//#include "PndDrcOptMatAbs.h"
//
//#include "TObject.h"
//#include "TVector3.h"
//#include "TRandom.h"
//
#include <iostream>
using std::cout;
using std::cerr;
//using std::cin;
using std::endl;
//
#include <cmath>

//----------------------------------------------------------------------
PndDrcEffiPerfect::PndDrcEffiPerfect()
{
  fName = "effi_perfect";
}
//----------------------------------------------------------------------
PndDrcEffiPerfect* PndDrcEffiPerfect::Clone() const
{
  return new PndDrcEffiPerfect(*this);
}
//----------------------------------------------------------------------
void PndDrcEffiPerfect::Copy(const PndDrcEffiPerfect& effi)
{
}//----------------------------------------------------------------------
PndDrcEffiPerfect::PndDrcEffiPerfect(const PndDrcEffiPerfect& effi)
  : PndDrcEffiAbs(effi)
{
  if (effi.fVerbosity>=1) cout<<"  PndDrcEffiPerfect::PndDrcEffiPerfect"
			    <<"(const PndDrcEffiPerfect&) "
			    <<effi.fName<<endl;
  Copy(effi);
}
//----------------------------------------------------------------------
PndDrcEffiPerfect& PndDrcEffiPerfect::operator=(const PndDrcEffiPerfect& effi)
{
  if (effi.fVerbosity>=1) cout<<"  PndDrcEffiPerfect::operator="
			    <<"(const PndDrcEffiPerfect&) "
			    <<effi.fName<<endl;
  if (&effi != this)
    {
      static_cast<PndDrcEffiAbs&>((*this)) = effi; // assignment of base class part.
      Copy(effi);
    }
  return *this;
}
//----------------------------------------------------------------------
bool PndDrcEffiPerfect::EffiFlag(double lambda, double cosine) const
{
  return true;
}
