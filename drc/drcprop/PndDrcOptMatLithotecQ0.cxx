// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcOptMatLithotecQ0.h"
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
PndDrcOptMatLithotecQ0::PndDrcOptMatLithotecQ0()
{
  fName = "Lithotec Q0";

  fF1 = 28.17847;
  fE1 = 10.28086;
  fF2 = 207.5092;
  fE2 = 15.77682;
}
//----------------------------------------------------------------------
PndDrcOptMatLithotecQ0* PndDrcOptMatLithotecQ0::Clone() const
{
  return new PndDrcOptMatLithotecQ0(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatLithotecQ0::Copy(const PndDrcOptMatLithotecQ0& mat)
{
  fF1  = mat.fF1;
  fE1  = mat.fE1;
  fF2  = mat.fF2;
  fE2  = mat.fE2;
}//----------------------------------------------------------------------
PndDrcOptMatLithotecQ0::PndDrcOptMatLithotecQ0(const PndDrcOptMatLithotecQ0& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatLithotecQ0::PndDrcOptMatLithotecQ0"
			    <<"(const PndDrcOptMatLithotecQ0&) "
			    <<mat.fName<<endl;
  Copy(mat);
}
//----------------------------------------------------------------------
PndDrcOptMatLithotecQ0& PndDrcOptMatLithotecQ0::operator=(const PndDrcOptMatLithotecQ0& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatLithotecQ0::operator="
			    <<"(const PndDrcOptMatLithotecQ0&) "
			    <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatLithotecQ0::RefIndex(const double lambda) const
{
  static const double kPi    = 3.1415926535;
  static const double kHbarc = 197.3269602;  // Mev fm

  if (lambda<0) return 1.453; // average value.

  //cout<<" PndDrcOptMatLithotecQ0::refIndex: lambda = "<<lambda<<endl;//###
  double e = 2*kPi*kHbarc/lambda; // energy [eV]

  if (e<fE1)
    {
      // refraction index
      return sqrt(1.0L + fF1/(fE1*fE1-e*e) + fF2/(fE2*fE2-e*e));
    }
  else
    {
      cerr<<" *** PndDrcOptMatLithotecQ0::refIndex: too short wavelength. "
	  <<" lambda = "<<lambda<<" ... abort"<<endl;
      exit(EXIT_FAILURE);
    }

  return 1.47;
}
//----------------------------------------------------------------------
double PndDrcOptMatLithotecQ0::RefIndexDeriv(const double lambda) const
{
  static const double kPi    = 3.1415926535;
  static const double kHbarc = 197.3269602;  // Mev fm

  double e = 2*kPi*kHbarc/lambda; // energy [eV]

  if (e<fE1)
    {
      // refraction index
      return e/RefIndex(lambda) * ( fF1/(fE1*fE1-e*e)/(fE1*fE1-e*e) +
				    fF2/(fE2*fE2-e*e)/(fE2*fE2-e*e)  )
	* (-2*kPi*kHbarc/lambda/lambda);
    }
  else
    {
      cerr<<" *** PndDrcOptMatLithotecQ0::refIndexDeriv: too short wavelength. "
	  <<" lambda = "<<lambda<<" ... abort"<<endl;
      exit(EXIT_FAILURE);
    }

 return 0;
}
//----------------------------------------------------------------------
bool PndDrcOptMatLithotecQ0::AbsorptionFlag(double lambda, double length) const
{
  // Rayleigh scattering.
  static const double kClarity = 2100*1000; // BaBar: @ 633 nm in mm (2100 m) (=335m at 400nm)

  double trans = exp(-(length)/(kClarity*pow(lambda/633,4)));


  double cmp1   = gRandom->Uniform(1.0);


  //cout<<" trans, cmp1 "<<trans<<" "<<cmp1<<endl;

  //for (int i=0; i<10; i++)
  //cout<<gRandom->Uniform(1.0)<<endl;

  if (cmp1>trans)
    {
      return true; // absorbed
    }

  return false; // no absorption.
}
