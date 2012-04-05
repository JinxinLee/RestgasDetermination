// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2012
//-----------------------------------------------------
#include "PndDrcOptMatK5G20.h"
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
PndDrcOptMatK5G20::PndDrcOptMatK5G20()
{
  fName = "K5G20";

  fB1 = 1.14094396;
  fB2 = 0.14500119;
  fB3 = 37.4705786;
  fC1 = 0.00694945478;
  fC2 = 0.0310574444;
  fC3 = 4536.25624;
}
//----------------------------------------------------------------------
PndDrcOptMatK5G20* PndDrcOptMatK5G20::Clone() const
{
  return new PndDrcOptMatK5G20(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatK5G20::Copy(const PndDrcOptMatK5G20& mat)
{
  fB1  = mat.fB1;
  fC1  = mat.fC1;
  fB2  = mat.fB2;
  fC2  = mat.fC2;
  fB3  = mat.fB3;
  fC3  = mat.fC3;
}//----------------------------------------------------------------------
PndDrcOptMatK5G20::PndDrcOptMatK5G20(const PndDrcOptMatK5G20& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatK5G20::PndDrcOptMatK5G20"
			    <<"(const PndDrcOptMatK5G20&) "
			    <<mat.fName<<endl;
  Copy(mat);
}
//----------------------------------------------------------------------
PndDrcOptMatK5G20& PndDrcOptMatK5G20::operator=(const PndDrcOptMatK5G20& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatK5G20::operator="
			    <<"(const PndDrcOptMatK5G20&) "
			    <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatK5G20::RefIndex(const double lambda) const
{

  if (lambda<0) return 1.52; // average value.

  double lam2 = lambda/1000 * lambda/1000; // um2

  return sqrt(1.0L +
	      fB1*lam2/(lam2-fC1) +
	      fB2*lam2/(lam2-fC2) +
	      fB3*lam2/(lam2-fC3));
}
//----------------------------------------------------------------------
double PndDrcOptMatK5G20::RefIndexDeriv(const double lambda) const
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
bool PndDrcOptMatK5G20::AbsorptionFlag(double lambda, double length) const
{

  // Rayleigh scattering.
  // data from Schott data sheets of 10mm sample

  const static double kLam[16] = {1060,    700,   660,   620,   580,
				  546,     500,   460,   436,   420,
				  405,     400,   390,   380,   370, 365};
  const static double kC[16]   = { 9995,   3328,  1995,  1662,  1424,
				    995,  620, 339.8, 212.4, 126.5,
				   64.80,  50.70, 26.53, 12.25, 4.901, 2.824};
  // C = -10mm /ln (t_i)

  double clarity;

  if (lambda>1060)
    {
      clarity=9995;
    }
  else if (lambda<365)
    {
      return true; // cut off
    }
  else
    {
      // find right bin
      int ibin=-1;
      for (int i=1; i<16; i++)
	{
	  if (lambda<kLam[i-1] && lambda>=kLam[i])
	    {
	      ibin = i;
	    }
	}
      if (ibin==-1)
	{
	  cerr<<" *** PndDrcOptMatK5G20::absorptionFlag: "
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
