// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcOptMatNLAK33A.h"
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

#include <cmath>

//----------------------------------------------------------------------
PndDrcOptMatNLAK33A::PndDrcOptMatNLAK33A()
{
  fB1 = 1.44116999;
  fB2 = 0.571749501;
  fB3 = 1.16605226;
  fC1 = 0.00680933877;
  fC2 = 0.0222291824;
  fC3 = 80.9379555;

}
//----------------------------------------------------------------------
PndDrcOptMatNLAK33A* PndDrcOptMatNLAK33A::Clone() const
{
  return new PndDrcOptMatNLAK33A(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatNLAK33A::Copy(const PndDrcOptMatNLAK33A& mat)
{
  fB1  = mat.fB1;
  fC1  = mat.fC1;
  fB2  = mat.fB2;
  fC2  = mat.fC2;
  fB3  = mat.fB3;
  fC3  = mat.fC3;
  fRan = mat.fRan;
}//----------------------------------------------------------------------
PndDrcOptMatNLAK33A::PndDrcOptMatNLAK33A(const PndDrcOptMatNLAK33A& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatNLAK33A::PndDrcOptMatNLAK33A"
			    <<"(const PndDrcOptMatNLAK33A&) "  
			    <<mat.fName<<endl;
  Copy(mat);
} 
//----------------------------------------------------------------------
PndDrcOptMatNLAK33A& PndDrcOptMatNLAK33A::operator=(const PndDrcOptMatNLAK33A& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatNLAK33A::operator="
			    <<"(const PndDrcOptMatNLAK33A&) "  
			    <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatNLAK33A::RefIndex(const double lambda) const
{

  if (lambda<0) return 1.74; // average value.

  double lam2 = lambda/1000 * lambda/1000; // um2
  
  return sqrt(1.0L + 
	      fB1*lam2/(lam2-fC1) +
	      fB2*lam2/(lam2-fC2) +
	      fB3*lam2/(lam2-fC3));
}  
//----------------------------------------------------------------------
double PndDrcOptMatNLAK33A::RefIndexDeriv(const double lambda) const
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
bool PndDrcOptMatNLAK33A::AbsorptionFlag(double lambda, double length) const
{

  // Rayleigh scattering. 
  // data from Schott data sheets of 10mm sample

  const static double kLam[21] = {1060,    700,   650,   620,   580,
				 546,     500,   460,   436,   420,
				 405,     400,   390,   380,   370,
				 365,     350,   334,   320,   310,
				 300};
  const static double kC[21]   = {5000,   5000,  5000,  5000,  5000,
				 5000,   5000,  1661,  1106, 828.3,
				 521.3, 411.6, 298.0, 195.0, 126.5,
				 100.2, 45.83, 19.64, 9.169, 5.457,
				 3.338};

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
	  cerr<<" *** PndDrcOptMatNLAK33A::absorptionFlag: "
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
