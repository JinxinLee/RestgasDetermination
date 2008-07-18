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

  _start(start),
  _dir(dir),
  _ID(ID)                 // TRACK - ID
{
  _statList = new std::vector<PndTpcLaserStat*>;
}


PndTpcLaserTrack::~PndTpcLaserTrack()
{;}


void
PndTpcLaserTrack::addStat(PndTpcLaserStat* stat)
{
  _statList->push_back(stat);
}


std::vector<PndTpcCluster*>*
PndTpcLaserTrack::getClusterList()
{
  std::vector<PndTpcCluster*>* c_list = new std::vector<PndTpcCluster*>;
  unsigned int NStats = _statList->size();
  for(unsigned int i=0;i<NStats;i++)
  {
    c_list->push_back(_statList->at(i)->getCluster());
  }
  return c_list;
}

