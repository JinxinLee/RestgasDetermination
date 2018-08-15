//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class PndEmcRecoHit
//
// Environment:
//	Software developed for the PANDA Detector at GSI.
//
//------------------------------------------------------------------------


//-----------------------
// This Class's Header --
//-----------------------

#include "PndEmcRecoHit.h"

//---------------
// C++ Headers --
//---------------

#include <iostream>
#include <iomanip>
#include <vector>
using std::vector;
using std::endl;
using std::ios;
using std::ostream;
using std::setw;

//----------------
// Constructors --
//----------------

PndEmcRecoHit::PndEmcRecoHit():
fEnergy(0), fEnergyCorrected(0), fPosition(0,0,0)
{
}

PndEmcRecoHit::PndEmcRecoHit(Double_t energy, TVector3 position):
	fEnergy(energy),
	fEnergyCorrected(energy),
	fPosition(position)
{
}

PndEmcRecoHit::~PndEmcRecoHit()
{
}

//-------------
// Modifiers --
//-------------

Double_t 
PndEmcRecoHit::GetEnergy() const
{
  return fEnergy;
}

Double_t 
PndEmcRecoHit::GetEnergyCorrected() const
{
  return fEnergyCorrected;
}

TVector3 
PndEmcRecoHit::GetPosition() const
{
  return fPosition;
}


ClassImp(PndEmcRecoHit)
