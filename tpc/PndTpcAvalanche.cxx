//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcAvalanche
//      see PndTpcAvalanche.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Cristoforo Simonetto TUM
//
//
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndTpcAvalanche.h"

// C/C++ Headers ----------------------
#include <cmath>

// Collaborating Class Headers --------
//#include "CLHEP/GenericFunctions/Gaussian.hh"
#include "PndTpcDriftedElectron.h"


ClassImp(PndTpcAvalanche)

// Class Member definitions -----------

PndTpcAvalanche::PndTpcAvalanche()
  : fx(0),fy(0),ft(0),famp(0),
    fmother(0)
{;}

PndTpcAvalanche::PndTpcAvalanche(const double X, 
			   const double Y,
			   const double T,
			   const double Amp,
			   PndTpcDriftedElectron* tpcDElectronp)
  : fx(X),fy(Y),ft(T),famp(Amp),
    fmother(tpcDElectronp)
{;}


unsigned int 
PndTpcAvalanche::mcTrackId() const {
  if (fmother==0) return 0;
  return fmother->mcTrackId();
}

unsigned int
PndTpcAvalanche::mcSecId() const {
  if (fmother==0) return 0;
  return fmother->mcSecId();
}

unsigned int
PndTpcAvalanche::mcHitId() const {
  if (fmother==0) return 0;
  return fmother->mcHitId();
}


bool operator== (const PndTpcAvalanche& lhs,const PndTpcAvalanche& rhs)
{
  double dx=lhs.fx - rhs.fx;
  double dy=lhs.fy - rhs.fy;
  double dt=lhs.ft - rhs.ft;
  double d2=dx*dx+dy*dy;
  return (d2<1E-10) && (fabs(dt)<1E-4);
}

std::ostream& operator<< (std::ostream& s, const PndTpcAvalanche& me){
  s << "PndTpcAvalanche:\n"
    << "   t    ="<<me.ft<<"\n"
    << "   x    ="<<me.fx<<"\n"
    << "   y    ="<<me.fy<<"\n"
    << "   Amp  ="<<me.famp<<"\n";
  return s;
}
