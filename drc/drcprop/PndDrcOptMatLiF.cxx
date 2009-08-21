// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcOptMatLiF.h"

//#include "PndDrcOptMatAbs.h"
//
//#include "TObject.h"
//#include "TVector3.h"
//#include "TRandom.h"
//
#include <iostream>
using std::cout;
//using std::cerr;
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
PndDrcOptMatLiF::PndDrcOptMatLiF()
{
  fName = "LiF";

  double temp=300; // Kelvin

  CalcSellmeier(fB1,fB2,fB3,fC1,fC2,fC3,temp);

  //cout<<" PndDrcOptMatLiF::PndDrcOptMatLiF: check: n(400)="<<refIndex(400)<<endl;

}
//----------------------------------------------------------------------
PndDrcOptMatLiF* PndDrcOptMatLiF::Clone() const
{
  return new PndDrcOptMatLiF(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatLiF::Copy(const PndDrcOptMatLiF& mat)
{
  fB1  = mat.fB1;
  fC1  = mat.fC1;
  fB2  = mat.fB2;
  fC2  = mat.fC2;
  fB3  = mat.fB3;
  fC3  = mat.fC3;
  fRan = mat.fRan;
}//----------------------------------------------------------------------
PndDrcOptMatLiF::PndDrcOptMatLiF(const PndDrcOptMatLiF& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatLiF::PndDrcOptMatLiF"
			    <<"(const PndDrcOptMatLiF&) "
			    <<mat.fName<<endl;
  Copy(mat);
}
//----------------------------------------------------------------------
PndDrcOptMatLiF& PndDrcOptMatLiF::operator=(const PndDrcOptMatLiF& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatLiF::operator="
			    <<"(const PndDrcOptMatLiF&) "
			    <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatLiF::RefIndex(const double lambda) const
{

  if (lambda<0) return 1.74; // average value.

  double lam2 = lambda/1000 * lambda/1000; // um2

  return sqrt(1.0L +
	      fB1*lam2/(lam2-fC1) +
	      fB2*lam2/(lam2-fC2) +
	      fB3*lam2/(lam2-fC3));
}
//----------------------------------------------------------------------
double PndDrcOptMatLiF::RefIndexDeriv(const double lambda) const
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
bool PndDrcOptMatLiF::AbsorptionFlag(double lambda, double length) const
{
  // not yet implemented
  return false; // no absorption.
}
//----------------------------------------------------------------------
void PndDrcOptMatLiF::CalcSellmeier(double& b1, double& b2, double& b3,
				 double& c1, double& c2, double& c3,
				 double temp)
{
  static const double kS1[5] = {9.33385e-1,1.84043e-5,-1.68868e-7,4.08865e-12,3.36861e-13};
  static const double kS2[5] = {2.91928,6.91076e-3,8.47693e-6,-3.20709e-8,1.36856e-10};
  static const double kS3[5] = {2.73548,6.71841e-4,-1.24327e-5,-2.97405e-8,1.21434e-10};

  static const double kL1[5] = {7.15768e-2,8.52651e-6,-5.56075e-8,2.37456e-10,-3.54594e-13};
  static const double kL2[5] = {2.93086e1,3.64286e-2,3.22167e-5,7.85825e-8,-2.81432e-10};
  static const double kL3[5] = {2.94083e1,1.04614e-4,-8.56271e-5,-1.18786e-7,7.82576e-10};// nm

  b1=0;
  b2=0;
  b3=0;
  c1=0;
  c2=0;
  c3=0;
  for (int i=0; i<5; i++)
    {
      b1 += kS1[i]*pow(temp,i);
      b2 += kS2[i]*pow(temp,i);
      b3 += kS3[i]*pow(temp,i);
      c1 += kL1[i]*pow(temp,i);
      c2 += kL2[i]*pow(temp,i);
      c3 += kL3[i]*pow(temp,i);
    }
  c1 = c1*c1;
  c2 = c2*c2;
  c3 = c3*c3;
}

