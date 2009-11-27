// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcOptMatMarcol7.h"
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
PndDrcOptMatMarcol7::PndDrcOptMatMarcol7()
{
  fName = "Marcol7";

  fNd      = 1.4684;
  fB       = 4240;   // nm2
  fLambdaD = 589.3;  // nm
  
}
//----------------------------------------------------------------------
PndDrcOptMatMarcol7* PndDrcOptMatMarcol7::Clone() const
{
  return new PndDrcOptMatMarcol7(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatMarcol7::Copy(const PndDrcOptMatMarcol7& mat)
{

  fNd      = mat.fNd;
  fB       = mat.fB;
  fLambdaD = mat.fLambdaD;
  fRan = mat.fRan;
}//----------------------------------------------------------------------
PndDrcOptMatMarcol7::PndDrcOptMatMarcol7(const PndDrcOptMatMarcol7& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatMarcol7::PndDrcOptMatMarcol7"
			     <<"(const PndDrcOptMatMarcol7&) "
			     <<mat.fName<<endl;
  Copy(mat);
}
//----------------------------------------------------------------------
PndDrcOptMatMarcol7& PndDrcOptMatMarcol7::operator=(const PndDrcOptMatMarcol7& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatMarcol7::operator="
			     <<"(const PndDrcOptMatMarcol7&) "
			     <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatMarcol7::RefIndex(const double lambda) const
{

  if (lambda<0) return fNd; // average value.

  return fNd + fB*(1.0L/(lambda*lambda) - 1.0L/(fLambdaD*fLambdaD));
  
}
//----------------------------------------------------------------------
double PndDrcOptMatMarcol7::RefIndexDeriv(const double lambda) const
{
  return -2*fB/(pow(lambda,3));
}
//----------------------------------------------------------------------
bool PndDrcOptMatMarcol7::AbsorptionFlag(double lambda, double length) const
{
  // lambda nm
  // length mm

  double att;
  
  if      (lambda<320) att = 0.01;   // see thesis Michel Sorel (MiniBooNE) page 156
  if      (lambda<400) att = 0.001;
  else if (lambda<600) att = 0.0002;
  else                 att = 0.0001;

  if (fRan.Uniform(1.0) < att*length) 
    {
      return true; //absorbed
    }
  
  return false; // no absorption.
}
