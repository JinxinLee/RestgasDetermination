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

#ifndef TPCXYRECOHIT_HH
#define TPCXYRECOHIT_HH

// Base Class Headers ----------------
#include "AbsRecoHit.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class PndTpcCluster;


class PndTpcXYRecoHit : public AbsRecoHit {
public:

  // Constructors/Destructors ---------
  PndTpcXYRecoHit();
  PndTpcXYRecoHit(double x, double y, double z,
	       double sigx, double sigy);
  PndTpcXYRecoHit(const TVector3& pos,
	       double sigx, double sigy);
  PndTpcXYRecoHit(PndTpcCluster* cl);

  virtual ~PndTpcXYRecoHit();

  virtual AbsRecoHit* clone();
  

  // Operators
  PndTpcXYRecoHit& operator=(const PndTpcXYRecoHit&);
  friend bool operator== (const PndTpcXYRecoHit& lhs, const PndTpcXYRecoHit& rhs);
  friend bool operator< (const PndTpcXYRecoHit& lhs, const PndTpcXYRecoHit& rhs);
  friend std::ostream& operator<< (std::ostream& s, const PndTpcXYRecoHit& me);

  // Accessors -----------------------


  // Modifiers -----------------------


  // Operations ----------------------
  virtual void setHMatrix(double s, const AbsTrackRep* stateVector,
                          const TMatrixT<double>& state);


  virtual double residualScalar(double s,
                                const AbsTrackRep* stateVector,
                                const TMatrixT<double>& state);

  

private:

  // Private Data Members ------------
  static const int NparHitRep = 2;

  // Private Methods -----------------

public:
  ClassDef(PndTpcXYRecoHit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
