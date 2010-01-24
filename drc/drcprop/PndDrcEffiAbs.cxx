// ----------------------------------------------------
// This file belongs to the ray tracing framework
// for the use with Cherenkov detectors
//
// created 2007
//-----------------------------------------------------
#include "PndDrcEffiAbs.h"

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
PndDrcEffiAbs::PndDrcEffiAbs()
{
  fName        = "unnamed PndDrcEffiAbs";
  fVerbosity   = 0;
}
//----------------------------------------------------------------------
PndDrcEffiAbs::~PndDrcEffiAbs()
{
}
//----------------------------------------------------------------------
PndDrcEffiAbs::PndDrcEffiAbs(const PndDrcEffiAbs& effi)
{
  fName      = effi.fName;
  fVerbosity = effi.fVerbosity;
}
//----------------------------------------------------------------------
PndDrcEffiAbs& PndDrcEffiAbs::operator=(const PndDrcEffiAbs& effi)
{
  fName      = effi.fName;
  fVerbosity = effi.fVerbosity;

  return *this;
}
