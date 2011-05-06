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

  TVectorD orig() const;
  double r() const; // radius of track
  void r(double &r1, double &r2) const; // radius of track
  TVector3 center() const; // center of helix in xz plane
  double dip() const;
  double sign() const;
  int winding() const;
  unsigned int getNumHits() const {return _hits.size();}
  bool isFitted()const {return _isFitted;}
  bool isFittedPlane()const {return _isFittedPlane;}
  PndTpcRiemannHit* getHit(unsigned int i) const {return _hits[i];}
  PndTpcRiemannHit* getLastHit() const {return _hits.back();}
  PndTpcRiemannHit* getFirstHit() const {return _hits.front();}

  double m() const {return _m;}
  double t() const {return _t;}

  const McIdCollection& mcid() const {return _mcid;}

  // Modifiers -----------------------
  void addHit(PndTpcRiemannHit* hit);
  void removeHit(unsigned int ihit);
  int sortHit(PndTpcRiemannHit* hitX);

  //void insertHit(PndTpcRiemannHit* hitX,const int it){hL.insert(it,hitX);} // manually insert hit

  // get closest hit in track fill distance and direction of track there
  int getClosestHit(PndTpcRiemannHit* hit, double& Dist, TVector3& dir);
  int getClosestHit(PndTpcRiemannHit* hit, double& Dist, int from=0,  int to=10000000); // optional: range where to search for closest hit
  int getClosestRiemannHit(PndTpcRiemannHit* hit, double& Dist);
  
  void setSort(bool k=true){_doSort=k;}
  void resetNit(){_nit=0;}

  void init(double x0, double y0, double R, 
	    double Dip, double z0);
  void setZ0(double z){_t=z;}

  // Operations ----------------------
  void refit(); // refit the plane
  
  // calculate RMS of distances of hits to intersection of plane with riemann sphere
  // if use Arguments == false, the members _n and _c will be used for calculation
  double planeRMS(TVector3 n1=(0.,0.,0.), double c1=0, bool useArguments=false);
                                                                    
  // calculate distance of hit to intersection of plane with riemann sphere
  // if use Arguments == false, the members _n and _c will be used for calculation
  double dist(PndTpcRiemannHit* hit, TVector3 n2=(0.,0.,0.), double c2=0, bool useArguments=false);
   
  void szFit(bool print=false);
  double szDist(PndTpcRiemannHit* hit, bool calcPos=true);
  void trackpos(); // calculate positions along track
  void Plot(bool standalone=true);

 private:

  // Private Data Members ------------
  TVector3 _n;  // normal vector of plane (pointing towards origin!)
  double _c;     // distance of plane to origin

  double _m; //parameters of sz-fit
  double _t;

  bool _isFitted; // szFit
  bool _isFittedPlane; // fit _n and _c

  double fRiemannScale;

  std::vector<PndTpcRiemannHit*> _hits; //
  TVector3 _av;  // average over all hits
  double _sumOfWeights; // for weighing the average with cluster error

  int _nit; //iteration counter for sorting
  bool _doSort; // flag for switching on and off sorting

  McIdCollection _mcid;

  // Private Methods -----------------
 
  bool checkScale(PndTpcRiemannHit*);


 public:
  ClassDef(PndTpcRiemannTrack,2)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
