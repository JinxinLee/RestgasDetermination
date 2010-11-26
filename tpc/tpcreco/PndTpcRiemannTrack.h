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
#include "TVectorD.h"
#include "McIdCollection.h"

// Collaborating Class Declarations --
class TVector3;
class PndTpcRiemannHit;


typedef std::list<PndTpcRiemannHit*> hitList;
typedef hitList::iterator hitIt;

class PndTpcRiemannTrack : public TObject{
public:

  // Constructors/Destructors ---------
  PndTpcRiemannTrack();
  ~PndTpcRiemannTrack(){;}

  
  // Accessors -----------------------
  const TVectorD& n() const {return _n;}
  double c() const {return _c;}
  const TVectorD& av() const {return _av;}

  TVectorD orig() const;
  double r() const;
  double dip() const;
  double sign() const;
  int winding() ;
  unsigned int getNumHits() const {return _hits.size();}
  bool isFitted()const {return _isFitted;}
  PndTpcRiemannHit* getHit(unsigned int i) const;
  PndTpcRiemannHit* getLastHit() const;
  PndTpcRiemannHit* getFirstHit() const {return _hits.front();}

  double m() const {return _m;}
  double t() const {return _t;}

  const McIdCollection& mcid() const {return _mcid;}

  // Modifiers -----------------------
  void addHit(PndTpcRiemannHit* hit);


  hitIt sortHit(PndTpcRiemannHit* hitX, 
	       hitList& hL, int dir);

  void insertHit(hitList& hL,PndTpcRiemannHit* hitX,const hitIt& it){hL.insert(it,hitX);} // manually insert hit

  // get closest hit in track fill distance and direction of track there
  hitIt getClosestHit(PndTpcRiemannHit* hit, double& Dist, TVector3& dir);
  hitIt getClosestHit(PndTpcRiemannHit* hit, double& Dist);

  void setSort(bool k=true){_doSort=k;}
  void resetNit(){_nit=0;}

  void init(double x0, double y0, double R, 
	    double Dip, double z0);
  void setZ0(double z){_t=z;}

  // Operations ----------------------
  void refit();
  double dist(PndTpcRiemannHit* hit);
  void szFit(bool print=false);
  double szDist(PndTpcRiemannHit* hit, bool calcPos=true);
  void trackpos(); // calculate positions along track
  void Plot(bool standalone=true);

private:

  // Private Data Members ------------
  TVectorD _n;  // normal vector to plane;
  double _c;     // distance of plane to origin

  double _m; //parameters of sz-fit
  double _t;

  bool _isFitted;

  std::list<PndTpcRiemannHit*> _hits; //!
  TVectorD _av;  // average over all hits

  int _nit; //iteration counter for sorting
  bool _doSort; // flag for switching on and off sorting

  McIdCollection _mcid;

  // Private Methods -----------------
 

public:
  ClassDef(PndTpcRiemannTrack,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
