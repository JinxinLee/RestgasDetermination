//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      an xyz reco hit (z being considered as a free parameter)
//      hitCoord=(x,y)^T
//      s=z
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCPLANARRECOHIT_HH
#define TPCPLANARRECOHIT_HH

// Base Class Headers ----------------
#include "RecoHitIfc.h"
#include "PlanarHitPolicy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class PndTpcCluster;

typedef RecoHitIfc<PlanarHitPolicy> PlanarRecoHit;

class PndLheTpcPlanarRecoHit : public PlanarRecoHit {
public:

  // Constructors/Destructors ---------
  PndLheTpcPlanarRecoHit();
  PndLheTpcPlanarRecoHit(double x, double y, double z,
	       double sigx, double sigy);
  PndLheTpcPlanarRecoHit(const TVector3& pos,
	       double sigx, double sigy);
  PndLheTpcPlanarRecoHit(PndTpcCluster* cl);

  virtual ~PndLheTpcPlanarRecoHit();

  virtual AbsRecoHit* clone();
  
  // Operations ----------------------
  virtual void setHMatrix(const AbsTrackRep* stateVector,
                          const TMatrixT<double>& state);


  virtual double residualScalar(AbsTrackRep* stateVector,
                                const TMatrixT<double>& state);

  // accessors
  unsigned int cluster_size() const {return _cluster_size;}
  unsigned int cluster_amp() const {return _cluster_amp;}
 
private:

  // Private Data Members ------------
  static const int NparHitRep = 2;

  unsigned int _cluster_size;
  unsigned int _cluster_amp;

  // Private Methods -----------------

public:
  ClassDef(PndLheTpcPlanarRecoHit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
