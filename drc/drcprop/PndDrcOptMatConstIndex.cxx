// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
// 
// created 2007
//-----------------------------------------------------
#include "PndDrcOptMatConstIndex.h"

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
PndDrcOptMatConstIndex::PndDrcOptMatConstIndex( double refIndex )
{
    fRefIndex = refIndex;
}
//----------------------------------------------------------------------
PndDrcOptMatConstIndex* PndDrcOptMatConstIndex::Clone() const
{
  return new PndDrcOptMatConstIndex(*this);
}
//----------------------------------------------------------------------
void PndDrcOptMatConstIndex::Copy(const PndDrcOptMatConstIndex& mat)
{
    fRefIndex = mat.fRefIndex;
}//----------------------------------------------------------------------
PndDrcOptMatConstIndex::PndDrcOptMatConstIndex(const PndDrcOptMatConstIndex& mat)
  : PndDrcOptMatAbs(mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatConstIndex::PndDrcOptMatConstIndex"
			    <<"(const PndDrcOptMatConstIndex&) "  
			    <<mat.fName<<endl;
  Copy(mat);
} 
//----------------------------------------------------------------------
PndDrcOptMatConstIndex& PndDrcOptMatConstIndex::operator=(const PndDrcOptMatConstIndex& mat)
{
  if (mat.fVerbosity>=1) cout<<"  PndDrcOptMatConstIndex::operator="
			    <<"(const PndDrcOptMatConstIndex&) "  
			    <<mat.fName<<endl;
  if (&mat != this)
    {
      static_cast<PndDrcOptMatAbs&>((*this)) = mat; // assignment of base class part.
      Copy(mat);
    }
  return *this;
}
//----------------------------------------------------------------------
double PndDrcOptMatConstIndex::RefIndex(const double lambda) const
{
    return fRefIndex; 
}  
//----------------------------------------------------------------------
double PndDrcOptMatConstIndex::RefIndexDeriv(const double lambda) const
{
 return 0;
}
//----------------------------------------------------------------------
bool PndDrcOptMatConstIndex::AbsorptionFlag(double lambda, double length) const
{
  return false; // no absorption.
}
