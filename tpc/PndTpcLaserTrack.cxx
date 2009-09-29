//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//    Implementation of a Laser-Track in the Tpc
//     
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//	Felix Boehmer    	TUM     (original author)
//
//-----------------------------------------------------------


// This Class' Header ------------------
#include "PndTpcLaserTrack.h"

#include "PndTpcLaserStat.h"
#include "PndTpcCluster.h"


// Class Member definitions -----------

ClassImp(PndTpcLaserTrack)

PndTpcLaserTrack::PndTpcLaserTrack(unsigned int ID, const TVector3& start,
		                   const TVector3& dir) :

  fstart(start),
  fdir(dir),
  fID(ID)                 // TRACK - ID
{
  fstatList = new std::vector<PndTpcLaserStat*>;
}


PndTpcLaserTrack::~PndTpcLaserTrack()
{;}


void
PndTpcLaserTrack::addStat(PndTpcLaserStat* stat)
{
  fstatList->push_back(stat);
}


std::vector<PndTpcCluster*>*
PndTpcLaserTrack::getClusterList()
{
  std::vector<PndTpcCluster*>* c_list = new std::vector<PndTpcCluster*>;
  unsigned int NStats = fstatList->size();
  for(unsigned int i=0;i<NStats;i++)
  {
    c_list->push_back(fstatList->at(i)->getCluster());
  }
  return c_list;
}

