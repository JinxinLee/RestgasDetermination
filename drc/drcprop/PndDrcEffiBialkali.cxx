// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcEffiAbs.h"
#include "PndDrcEffiBialkali.h"
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

//PndDrcEffiBialkali::fRan.SetSeed();
TRandom3  PndDrcEffiBialkali::fRan;

//----------------------------------------------------------------------
PndDrcEffiBialkali::PndDrcEffiBialkali()
{
  fName = "effi_perfect";

  for (int i=0; i<70; i++) fEffiArray[i]=0;

  // the argument is the wavelenght (nm) divided by 10.
  fEffiArray[29] = 0.06;
  fEffiArray[30] = 0.10;
  fEffiArray[31] = 0.16;
  fEffiArray[32] = 0.22;
  fEffiArray[33] = 0.27;
  fEffiArray[34] = 0.28;
  fEffiArray[35] = 0.285;
  fEffiArray[36] = 0.31;
  fEffiArray[37] = 0.31;
  fEffiArray[38] = 0.30;
  fEffiArray[39] = 0.29;
  fEffiArray[40] = 0.28;
  fEffiArray[41] = 0.27;
  fEffiArray[42] = 0.26;
  fEffiArray[43] = 0.25;
  fEffiArray[44] = 0.24;
  fEffiArray[45] = 0.23;
  fEffiArray[46] = 0.21;
  fEffiArray[47] = 0.19;
  fEffiArray[48] = 0.18;
  fEffiArray[49] = 0.17;
  fEffiArray[50] = 0.16;
  fEffiArray[51] = 0.14;
  fEffiArray[52] = 0.12;
  fEffiArray[53] = 0.10;
  fEffiArray[54] = 0.07;
  fEffiArray[55] = 0.05;
  fEffiArray[56] = 0.04;
  fEffiArray[57] = 0.03;
  fEffiArray[58] = 0.03;
  fEffiArray[59] = 0.02;
  fEffiArray[60] = 0.01;
  fEffiArray[61] = 0.0075;
  fEffiArray[62] = 0.005;
  fEffiArray[63] = 0.0025;



}
//----------------------------------------------------------------------
PndDrcEffiBialkali* PndDrcEffiBialkali::Clone() const
{
  return new PndDrcEffiBialkali(*this);
}
//----------------------------------------------------------------------
void PndDrcEffiBialkali::Copy(const PndDrcEffiBialkali& effi)
{
  fRan       = effi.fRan;
  for (int i=0; i<70; i++)
    {
      fEffiArray[i] = effi.fEffiArray[i];
    }
}//----------------------------------------------------------------------
PndDrcEffiBialkali::PndDrcEffiBialkali(const PndDrcEffiBialkali& effi)
  : PndDrcEffiAbs(effi)
{
  if (effi.fVerbosity>=1) cout<<"  PndDrcEffiBialkali::PndDrcEffiBialkali"
			    <<"(const PndDrcEffiBialkali&) "
			    <<effi.fName<<endl;
  Copy(effi);
}
//----------------------------------------------------------------------
PndDrcEffiBialkali& PndDrcEffiBialkali::operator=(const PndDrcEffiBialkali& effi)
{
  if (effi.fVerbosity>=1) cout<<"  PndDrcEffiBialkali::operator="
			    <<"(const PndDrcEffiBialkali&) "
			    <<effi.fName<<endl;
  if (&effi != this)
    {
      static_cast<PndDrcEffiAbs&>((*this)) = effi; // assignment of base class part.
      Copy(effi);
    }
  return *this;
}
//----------------------------------------------------------------------
bool PndDrcEffiBialkali::EffiFlag(double lambda, double cosine) const
{

  if (lambda < 700 && (fRan.Uniform() < fEffiArray[(int)(lambda/10+0.5)]))
    {
      return true;
    }
  else
    {
      return false;
    }
}
