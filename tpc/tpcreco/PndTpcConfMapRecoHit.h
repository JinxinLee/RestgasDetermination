//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      RecoHit using conformal mapping in the tpc
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCCONFMAPRECOHIT_HH
#define TPCCONFMAPRECOHIT_HH

// Base Class Headers ----------------
#include "RecoHitIfc.h"
#include "PlanarHitPolicy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "PndTpcCluster.h"

// Collaborating Class Declarations --



class PndTpcConfMapRecoHit : public RecoHitIfc<PlanarHitPolicy> {
public:

  // Constructors/Destructors ---------
  PndTpcConfMapRecoHit();
  PndTpcConfMapRecoHit(double xp, double yp); //set conformal coordinates directly
  PndTpcConfMapRecoHit(PndTpcCluster* cl);
  virtual ~PndTpcConfMapRecoHit(){;}
  virtual AbsRecoHit* clone();

  

  // Accessors -----------------------
  double getXcf() const {return _xcf;}
  double z() const {return _clusterz;}
  double x() const {return _clusterx;}
  double y() const {return _clustery;}
  double s() const {return _s;}
  double calc_s(double R); // sets _s
  unsigned int index() const {return _clusterindex;}

  // Modifiers -----------------------
  void setReferencePoint(double xref, double yref); // sets reference point for conformal map
                            // default is (0,0,0);

  void setRotated(bool flag=true){_isRotated=flag;reMap();}

  // Operations ----------------------
  virtual void setHMatrix(const AbsTrackRep* stateVector,
                          const TMatrixT<double>& state);

  // overwrite residualVector since we will use this RecoHit 
  // in two independent fits! Tricky!
  virtual TMatrixT<double> residualVector(const AbsTrackRep* stateVector,
                                          const TMatrixT<double>& state);
  


private:

  // Private Data Members ------------
  double _refx; // conformal map reference point;
  double _refy;

  double _clusterx; // original coordinates of cluster
  double _clustery;
  double _clusterz;

  PndTpcCluster* _cl;

  unsigned int _clusterindex;

  double _xcf;
  double _s;

  bool _isRotated;

  // Private Methods -----------------
  void reMap();

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
