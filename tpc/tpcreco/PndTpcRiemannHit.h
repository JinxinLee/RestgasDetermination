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
  PndTpcRiemannHit():_cluster(NULL),_s(0.),_angleOnHelix(0.),fRiemannScale(24.6){}
  PndTpcRiemannHit(double scale):_cluster(NULL),_s(0.),_angleOnHelix(0.),fRiemannScale(scale){}
  PndTpcRiemannHit(double r,double phi, double scale=24.6);
  PndTpcRiemannHit(PndTpcCluster* cl, double scale=24.6);
  ~PndTpcRiemannHit();

  
  // Accessors -----------------------
  const TVector3& x() const {return _x;}
  PndTpcCluster* cluster() const {return _cluster;}
  double s() const {return _s;}
  double getAngleOnHelix() const {return _angleOnHelix;}
  double z() const;
  double getScale() const {return fRiemannScale;}

  // Modifiers -----------------------
  double setPosOnTrk(double S){_s=S;}
  double setAngleOnHelix(double ang){_angleOnHelix=ang;}

  // Operations ----------------------

private:

  // Private Data Members ------------
  TVector3 _x; // Position on Riemann sphere in cartesian coordinates
  PndTpcCluster* _cluster; //->  //no ownership over this pointer!
  double _s;     // pathlength along track
  double _angleOnHelix;   // angle on helix
  double fRiemannScale; //scaling in xy plane: 8.7 for prototype, 24.6 for panda

public:
  ClassDef(PndTpcRiemannHit,3)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
