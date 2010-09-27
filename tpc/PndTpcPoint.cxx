//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPoint
//      see PndTpcPoint.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcPoint.h"
#include "PndDetectorList.h"

// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------


// Class Member definitions -----------

PndTpcPoint::PndTpcPoint()
  : FairMCPoint()
{}

PndTpcPoint::PndTpcPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
			 Double_t tof, Double_t length, Double_t eLoss)
  : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss)
{
  //SetLink(FairLink("MCTrack", trackID));
}

void 
PndTpcPoint::Print(const Option_t*) const {
  std::cout<<"PndTpcPoint\n"
	   <<" Pos("<<fX<<","<<fY<<","<<fZ<<")\n"
	   <<" dE="<<fELoss
	   <<std::endl;	   
}



ClassImp(PndTpcPoint)
