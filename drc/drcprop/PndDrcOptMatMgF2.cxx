// ----------------------------------------------------
// This file belongs to the ray tracing framework#include <list>
#include <list>
using std::list;

// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcOptMatMgF2.h"
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
PndDrcOptMatMgF2::PndDrcOptMatMgF2()
{
  fName = "MgF2";

  fB1 = 0.487551080;
  fB2 = 0.398750310;
  fB3 = 2.31203530;
  fC1 = 0.001882178;
  fC2 = 0.00895188847;
  fC3 = 566.135591;
}
//----------------------------------------------------------------------
PndDrcOptMatMgF2* PndDrcOptMatMgF2::Clone() const
{
  return new PndDrcOptMatMgF2(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatMgF2::Copy(const PndDrcOptMatMgF2& mat)
{
  fB1  = mat.fB1;
  fC1  = mat.fC1;
  fB2  = mat.fB2;
  fC2  = mat.fC2;
  fB3  = mat.fB3;
  fC3  = mat.fC3;
}//----------------------------------------------------------------------
PndDrcOptMatMgF2::PndDrcOptMatMgF2(const PndDrcOptMatMgF2& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatMgF2::PndDrcOptMatMgF2"
			    <<"(const PndDrcOptMatMgF2&) "
			    <<mat.fName<<endl;
  Copy(mat);
}
//----------------------------------------------------------------------
PndDrcOptMatMgF2& PndDrcOptMatMgF2::operator=(const PndDrcOptMatMgF2& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatMgF2::operator="
			    <<"(const PndDrcOptMatMgF2&) "
			    <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatMgF2::RefIndex(const double lambda) const
{

  if (lambda<0) return 1.38; // average value.

  double lam2 = lambda/1000 * lambda/1000; // um2

  return sqrt(1.0L +
	      fB1*lam2/(lam2-fC1) +
	      fB2*lam2/(lam2-fC2) +
	      fB3*lam2/(lam2-fC3));
}
//----------------------------------------------------------------------
double PndDrcOptMatMgF2::RefIndexDeriv(const double lambda) const
{
  double lam  = lambda/1000;
  double lam2 = lam*lam;
  //double lam3 = lam2*lam;

  return (   (-fB1*fC1*lam)/((lam2-fC1)*(lam2-fC1)) +
	     (-fB2*fC2*lam)/((lam2-fC2)*(lam2-fC2)) +
	     (-fB3*fC3*lam)/((lam2-fC3)*(lam2-fC3))  )
    / RefIndex(lambda) / 1000;

}
//----------------------------------------------------------------------
bool PndDrcOptMatMgF2::AbsorptionFlag(double lambda, double length) const
{

  return false; // no absorption.
}
