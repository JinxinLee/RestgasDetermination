// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcOptMatAbs.h"

//#include "TObject.h"
//#include "TVector3.h"
//
//#include <iostream>
//using std::cout;
//using std::cerr;
//using std::cin;
//using std::endl;
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
PndDrcOptMatAbs::PndDrcOptMatAbs()
{
  fName        = "unnamed PndDrcOptMatAbs";
  fVerbosity   = 0;
}
//----------------------------------------------------------------------
PndDrcOptMatAbs::~PndDrcOptMatAbs()
{
}
//----------------------------------------------------------------------
PndDrcOptMatAbs::PndDrcOptMatAbs(const PndDrcOptMatAbs& mat)
{
  fName      = mat.fName;
  fVerbosity = mat.fVerbosity;
}
//----------------------------------------------------------------------
PndDrcOptMatAbs& PndDrcOptMatAbs::operator=(const PndDrcOptMatAbs& mat)
{
  fName      = mat.fName;
  fVerbosity = mat.fVerbosity;

  return *this;
}
