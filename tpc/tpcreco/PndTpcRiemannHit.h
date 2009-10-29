//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Conformal Mapping of a PndTpc Cluster (x,y)->(r,phi)-> riemann sphere
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

#ifndef TPCRIEMANNHIT_HH
#define TPCRIEMANNHIT_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "TVector3.h"

// Collaborating Class Declarations --
class PndTpcCluster;
class PndTpcRiemannTrack;

class PndTpcRiemannHit : public TObject{
public:

  // Constructors/Destructors ---------
  PndTpcRiemannHit():_cluster(NULL),_s(0.),_alpha(0.){}
  PndTpcRiemannHit(double r,double phi);
  PndTpcRiemannHit(PndTpcCluster* cl);
  ~PndTpcRiemannHit();

  
  // Accessors -----------------------
  const TVector3& x() const {return _x;}
  PndTpcCluster* cluster() const {return _cluster;}
  double s() const {return _s;}
  double z() const;
  double alpha() const {return _alpha;}
  

  // Modifiers -----------------------


  // Operations ----------------------
  // recursively calculate s:
  double calcPosOnTrk(PndTpcRiemannHit* hit, bool refine=true);
  double setPosOnTrk(double S){_s=S;}

private:

  // Private Data Members ------------
  TVector3 _x;
  PndTpcCluster* _cluster; //->  //no ownership over this pointer!
  double _s;     // pathlength along track
  double _alpha; // angle along track

  // Private Methods -----------------

public:
  ClassDef(PndTpcRiemannHit,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
