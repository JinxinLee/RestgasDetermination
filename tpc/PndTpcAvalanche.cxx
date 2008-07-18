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
  : _x(0),_y(0),_t(0),_amp(0),
    _mother(0)
{;}

PndTpcAvalanche::PndTpcAvalanche(const double x, 
			   const double y,
			   const double t,
			   const double Amp,
			   PndTpcDriftedElectron* tpcDElectronp)
  : _x(x),_y(y),_t(t),_amp(Amp),
    _mother(tpcDElectronp)
{;}


unsigned int 
PndTpcAvalanche::mcTrackId() const {
  if (_mother==0) return 0;
  return _mother->mcTrackId();
}

unsigned int
PndTpcAvalanche::mcHitId() const {
  if (_mother==0) return 0;
  return _mother->mcHitId();
}


bool operator== (const PndTpcAvalanche& lhs,const PndTpcAvalanche& rhs)
{
  double dx=lhs._x - rhs._x;
  double dy=lhs._y - rhs._y;
  double dt=lhs._t - rhs._t;
  double d2=dx*dx+dy*dy;
  return (d2<1E-10) && (fabs(dt)<1E-4);
}

std::ostream& operator<< (std::ostream& s, const PndTpcAvalanche& me){
  s << "PndTpcAvalanche:\n"
    << "   t    ="<<me._t<<"\n"
    << "   x    ="<<me._x<<"\n"
    << "   y    ="<<me._y<<"\n"
    << "   Amp  ="<<me._amp<<"\n";
  return s;
}
