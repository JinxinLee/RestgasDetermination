#ifndef PNDENDRC_H
#define PNDENDRC_H

////////////////////////////////////////////////////////////////////////////////
//
//  PndEnDrc
//
//  Interface to virtual monte carlo
//
//  created 2008 D. Glazier, K. Foehl Edinburgh
//
////////////////////////////////////////////////////////////////////////////////

#include "FairDetector.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TGeoNode.h"

class PndEnDrcPoint;
class FairVolume;

class PndEnDrc : public FairDetector
{

public:
  /// constructor, destructor
  PndEnDrc();
  PndEnDrc(const char *name, Bool_t active);
  virtual ~PndEnDrc();

  PndEnDrcPoint    *AddPoint(Int_t trackID, Int_t detID, Int_t pdgCode,
			     TVector3 pos, TVector3 mom, Double_t tof, 
			     Double_t length, Double_t eLoss,
			     Double_t phote, Double_t fpz, Double_t ang);
  virtual void          ConstructGeometry();
  virtual void          ConstructRootGeometry();
  virtual void          EndOfEvent();
  virtual TClonesArray *GetCollection(Int_t iColl) const;
  virtual Bool_t        ProcessHits(FairVolume *vol = 0);
  virtual Bool_t        ProcessTrueHits(FairVolume *vol = 0);
  virtual void          Register();
  virtual void          Reset();
  virtual void          ExpandNode(TGeoNode *N0);
private:
  /// pointer to output array
  TClonesArray *fEnDrcPointCollection;
  TClonesArray *fEnDrcTrueCollection;

  /// the values, stored for AddPoint
  Int_t    fTrackID;      // the track number
  Int_t    fDetID;        // the ID of the volume that got hit
  Int_t    fPdgCode;      // PDG Code of particle
  TVector3 fPos;          // position
  TVector3 fMom;          // momentum
  Double_t fTof;          // time of flight
  Double_t fLength;       // length of track
  Double_t fELoss;        // energy loss

  Double_t fPAR[12];  //parameters for pm position
  ClassDef(PndEnDrc,1)
};

#endif // PNDENDRC_H
