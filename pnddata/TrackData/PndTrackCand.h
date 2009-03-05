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

// Root Class Headers ----------------
#include "TObject.h"
#include "TArrayI.h"

// Collaborating Class Headers -------

// Collaborating Class Declarations --



class PndTrackCand : public TObject {
public:

  // Constructors/Destructors ---------
  PndTrackCand();
  ~PndTrackCand();

  // operators
  friend bool operator== (const PndTrackCand& lhs, const PndTrackCand& rhs);

  // Accessors -----------------------
  void GetHit(unsigned int i,
	      unsigned int& detId,
	      unsigned int& hitId) const {detId=fDetId[i];hitId=fHitId[i];}
  unsigned int GetNHits() const {return fDetId.GetSize();}
  int GetDetId(unsigned int pos) const {return fDetId[pos];}
  int GetHitId(unsigned int pos) const {return fHitId[pos];}

  TArrayI GetHitIDs(int detId=-1);

  // Modifiers -----------------------
  void AddHit(unsigned int detId, unsigned int hitId);

  bool HitInTrack(unsigned int detId, unsigned int hitId);

  void Reset();


private:

  // Private Data Members ------------
  TArrayI fDetId;
  TArrayI fHitId;

public:
  ClassDef(PndTrackCand,1)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
