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

#include <vector>
#include <map>

typedef std::multimap<Double_t, std::pair<Int_t, Int_t> >::const_iterator mapIter;

class PndTrackCandHit
{
public :
	PndTrackCandHit():fHitId(-1), fDetId(-1), fRho(0){}
	PndTrackCandHit(Int_t detId, Int_t hitId, Double_t rho):fDetId(detId), fHitId(hitId), fRho(rho){}
	friend bool operator< (const PndTrackCandHit& lhs, const PndTrackCandHit& rhs)
		{return lhs.GetRho()<rhs.GetRho();};
	bool operator== (const PndTrackCandHit& hit){
		if (GetHitId() == hit.GetHitId() && GetDetId() == hit.GetDetId())
			return true;
		return false;
	}

	Int_t GetHitId()const {return fHitId;}
	Int_t GetDetId()const {return fDetId;}
	Double_t GetRho()const {return fRho;}

private :
	Int_t fHitId;
	Int_t fDetId;
	Double_t fRho;		///< sorting parameter

	ClassDef(PndTrackCandHit,1);
};


ClassImp(PndTrackCandHit);

class PndTrackCand : public TObject {
public:

  // Constructors/Destructors ---------
  PndTrackCand();
  ~PndTrackCand();

  // operators
  friend bool operator== (const PndTrackCand& lhs, const PndTrackCand& rhs);

  // Accessors -----------------------
  PndTrackCandHit GetSortedHit(unsigned int i){
	  if (sorted == false)
		  Sort();
	  return fHitId.at(i);
  }
  unsigned int GetNHits() const {return fHitId.size();}

  std::vector<PndTrackCandHit>GetSortedHits();
  void Sort();

  // Modifiers -----------------------
  void AddHit(unsigned int detId, unsigned int hitId, Double_t rho);
  void DeleteHit(unsigned int detId, unsigned int hitId);
  int HitInTrack(unsigned int detId, unsigned int hitId);

  void Reset();


private:

  // Private Data Members ------------
	std::vector<PndTrackCandHit> fHitId;  ///< first index is detId, second index is hit Id
	bool sorted;
public:
  ClassDef(PndTrackCand,1)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
