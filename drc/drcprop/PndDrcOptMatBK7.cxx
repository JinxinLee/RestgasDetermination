// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcOptMatBK7.h"
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
PndDrcOptMatBK7::PndDrcOptMatBK7()
{
  fB1 = 1.03961212;
  fB2 = 0.231792344;
  fB3 = 1.01046945;
  fC1 = 0.00600069867;
  fC2 = 0.0200179144;
  fC3 = 103.560653;

}
//----------------------------------------------------------------------
PndDrcOptMatBK7* PndDrcOptMatBK7::Clone() const
{
  return new PndDrcOptMatBK7(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatBK7::Copy(const PndDrcOptMatBK7& mat)
{
  fB1  = mat.fB1;
  fC1  = mat.fC1;
  fB2  = mat.fB2;
  fC2  = mat.fC2;
  fB3  = mat.fB3;
  fC3  = mat.fC3;
  fRan = mat.fRan;
}//----------------------------------------------------------------------
PndDrcOptMatBK7::PndDrcOptMatBK7(const PndDrcOptMatBK7& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatBK7::PndDrcOptMatBK7"
			    <<"(const PndDrcOptMatBK7&) "  
			    <<mat.fName<<endl;
  Copy(mat);
} 
//----------------------------------------------------------------------
PndDrcOptMatBK7& PndDrcOptMatBK7::operator=(const PndDrcOptMatBK7& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatBK7::operator="
			    <<"(const PndDrcOptMatBK7&) "  
			    <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatBK7::RefIndex(const double lambda) const
{

  if (lambda<0) return 1.74; // average value.

  double lam2 = lambda/1000 * lambda/1000; // um2
  
  return sqrt(1.0L + 
	      fB1*lam2/(lam2-fC1) +
	      fB2*lam2/(lam2-fC2) +
	      fB3*lam2/(lam2-fC3));
}  
//----------------------------------------------------------------------
double PndDrcOptMatBK7::RefIndexDeriv(const double lambda) const
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
bool PndDrcOptMatBK7::AbsorptionFlag(double lambda, double length) const
{

  // Rayleigh scattering. 
  // data from Schott data sheets of 10mm sample

  const static double kLam[21] = {1060,    700,   660,   620,   580,
				 546,     500,   460,   436,   420,
				 405,     400,   390,   380,   370,
				 365,     350,   334,   320,   310,
				 300};
  const static double kC[21]   = {5000,   5000,  5000,  5000,  5000,
				 5000,   5000,  3328,  3328,  3328,
				 3328,   3328,  2495,  1424,  1106,
				 828.3,  298.0, 100.2, 38.26, 18.01,
				8.123};
  // C = -10mm /ln (t_i)
 
  double clarity;

  if (lambda>1060)
    {
      clarity=5000;
    }
  else if (lambda<300)
    {
      return true; // cut off
    }
  else
    {
      // find right bin
      int ibin=-1;
      for (int i=1; i<21; i++)
	{
	  if (lambda<kLam[i-1] && lambda>=kLam[i])
	    {
	      ibin = i;
	    }
	}
      if (ibin==-1)
	{
	  cerr<<" *** PndDrcOptMatBK7::absorptionFlag: "
	      <<"this line should never been hit"<<endl;
	  exit(EXIT_FAILURE);
	}
      double frac = (kLam[ibin-1]-lambda)/(kLam[ibin-1]-kLam[ibin]);
      clarity = kC[ibin-1] + frac * (kC[ibin]-kC[ibin-1]);


    }

  // clarity at lambda!
  //double trans = exp(-(length)/(clarity*pow(lambda/663,4)));
  double trans = exp(-(length)/clarity);
  double cmp   = fRan.Uniform(1.0);


  if (cmp>trans)
    {
      return true; // absorbed
    } 

  return false; // no absorption.
}
