//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Track candidate. Basically a list of hit indices.
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

#ifndef TRACKCAND_HH
#define TRACKCAND_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <vector>

// Collaborating Class Declarations --



class TrackCand : public TObject {
public:

  // Constructors/Destructors ---------
  TrackCand();
  ~TrackCand();

  TrackCand(double curv, double dip, double inv, std::vector<unsigned int> detIDs, std::vector<unsigned int> hitIDs);

  // operators 
  friend bool operator== (const TrackCand& lhs, const TrackCand& rhs);

  // Accessors -----------------------
  void getHit(unsigned int i, 
	      unsigned int& detId,
	      unsigned int& hitId) const {detId=_detId[i];hitId=_hitId[i];}
  unsigned int getNHits() const {return _detId.size();}
  double getCurv() const {return _curv;}
  double getDip() const {return _dip;}
  bool inverted() const {return _inv;}
  std::vector<unsigned int> GetHitIDs(int detId=-1);

  // Modifiers -----------------------
  void addHit(unsigned int detId, unsigned int hitId);
  void setCurv(double c){_curv=c;}
  void setDip(double d){_dip=d;}
  void setInverted(bool f=true) {_inv=f;}

  // Operations ----------------------
  void reset();

private:

  // Private Data Members ------------
  std::vector<unsigned int> _detId;
  std::vector<unsigned int> _hitId;

  double _curv; // curvature from pattern reco
  double _dip;  // dip angle from pattern reco
  bool _inv;  // true if inverted track
  // Private Methods -----------------

public:
  ClassDef(TrackCand,2)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
