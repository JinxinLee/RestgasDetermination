//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//    Laser-Track in the PndTpc, built by PndTpcLaserFitTask
//     
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//	Felix Boehmer    	TUM     (original author)
//
//-----------------------------------------------------------

#ifndef TPCLASERSTAT_H
#define TPCLASERSTAT_H


// External Headers -----------------------------------------
#include <vector> 
#include "TVector3.h"
#include "TObject.h"
#include "PndTpcCluster.h"


class PndTpcLaserStat : public TObject {

 public:

// Constructors ---------------------------------------------

  PndTpcLaserStat();
  PndTpcLaserStat(unsigned int, PndTpcCluster*, const TVector3& res, const TVector3& sigres, 
		  				    const TVector3& pP, const TVector3& sig_pP);
  ~PndTpcLaserStat();


// Acessors -------------------------------------------------

  unsigned int getLaserID() {return _laserID;}
  unsigned int getClusterID() {return _clusterID;}
  const TVector3& getRes() {return _res;}
  const TVector3& getPos() {return _pos;}
  const TVector3& getProjection() {return _pP;}
  const TVector3& getResSig() {return _sig;}      //uncertainty of the reconst. residual 
  const TVector3& getProjectionSig() {return _sigpP;} //uncertainty of the proj. point 
  
  PndTpcCluster* getCluster() {return _clust;}

// Modifiers ------------------------------------------------

 
 private:

  unsigned int _clusterID; 
  unsigned int _laserID;
  TVector3 _pos;
  //double _posX, _posY, _posZ;
  TVector3 _res;
  TVector3 _pP;   //Projection Point on the Track
  TVector3 _sig;  //uncertainties of the rec. residual
  TVector3 _sigpP;  //uncertainties of the rec. proj. point
  PndTpcCluster* _clust; //!

public:
  ClassDef(PndTpcLaserStat,1)

};

#endif
