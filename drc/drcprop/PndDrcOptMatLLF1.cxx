// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcOptMatLLF1.h"

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
PndDrcOptMatLLF1::PndDrcOptMatLLF1()
{
  fB1 = 1.21640125;
  fB2 = 0.13366454;
  fB3 = 0.883399468;
  fC1 = 0.00857807248;
  fC2 = 0.0420143003;
  fC3 = 107.59306;

}
//----------------------------------------------------------------------
PndDrcOptMatLLF1* PndDrcOptMatLLF1::Clone() const
{
  return new PndDrcOptMatLLF1(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatLLF1::Copy(const PndDrcOptMatLLF1& mat)
{
  fB1  = mat.fB1;
  fC1  = mat.fC1;
  fB2  = mat.fB2;
  fC2  = mat.fC2;
  fB3  = mat.fB3;
  fC3  = mat.fC3;
  fRan = mat.fRan;
}//----------------------------------------------------------------------
PndDrcOptMatLLF1::PndDrcOptMatLLF1(const PndDrcOptMatLLF1& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatLLF1::PndDrcOptMatLLF1"
			    <<"(const PndDrcOptMatLLF1&) "  
			    <<mat.fName<<endl;
  Copy(mat);
} 
//----------------------------------------------------------------------
PndDrcOptMatLLF1& PndDrcOptMatLLF1::operator=(const PndDrcOptMatLLF1& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatLLF1::operator="
			    <<"(const PndDrcOptMatLLF1&) "  
			    <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatLLF1::RefIndex(const double lambda) const
{

  if (lambda<0) return 1.74; // average value.

  double lam2 = lambda/1000 * lambda/1000; // um2
  
  return sqrt(1.0L + 
	      fB1*lam2/(lam2-fC1) +
	      fB2*lam2/(lam2-fC2) +
	      fB3*lam2/(lam2-fC3));
}  
//----------------------------------------------------------------------
double PndDrcOptMatLLF1::RefIndexDeriv(const double lambda) const
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
bool PndDrcOptMatLLF1::AbsorptionFlag(double lambda, double length) const
{

  // Rayleigh scattering. 
  // data from Schott data sheets of 10mm sample

  const static double kLam[21] = {1060,    700,   650,   620,   580,
				 546,     500,   460,   436,   420,
				 405,     400,   390,   380,   370,
				 365,     350,   334,   320,   310,
				 300};
  const static double kC[21]   = {5000,   5000,  5000,  5000,  5000,
				 5000,   5000,  5000,  5000,  5000,
				 5000,   3328,  3328,  1995,  1662,
				 1245,  550.5, 118.4, 20.78, 7.007,
				2.556};
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
	  cerr<<" *** PndDrcOptMatLLF1::absorptionFlag: "
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
