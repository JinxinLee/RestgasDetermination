// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2012
//-----------------------------------------------------
#include "PndDrcOptMatPbF2.h"
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
PndDrcOptMatPbF2::PndDrcOptMatPbF2()
{
  fName = "PbF2";

  fB1 = 0.66959342;
  fB2 = 1.3086319;
  fB3 = 0.01670641;
  fB4 = 2007.8865;
  
  fC1 = 0.00034911;
  fC2 = 0.17144455;
  fC3 = 0.28125513;
  fC4 = 796.67469;
  
}
//----------------------------------------------------------------------
PndDrcOptMatPbF2* PndDrcOptMatPbF2::Clone() const
{
  return new PndDrcOptMatPbF2(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatPbF2::Copy(const PndDrcOptMatPbF2& mat)
{
  fB1  = mat.fB1;
  fC1  = mat.fC1;
  fB2  = mat.fB2;
  fC2  = mat.fC2;
  fB3  = mat.fB3;
  fC3  = mat.fC3;
  fB4  = mat.fB4;
  fC4  = mat.fC4;
}//----------------------------------------------------------------------
PndDrcOptMatPbF2::PndDrcOptMatPbF2(const PndDrcOptMatPbF2& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatPbF2::PndDrcOptMatPbF2"
			    <<"(const PndDrcOptMatPbF2&) "
			    <<mat.fName<<endl;
  Copy(mat);
}
//----------------------------------------------------------------------
PndDrcOptMatPbF2& PndDrcOptMatPbF2::operator=(const PndDrcOptMatPbF2& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatPbF2::operator="
			    <<"(const PndDrcOptMatPbF2&) "
			    <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatPbF2::RefIndex(const double lambda) const
{

  if (lambda<0) return 1.52; // average value.

  double lam2 = lambda/1000 * lambda/1000; // um2

  return sqrt(1.0L +
	      fB1*lam2/(lam2-fC1) +
	      fB2*lam2/(lam2-fC2) +
	      fB3*lam2/(lam2-fC3) +
	      fB4*lam2/(lam2-fC4));
}
//----------------------------------------------------------------------
double PndDrcOptMatPbF2::RefIndexDeriv(const double lambda) const
{
  double lam  = lambda/1000;
  double lam2 = lam*lam;
  //double lam3 = lam2*lam;

  return (   (-fB1*fC1*lam)/((lam2-fC1)*(lam2-fC1)) +
	     (-fB2*fC2*lam)/((lam2-fC2)*(lam2-fC2)) +
	     (-fB3*fC3*lam)/((lam2-fC3)*(lam2-fC3)) +
	     (-fB4*fC4*lam)/((lam2-fC4)*(lam2-fC4))  )
    / RefIndex(lambda) / 1000;

}
//----------------------------------------------------------------------
bool PndDrcOptMatPbF2::AbsorptionFlag(double lambda, double length) const
{

  // Rayleigh scattering.
  // P. Achenbach et al. /Nucl. Instr. and Meth. in Phys. Res. A 416 (1998) 357

  // C = -160mm /ln (t_i)

  const static double kLam[8] = {800,     700,   600,   500,   400,
				  300,     285,   245};
  const static double kC[8]   = { 9999,   9999,  9999,  9999,  9999,
				  2828.3,     1518.6, 1};

  double clarity;

  if (lambda>800)
    {
      clarity=9999;
    }
  else if (lambda<245)
    {
      return true; // cut off
    }
  else
    {
      // find right bin
      int ibin=-1;
      for (int i=1; i<8; i++)
	{
	  if (lambda<kLam[i-1] && lambda>=kLam[i])
	    {
	      ibin = i;
	    }
	}
      if (ibin==-1)
	{
	  cerr<<" *** PndDrcOptMatPbF2::absorptionFlag: "
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
