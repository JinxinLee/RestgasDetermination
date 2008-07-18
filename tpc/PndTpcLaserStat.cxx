//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//    Basic statistics container for the Lasertracks
//     
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//	Felix Boehmer    	TUM     (original author)
//
//-----------------------------------------------------------

#include "PndTpcLaserStat.h"
#include "PndTpcCluster.h"

#include "assert.h"

// Class Member definitions -----------

ClassImp(PndTpcLaserStat)

PndTpcLaserStat::PndTpcLaserStat() 
  :
  _laserID(0),
  _clusterID(0),
  _res(TVector3(0,0,0)),
  //_resX(_res.X()),
  //_resY(_res.Y()), 
  //_resZ(_res.Z()),
  _clust(NULL),
  _pos(TVector3(0,0,0)),
  //_posX(_pos.X()),
  //_posY(_pos.Y()),
  //_posZ(_pos.Z()),
  _pP(TVector3(0,0,0))
{;}
  
PndTpcLaserStat::PndTpcLaserStat(unsigned int laserID, PndTpcCluster* clust,
				 const TVector3& residual, const TVector3& res_sig, const TVector3& pP, 
				 const TVector3& sigpP)
  :
  _laserID(laserID),
  _res(residual),
  _pP(pP),
  _sig(res_sig),
  _sigpP(sigpP)
{
  assert(clust);
  _clust=clust;
  _clusterID=_clust->index();
  _pos=_clust->pos();
}

PndTpcLaserStat::~PndTpcLaserStat()
{;}

