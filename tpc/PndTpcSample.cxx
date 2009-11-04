//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcSample
//      see PndTpcSample.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Xiaodong Zhang       TUM       (modify it on Oct. 06, 2009)
//
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndTpcSample.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------


// Class Member definitions -----------

ClassImp(PndTpcSample)


PndTpcSample::PndTpcSample()
: famp(0),ft(0),fpadId(0),felectronicChId(-1)
{;}

PndTpcSample::PndTpcSample( const int T, 
		      const int Amp, 
		      const unsigned int PadID,
		      const McIdCollection& mcid)
  : famp(Amp),ft(T),fpadId(PadID),fmcId(mcid),felectronicChId(-1)
{;}

PndTpcSample::PndTpcSample( const int T,
                      const int Amp,
                      const unsigned int PadID,
                      const unsigned int ElectronicChId)
  : famp(Amp),ft(T),fpadId(PadID),felectronicChId(ElectronicChId) 
{;}

PndTpcSample::~PndTpcSample()
{
}


bool 
operator== (const PndTpcSample& lhs, const PndTpcSample& rhs){
  return lhs.ft==rhs.ft && lhs.famp==rhs.famp && lhs.fpadId==rhs.fpadId
    && lhs.felectronicChId==rhs.felectronicChId;
}

bool 
operator< (const PndTpcSample& lhs, const PndTpcSample& rhs){
  return lhs.ft<rhs.ft;
}

std::ostream& operator<< (std::ostream& s, const PndTpcSample& me){
  s << "PndTpcSample:\n"
    << "   t(Sample)     ="<<me.ft<<"\n"
    << "   Amp           ="<<me.famp<<"\n"
    << "   PadID         ="<<me.fpadId<<"\n"
    << "   channelId     ="<<me.fchannelId<<"\n"
    << "   chipId        ="<<me.fchipId<<"\n"
    << "   adcId         ="<<me.fadcId<<"\n"
    << "   sourceId      ="<<me.fsourceId<<"\n"
    << "   ElectronicChId="<<me.felectronicChId<<"\n";
  return s;
}

void PndTpcSample::reset(){
  
  ft = 0;
  famp = 0;
  fchannelId = 0;
  fchipId = 0;
  //fadcId = 0;
  //fsourceId = 0;

}
