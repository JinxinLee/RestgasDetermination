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

  unsigned int getLaserID() {return flaserID;}
  unsigned int getClusterID() {return fclusterID;}
  const TVector3& getRes() {return fres;}
  const TVector3& getPos() {return fpos;}
  const TVector3& getProjection() {return fpP;}
  const TVector3& getResSig() {return fsig;}      //uncertainty of the reconst. residual 
  const TVector3& getProjectionSig() {return fsigpP;} //uncertainty of the proj. point 
  
  PndTpcCluster* getCluster() {return fclust;}

// Modifiers ------------------------------------------------

 
 private:

  unsigned int fclusterID; 
  unsigned int flaserID;
  TVector3 fpos;
  //double fposX, fposY, fposZ;
  TVector3 fres;
  TVector3 fpP;   //Projection Point on the Track
  TVector3 fsig;  //uncertainties of the rec. residual
  TVector3 fsigpP;  //uncertainties of the rec. proj. point
  PndTpcCluster* fclust; //!

public:
  ClassDef(PndTpcLaserStat,1)

};

#endif
