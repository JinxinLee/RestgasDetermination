// ----------------------------------------------------
// This file belongs to the ray tracing framework#include <list>
#include <list>
using std::list;

// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcOptMatTiO2.h"
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
//#include <valarray>
//using std::valarray;
//
//#include <fstream>
//using std::fstream;
//
//#include <string>
//using std::string;
//
//#include <list>
//using std::list;

//----------------------------------------------------------------------
PndDrcOptMatTiO2::PndDrcOptMatTiO2()
{
  fName = "TiO2";

  fC1 = 5.913;
  fC2 = 0.2441;
  fC3 = 0.0803;
}
//----------------------------------------------------------------------
PndDrcOptMatTiO2* PndDrcOptMatTiO2::Clone() const
{
  return new PndDrcOptMatTiO2(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatTiO2::Copy(const PndDrcOptMatTiO2& mat)
{
  fC1  = mat.fC1;
  fC2  = mat.fC2;
  fC3  = mat.fC3;
  fRan = mat.fRan;
}//----------------------------------------------------------------------
PndDrcOptMatTiO2::PndDrcOptMatTiO2(const PndDrcOptMatTiO2& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatTiO2::PndDrcOptMatTiO2"
			    <<"(const PndDrcOptMatTiO2&) "
			    <<mat.fName<<endl;
  Copy(mat);
}
//----------------------------------------------------------------------
PndDrcOptMatTiO2& PndDrcOptMatTiO2::operator=(const PndDrcOptMatTiO2& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatTiO2::operator="
			    <<"(const PndDrcOptMatTiO2&) "
			    <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatTiO2::RefIndex(const double lambda) const
{

  if (lambda<0) return 2.49621; // average value.

  double lam2 = lambda/1000 * lambda/1000; // um2

  return sqrt(fC1+fC2*lam2/(lam2-fC3));
}
//----------------------------------------------------------------------
double PndDrcOptMatTiO2::RefIndexDeriv(const double lambda) const
{
  double lam  = lambda/1000;
  double lam2 = lam*lam;
  //double lam3 = lam2*lam;

  return (   (-fC2*fC3*lam)/((lam2-fC3)*(lam2-fC3)) )
    / RefIndex(lambda) / 1000;

}
//----------------------------------------------------------------------
bool PndDrcOptMatTiO2::AbsorptionFlag(double lambda, double length) const
{

  return false; // no absorption.
}
