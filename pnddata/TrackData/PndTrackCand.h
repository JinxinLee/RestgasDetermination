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

#include <map>

typedef std::map<Double_t, std::pair<Int_t, Int_t> >::const_iterator mapIter;

class PndTrackCand : public TObject {
public:

  // Constructors/Destructors ---------
  PndTrackCand();
  ~PndTrackCand();

  // operators
  friend bool operator== (const PndTrackCand& lhs, const PndTrackCand& rhs);

  // Accessors -----------------------
  std::pair<Int_t, Int_t> GetHit(unsigned int i) const {
	  if (i < fHitId.size()){
		  mapIter iter = fHitId.begin();
		  for (int j = 0; j < i; j++) iter++;
		  return (iter->second);
	  }
	  else return std::pair<Int_t, Int_t>(-1, -1);
  }
  unsigned int GetNHits() const {return fHitId.size();}

  std::map<Double_t, std::pair<Int_t, Int_t> > GetHits() {return fHitId;}

  // Modifiers -----------------------
  void AddHit(unsigned int detId, unsigned int hitId, Double_t rho);
  void DeleteHit(unsigned int detId, unsigned int hitId);
  int HitInTrack(unsigned int detId, unsigned int hitId);

  void Reset();


private:

  // Private Data Members ------------
	std::map<Double_t, std::pair<Int_t, Int_t> > fHitId;  ///< first index is detId, second index is hit Id

public:
  ClassDef(PndTrackCand,1)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
