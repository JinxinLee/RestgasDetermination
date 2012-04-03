// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcOptMatF2G12.h"
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
PndDrcOptMatF2G12::PndDrcOptMatF2G12()
{
  fName = "F2G12";

  fB1 = 1.34533359;
  fB2 = 0.209073176;
  fB3 = 0.937357162;
  fC1 = 0.00997743871;
  fC2 = 0.0470450767;
  fC3 = 111.886764;
}
//----------------------------------------------------------------------
PndDrcOptMatF2G12* PndDrcOptMatF2G12::Clone() const
{
  return new PndDrcOptMatF2G12(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatF2G12::Copy(const PndDrcOptMatF2G12& mat)
{
  fB1  = mat.fB1;
  fC1  = mat.fC1;
  fB2  = mat.fB2;
  fC2  = mat.fC2;
  fB3  = mat.fB3;
  fC3  = mat.fC3;
}//----------------------------------------------------------------------
PndDrcOptMatF2G12::PndDrcOptMatF2G12(const PndDrcOptMatF2G12& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatF2G12::PndDrcOptMatF2G12"
			    <<"(const PndDrcOptMatF2G12&) "
			    <<mat.fName<<endl;
  Copy(mat);
}
//----------------------------------------------------------------------
PndDrcOptMatF2G12& PndDrcOptMatF2G12::operator=(const PndDrcOptMatF2G12& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatF2G12::operator="
			    <<"(const PndDrcOptMatF2G12&) "
			    <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatF2G12::RefIndex(const double lambda) const
{

  if (lambda<0) return 1.62; // average value.

  double lam2 = lambda/1000 * lambda/1000; // um2

  return sqrt(1.0L +
	      fB1*lam2/(lam2-fC1) +
	      fB2*lam2/(lam2-fC2) +
	      fB3*lam2/(lam2-fC3));
}
//----------------------------------------------------------------------
double PndDrcOptMatF2G12::RefIndexDeriv(const double lambda) const
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
bool PndDrcOptMatF2G12::AbsorptionFlag(double lambda, double length) const
{

  // Rayleigh scattering.
  // data from Schott data sheets of 10mm sample for F2 not F2G12
  // normally the Cerium doping shifts the spectrum towards red

  const static double kLam[20] = {1060,    700,   660,   620,   580,
				  546,     500,   460,   436,   420,
				  405,     400,   390,   380,   370,
				  365,     350,   334,   320,   310};
  const static double kC[20]   = {9995,   9995,  4995,  9995,  9995,
				  9995,   4995,  3328,  2495,  2495,
				  1662,   1662,  1106, 661.7, 411.6,
				  298,   106.1, 16.22,  4.53,  2.17};
  // C = -10mm /ln (t_i)

  double clarity;

  if (lambda>1060)
    {
      clarity=5000;
    }
  else if (lambda<310)
    {
      return true; // cut off
    }
  else
    {
      // find right bin
      int ibin=-1;
      for (int i=1; i<20; i++)
	{
	  if (lambda<kLam[i-1] && lambda>=kLam[i])
	    {
	      ibin = i;
	    }
	}
      if (ibin==-1)
	{
	  cerr<<" *** PndDrcOptMatF2G12::absorptionFlag: "
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
