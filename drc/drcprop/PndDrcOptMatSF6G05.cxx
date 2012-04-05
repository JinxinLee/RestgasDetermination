// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2012
//-----------------------------------------------------
#include "PndDrcOptMatSF6G05.h"
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
PndDrcOptMatSF6G05::PndDrcOptMatSF6G05()
{
  fName = "SF6G05";

  fB1 = 1.62113942;
  fB2 = 0.506586092;
  fB3 = 10.4032298;
  fC1 = 0.0113478992;
  fC2 = 0.0535840223;
  fC3 = 1118.83658;
}
//----------------------------------------------------------------------
PndDrcOptMatSF6G05* PndDrcOptMatSF6G05::Clone() const
{
  return new PndDrcOptMatSF6G05(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatSF6G05::Copy(const PndDrcOptMatSF6G05& mat)
{
  fB1  = mat.fB1;
  fC1  = mat.fC1;
  fB2  = mat.fB2;
  fC2  = mat.fC2;
  fB3  = mat.fB3;
  fC3  = mat.fC3;
}//----------------------------------------------------------------------
PndDrcOptMatSF6G05::PndDrcOptMatSF6G05(const PndDrcOptMatSF6G05& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatSF6G05::PndDrcOptMatSF6G05"
			    <<"(const PndDrcOptMatSF6G05&) "
			    <<mat.fName<<endl;
  Copy(mat);
}
//----------------------------------------------------------------------
PndDrcOptMatSF6G05& PndDrcOptMatSF6G05::operator=(const PndDrcOptMatSF6G05& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatSF6G05::operator="
			    <<"(const PndDrcOptMatSF6G05&) "
			    <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatSF6G05::RefIndex(const double lambda) const
{

  if (lambda<0) return 1.80906; // average value.

  double lam2 = lambda/1000 * lambda/1000; // um2

  return sqrt(1.0L +
	      fB1*lam2/(lam2-fC1) +
	      fB2*lam2/(lam2-fC2) +
	      fB3*lam2/(lam2-fC3));
}
//----------------------------------------------------------------------
double PndDrcOptMatSF6G05::RefIndexDeriv(const double lambda) const
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
bool PndDrcOptMatSF6G05::AbsorptionFlag(double lambda, double length) const
{

  // Rayleigh scattering.
  // data from Schott data sheets of 10mm sample

  const static double kLam[9] = {1060,    700,   660,   620,   580,
				  546,     500,   460,   436};
  const static double kC[9]   = { 9995,   661.7,  495 ,  352.1,  233.1,
				   115.4,  22.56, 4.15 , 2};
  
  // C = -10mm /ln (t_i)

  double clarity;

  if (lambda>1060)
    {
      clarity=9995;
    }
  else if (lambda<436)
    {
      return true; // cut off
    }
  else
    {
      // find right bin
      int ibin=-1;
      for (int i=1; i<9; i++)
	{
	  if (lambda<kLam[i-1] && lambda>=kLam[i])
	    {
	      ibin = i;
	    }
	}
      if (ibin==-1)
	{
	  cerr<<" *** PndDrcOptMatSF6G05::absorptionFlag: "
	      <<"this line should never been hit"<<endl;
	  exit(EXIT_FAILURE);
	}
      double frac = (kLam[ibin-1]-lambda)/(kLam[ibin-1]-kLam[ibin]);
      clarity = kC[ibin-1] + frac * (kC[ibin]-kC[ibin-1]);


    }

  // clarity at lambda!
  //double trans = exp(-(length)/(clarity*pow(lambda/663,4)));
  double trans = exp(-(length)/clarity);
  double cmp   = gRandom->Uniform(1.0);


  if (cmp>trans)
    {
      return true; // absorbed
    }

  return false; // no absorption.
}
