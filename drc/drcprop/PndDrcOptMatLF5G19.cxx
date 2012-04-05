// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2012
//-----------------------------------------------------
#include "PndDrcOptMatLF5G19.h"
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
PndDrcOptMatLF5G19::PndDrcOptMatLF5G19()
{
  fName = "LF5G19";

  fB1 = 1.34611327;
  fB2 = 0.142428018;
  fB3 = 0.900477176;
  fC1 = 0.0097174385;
  fC2 = 0.0501911619;
  fC3 = 111.959703;
}
//----------------------------------------------------------------------
PndDrcOptMatLF5G19* PndDrcOptMatLF5G19::Clone() const
{
  return new PndDrcOptMatLF5G19(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatLF5G19::Copy(const PndDrcOptMatLF5G19& mat)
{
  fB1  = mat.fB1;
  fC1  = mat.fC1;
  fB2  = mat.fB2;
  fC2  = mat.fC2;
  fB3  = mat.fB3;
  fC3  = mat.fC3;
}//----------------------------------------------------------------------
PndDrcOptMatLF5G19::PndDrcOptMatLF5G19(const PndDrcOptMatLF5G19& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatLF5G19::PndDrcOptMatLF5G19"
			    <<"(const PndDrcOptMatLF5G19&) "
			    <<mat.fName<<endl;
  Copy(mat);
}
//----------------------------------------------------------------------
PndDrcOptMatLF5G19& PndDrcOptMatLF5G19::operator=(const PndDrcOptMatLF5G19& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatLF5G19::operator="
			    <<"(const PndDrcOptMatLF5G19&) "
			    <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatLF5G19::RefIndex(const double lambda) const
{

  if (lambda<0) return 1.60; // average value.

  double lam2 = lambda/1000 * lambda/1000; // um2

  return sqrt(1.0L +
	      fB1*lam2/(lam2-fC1) +
	      fB2*lam2/(lam2-fC2) +
	      fB3*lam2/(lam2-fC3));
}
//----------------------------------------------------------------------
double PndDrcOptMatLF5G19::RefIndexDeriv(const double lambda) const
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
bool PndDrcOptMatLF5G19::AbsorptionFlag(double lambda, double length) const
{

  // Rayleigh scattering.
  // data from Schott data sheets of 10mm sample

  const static double kLam[14] = {1060,    700,   660,   620,   580,
				  546,     500,   460,   436,   420,
				  405,     400,   390,   380};
  const static double kC[14]   = { 9999,   3328,  1995,  1424,  1106,
				   709.3,  365.3, 135.8, 51.02, 23.81,
				  10.39,  7.768, 4.153, 2};
  // C = -10mm /ln (t_i)

  double clarity;

  if (lambda>1060)
    {
      clarity=9999;
    }
  else if (lambda<380)
    {
      return true; // cut off
    }
  else
    {
      // find right bin
      int ibin=-1;
      for (int i=1; i<14; i++)
	{
	  if (lambda<kLam[i-1] && lambda>=kLam[i])
	    {
	      ibin = i;
	    }
	}
      if (ibin==-1)
	{
	  cerr<<" *** PndDrcOptMatLF5G19::absorptionFlag: "
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
