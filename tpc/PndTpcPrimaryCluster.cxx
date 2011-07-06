//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPrimaryCluster
//      see PndTpcPrimaryCluster.hh for details
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
#include "PndTpcPrimaryCluster.h"
#include "PndDetectorList.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------


// Class Member definitions -----------


ClassImp(PndTpcPrimaryCluster);

PndTpcPrimaryCluster::PndTpcPrimaryCluster()
  : ft(0), fq(0), fmcTrackId(0),fmcHitId(0),fpos(0,0,0)
{}

PndTpcPrimaryCluster::PndTpcPrimaryCluster(const double T, 
				     const int Q, 
				     const TVector3& Pos,
				     const unsigned int McTrackId,
				     const unsigned int McHitId,
				     const unsigned int McSecId)
  : ft(T), fq(Q), fmcTrackId(McTrackId),fmcHitId(McHitId),fpos(Pos), fmcSecId(McSecId)
{
	SetLink(FairLink("PndTpcPoint", McHitId));
}


PndTpcPrimaryCluster::~PndTpcPrimaryCluster()
{}
