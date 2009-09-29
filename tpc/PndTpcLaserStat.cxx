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
  flaserID(0),
  fclusterID(0),
  fres(TVector3(0,0,0)),
  //fresX(fres.X()),
  //fresY(fres.Y()), 
  //fresZ(fres.Z()),
  fclust(NULL),
  fpos(TVector3(0,0,0)),
  //fposX(fpos.X()),
  //fposY(fpos.Y()),
  //fposZ(fpos.Z()),
  fpP(TVector3(0,0,0))
{;}
  
PndTpcLaserStat::PndTpcLaserStat(unsigned int laserID, PndTpcCluster* clust,
				 const TVector3& residual, const TVector3& res_sig, const TVector3& pP, 
				 const TVector3& sigpP)
  :
  flaserID(laserID),
  fres(residual),
  fpP(pP),
  fsig(res_sig),
  fsigpP(sigpP)
{
  assert(clust);
  fclust=clust;
  fclusterID=fclust->index();
  fpos=fclust->pos();
}

PndTpcLaserStat::~PndTpcLaserStat()
{;}

