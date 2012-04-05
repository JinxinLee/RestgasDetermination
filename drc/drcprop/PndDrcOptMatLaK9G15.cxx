// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2012
//-----------------------------------------------------
#include "PndDrcOptMatLaK9G15.h"
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
PndDrcOptMatLaK9G15::PndDrcOptMatLaK9G15()
{
  fName = "LaK9G15";

  fB1 = 1.28773667;
  fB2 = 0.518244853;
  fB3 = 26.1756109;
  fC1 = 0.0055754192;
  fC2 = 0.0223679524;
  fC3 = 1892.2533;
}
//----------------------------------------------------------------------
PndDrcOptMatLaK9G15* PndDrcOptMatLaK9G15::Clone() const
{
  return new PndDrcOptMatLaK9G15(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatLaK9G15::Copy(const PndDrcOptMatLaK9G15& mat)
{
  fB1  = mat.fB1;
  fC1  = mat.fC1;
  fB2  = mat.fB2;
  fC2  = mat.fC2;
  fB3  = mat.fB3;
  fC3  = mat.fC3;
}//----------------------------------------------------------------------
PndDrcOptMatLaK9G15::PndDrcOptMatLaK9G15(const PndDrcOptMatLaK9G15& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatLaK9G15::PndDrcOptMatLaK9G15"
			    <<"(const PndDrcOptMatLaK9G15&) "
			    <<mat.fName<<endl;
  Copy(mat);
}
//----------------------------------------------------------------------
PndDrcOptMatLaK9G15& PndDrcOptMatLaK9G15::operator=(const PndDrcOptMatLaK9G15& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatLaK9G15::operator="
			    <<"(const PndDrcOptMatLaK9G15&) "
			    <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatLaK9G15::RefIndex(const double lambda) const
{

  if (lambda<0) return 1.69; // average value.

  double lam2 = lambda/1000 * lambda/1000; // um2

  return sqrt(1.0L +
	      fB1*lam2/(lam2-fC1) +
	      fB2*lam2/(lam2-fC2) +
	      fB3*lam2/(lam2-fC3));
}
//----------------------------------------------------------------------
double PndDrcOptMatLaK9G15::RefIndexDeriv(const double lambda) const
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
bool PndDrcOptMatLaK9G15::AbsorptionFlag(double lambda, double length) const
{

  // Rayleigh scattering.
  // data from Schott data sheets of 10mm sample

  const static double kLam[14] = {1060,    700,   660,   620,   580,
				  546,     500,   460,   436,   420,
				  405,     400,   390,   380};
  const static double kC[14]   = { 4995,   1662,  1424,  1106, 904.1,
				  661.7,  339.8, 118.4, 44.56, 21.94,
				  10.39,  8.123, 4.772, 2.740};
  // C = -10mm /ln (t_i)

  double clarity;

  if (lambda>1060)
    {
      clarity=4995;
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
	  cerr<<" *** PndDrcOptMatLaK9G15::absorptionFlag: "
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
