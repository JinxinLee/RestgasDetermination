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

  fA    = 0.5189;
  fB    = 8.1605;
  fC    = 17.5291;
  fB0   = -0.4195;
  fC0   = 2.9587;
  fNinf = 1.7614;
  




}
//----------------------------------------------------------------------
PndDrcOptMatTiO2* PndDrcOptMatTiO2::Clone() const
{
  return new PndDrcOptMatTiO2(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatTiO2::Copy(const PndDrcOptMatTiO2& mat)
{
  fA   = mat.fA;
  fB   = mat.fB;
  fC   = mat.fC;
  fB0  = mat.fB0;
  fC0  = mat.fC0;
  fNinf= mat.fNinf;
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

  if (lambda<0) return 2.5; // average value.

  const double pi = 3.1415926535;
  const double  hbarc = 197.326968; // eV nm

  double e = 2*pi*hbarc / lambda;

  return fNinf + (fB0*e + fC0) / (e*e - fB*e + fC);
}
//----------------------------------------------------------------------
double PndDrcOptMatTiO2::RefIndexDeriv(const double lambda) const
{

  const double pi = 3.1415926535;
  const double  hbarc = 197.326968; // eV nm

  double e = 2*pi*hbarc / lambda;

  double dnde = (-e*e*fB0 - e*2*fC0 + fB0*fC + fC0*fB) / (e*e-e*fB+fC) / (e*e-e*fB+fC);
  
  double dedlambda = -2*pi*hbarc/lambda/lambda;
  

  return dnde*dedlambda;
  

}
//----------------------------------------------------------------------
bool PndDrcOptMatTiO2::AbsorptionFlag(double lambda, double length) const
{

  return false; // no absorption.
}
