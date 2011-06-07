//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Track on Riemann Sphere
//      Circle parameters can be calculated from plane parameters
//      plane(c,nx,ny,nz);
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

#ifndef TPCRIEMANNTRACK_HH
#define TPCRIEMANNTRACK_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <list>
#include "TVector3.h"

#include "McIdCollection.h"

// Collaborating Class Declarations --
class TVector3;
class PndTpcRiemannHit;


class PndTpcRiemannTrack : public TObject{
 public:

  // Constructors/Destructors ---------
  PndTpcRiemannTrack();
  PndTpcRiemannTrack(double scale);
  ~PndTpcRiemannTrack(){;}

  
  // Accessors -----------------------
  const TVector3& n() const {return _n;}
  double c() const {return _c;}
  const TVector3& av() const {return _av;}
  double getScale() const {return fRiemannScale;}

  double r() const {return _radius;} // radius of track
  TVector3 center() const {return _center;} // center of helix in xz plane (z=0)
  double dip() const {return _dip;} // dip angle vs z axis [0, pi]
  double sinDip() const {return _sinDip;} // dip angle vs z axis [0, pi]

  bool isFitted() const {return _isFitted;}
  double m() const {return _m;}
  double t() const {return _t;}


  unsigned int getNumHits() const {return _hits.size();}
  PndTpcRiemannHit* getHit(unsigned int i) const {return _hits[i];}
  PndTpcRiemannHit* getLastHit() const {return _hits.back();}
  PndTpcRiemannHit* getFirstHit() const {return _hits.front();}

  int getClosestHit(PndTpcRiemannHit* hit, double& Dist, TVector3& dir) const;
  int getClosestHit(PndTpcRiemannHit* hit, double& Dist, int from=0,  int to=10000000) const; // optional: range where to search for closest hit
  int getClosestRiemannHit(PndTpcRiemannHit* hit, double& Dist) const;

  // these functions can be used to calculate seed values for the fitter
  void getPosDirOnHelix(unsigned int i, TVector3& pos, TVector3& dir) const; // get position (3D) of point i on the fitted helix
  double getMom(double Bz) const; // get the magnitude of the momentum
  int winding() const; // +- 1; winding sense along z axis

  const McIdCollection& mcid() const {return _mcid;}

  // Modifiers -----------------------
  void addHit(PndTpcRiemannHit* hit);
  void removeHit(unsigned int ihit);

  void setSort(bool k=true){_doSort=k;}

  void init(double x0, double y0, double R, 
	    double Dip, double z0);

  // Operations ----------------------
  void fitAndSort();  // refit the plane and sort the hits; calculate center and radius
  
  // calculate RMS of distances of hits to intersection of plane with riemann sphere
  // if use Arguments == false, the members _n and _c will be used for calculation
  double planeRMS(TVector3 n1=(0.,0.,0.), double c1=0, bool useArguments=false) const;
                                                                    
  // calculate distance of hit to intersection of plane with riemann sphere
  // if use Arguments == false, the members _n and _c will be used for calculation
  double dist(PndTpcRiemannHit* hit, TVector3 n2=(0.,0.,0.), double c2=0, bool useArguments=false) const;
   
  double distHelix(PndTpcRiemannHit* hit, bool calcPos=true) const; // distance to helix
  void Plot(bool standalone=true);

 private:
  // Operations ----------------------
  void refit(); // refit the plane and dip; set angles of hits
  void centerR(); // calculate center and radius

  // Private Data Members ------------
  TVector3 _n;  // normal vector of plane (pointing towards origin!)
  double _c;     // distance of plane to origin

  TVector3 _center; // center of helix in 3D (z=0)
  double _radius;   // radius of helix

  double _m; //parameters of Helix sz-fit: z = _m*phi + _t
  double _t; //parameters of Helix sz-fit: z = _m*phi + _t

  double _dip; // dip angle of track [0, pi]
  double _sinDip; // sinus of dip angle

  bool _isFitted; // fit plane and dip

  double fRiemannScale;

  std::vector<PndTpcRiemannHit*> _hits; //
  TVector3 _av;  // average over all hits
  double _sumOfWeights; // for weighing the average with cluster error

  bool _doSort; // flag for switching on and off sorting

  McIdCollection _mcid;

  // Private Methods -----------------
  bool checkScale(PndTpcRiemannHit*) const;

 public:
  ClassDef(PndTpcRiemannTrack,3)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
