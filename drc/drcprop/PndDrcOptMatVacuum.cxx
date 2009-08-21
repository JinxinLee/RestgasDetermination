// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcOptMatVacuum.h"

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


//----------------------------------------------------------------------
PndDrcOptMatVacuum::PndDrcOptMatVacuum()
{
	fName = "vacuum";
}
//----------------------------------------------------------------------
PndDrcOptMatVacuum* PndDrcOptMatVacuum::Clone() const
{
  return new PndDrcOptMatVacuum(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatVacuum::Copy(const PndDrcOptMatVacuum& mat)
{
}//----------------------------------------------------------------------
PndDrcOptMatVacuum::PndDrcOptMatVacuum(const PndDrcOptMatVacuum& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatVacuum::PndDrcOptMatVacuum"
			    <<"(const PndDrcOptMatVacuum&) "
			    <<mat.fName<<endl;
  Copy(mat);
}
//----------------------------------------------------------------------
PndDrcOptMatVacuum& PndDrcOptMatVacuum::operator=(const PndDrcOptMatVacuum& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatVacuum::operator="
			    <<"(const PndDrcOptMatVacuum&) "
			    <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatVacuum::RefIndex(const double lambda) const
{
  return 1.00;
}
//----------------------------------------------------------------------
double PndDrcOptMatVacuum::RefIndexDeriv(const double lambda) const
{
 return 0;
}
//----------------------------------------------------------------------
bool PndDrcOptMatVacuum::AbsorptionFlag(double lambda, double length) const
{
  return false; // no absorption.
}
