/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//    PndLheCandidate class - implementation of track for LHE tracking         //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

#ifndef PND_LHE_CANDIDATE_H
#define PND_LHE_CANDIDATE_H

#include "TObject.h"
#include "TVector3.h"
#include "TArrayI.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include "PndLhePoint.h"
#include "PndLheHit.h"
//#include "lhe.h"

#include "Riostream.h"

class TrackCand;

class PndLheCandidate: public TObject {
  
  //  using namespace std;

protected:
  
  TObjArray  *fRealHits;           // Array of indices of hits

  Int_t   fQ;                   // track charge
  Int_t   fTrackNumber;         // track number in
  Bool_t  fFromMainVertex;      // true for primary tracks
  //  Double_t  fRadius;         // Radius of the helix (projected to a circle)
  Double_t   fMoment;         // momentum estimation
  Bool_t  fGood;                //

  Double_t fDipChi2;
  Double_t fTanDipAngle;
  Double_t fTanDipAngleErr;
  Double_t fZ0;
  Double_t fZ0Err;

  Int_t fMvdHits;                      // Number of MVD hits
  Int_t fTpcHits;                      // Number of TPC hits
  Int_t fGemHits;                      // Number of GEM hits
  
  PndLhePoint fVertex;              // track vertex
  PndLhePoint fLastHit;             // last track hit
  PndLhePoint fFirstHit;            // first track hit
  PndLhePoint fCircle;              // circle x,y,R

  TArrayI fCorrTrackIds;               // A unique list of correlated Ids
  TArrayI fMultTrackIds;               // A unique list of multiplicity of correlated Ids

  //  ---  the code below is needed only for testing with Geant

  Int_t   fPid;               // GEANT particle id
  TVector3   fP;              // track momentum

  // -----------

public:
  
  PndLheCandidate ();                              // constructor
  PndLheCandidate (Int_t tracknumber);             // constructor with tracknumber

  virtual  ~PndLheCandidate ();                    //

  void SetDefaults();                // performs the default setup for the track
  void AddHit(PndLheHit *point);     // adds a hit to the track

  // getters

  TObjArray  *GetRHits()           const { return fRealHits; }

  Int_t   GetTrackNumber()         const { return fTrackNumber; }
  Bool_t  IsGood()                 const { return fGood; }
  Int_t   GetPid()                 const { return fPid; }
  Int_t   GetNumberOfHits()        const { return fRealHits->GetEntriesFast(); }
  Bool_t  ComesFromMainVertex()    const { return fFromMainVertex; }
  TVector3   GetMomentum()         const { return fP; }

  Double_t  GetMoment ()           { return fMoment; }
  Double_t  GetTheta ();
  Double_t  GetTanAlpha ();
  Double_t  GetDipChi2()            const { return fDipChi2; }
  Double_t   GetTanDipAngle()       const { return fTanDipAngle; }
  Double_t   GetZ0()                const { return fZ0; }

  Double_t   GetPx()                const { return fP.X(); }
  Double_t   GetPy()                const { return fP.Y(); }
  Double_t   GetPz()                const { return fP.Z(); }
  Double_t   GetPt()                const;
  Double_t   GetP()                 const;
  Double_t   GetPseudoRapidity()    const;

  Int_t      GetMvdHits()           const { return fMvdHits; }
  Int_t      GetTpcHits()           const { return fTpcHits; }
  Int_t      GetGemHits()           const { return fGemHits; }
  
  TrackCand* GetTrackCand();
  
  PndLhePoint   GetVertex()      const { return fVertex;  }
  PndLhePoint   GetLastHit()     const { return fLastHit; }
  PndLhePoint   GetFirstHit()    const { return fFirstHit; }
  PndLhePoint   GetCircle()      const { return fCircle;  }
  Int_t   GetCharge()               const { return fQ; }

  Float_t ExtrapolateToZ(TVector3 *mom, TVector3 *vertex, const Float_t z = 0.); // extrapolate momentum and vertex at z=...
  Float_t ExtrapolateToR(TVector3 *mom, TVector3 *vertex, const Float_t R);      // extrapolate momentum and vertex at sqrt(x*x+y*y)=...
  
  Short_t GetNCorrTrackId(void)     const { return fCorrTrackIds.GetSize(); }
  Int_t   GetTrackID(Int_t i=0)     const { return fCorrTrackIds[i]; }
  Int_t   GetMultTrackID(Int_t i=0) const { return fMultTrackIds[i]; }
 
  // setters   

  Double_t  GetRadius()           const { return  fCircle.GetZ(); }
  void SetRadius(Double_t f)        { fCircle.SetZ(f); }

  void SetTrackNumber(Int_t num);
	    
  void SetPx(Double_t f) {fP.SetX(f); }
  void SetPy(Double_t f) {fP.SetY(f); }
  void SetPz(Double_t f) {fP.SetZ(f); }

  void SetMoment(Double_t f);

  void SetTanDipAngle(Double_t f) {fTanDipAngle = f;}
  void SetTanDipAngleErr(Double_t f) {fTanDipAngleErr = f;}
  void SetDipChi2(Double_t f) {fDipChi2 = f;}

  void SetZ0(Double_t f) {fZ0 = f;}
  void SetZ0Err(Double_t f) {fZ0Err = f;}

  void SetGood(Bool_t f)  { fGood = f;  }

  void SetMvdHits(Int_t hits) { fMvdHits = hits; }
  void SetTpcHits(Int_t hits) { fTpcHits = hits; }
  void SetGemHits(Int_t hits) { fGemHits = hits; }
 
  void   SetVertex(Double_t vx, Double_t vy, Double_t vz); 
  void   SetLastHit(Double_t vx, Double_t vy, Double_t vz);
  void   SetFirstHit(Double_t vx, Double_t vy, Double_t vz);
  void   SetCircle(Double_t vx, Double_t vy, Double_t vz);

  void   SetCharge(Int_t f) {fQ = f; }
  void   SetPid(Int_t f) {fPid = f; }

  void   ComesFromMainVertex(Bool_t f) { fFromMainVertex = f; }
  
  void   SetTrackID(const TArrayI track, const TArrayI mult) { fCorrTrackIds = track;  fMultTrackIds = mult; }
  void   SortHits();
  virtual void   Print(); //
  void   PrintHits();

  ClassDef(PndLheCandidate , 1)    // LHE track class  

    };

//________________________________________________________________
inline void PndLheCandidate::SetMoment (Double_t f) {
  //
  fMoment = f* 1.e-3;

#if 0
  // beta --- angle between Z and projection on XZ plane
  SetPz(fMoment*TMath::Cos(fBeta) /
	TMath::Sqrt(1. + fTanAlpha*fTanAlpha));
  SetPx(fMoment*TMath::Sin(fBeta) /
	TMath::Sqrt(1. + fTanAlpha*fTanAlpha));
  SetPy(fMoment*fTanAlpha /
	TMath::Sqrt(1. + fTanAlpha*fTanAlpha) );
#endif

}

//________________________________________________________________
inline  Double_t  PndLheCandidate::GetTheta () {
  return TMath::ATan2 (fP.X(), fP.Z());
}

//________________________________________________________________
inline Double_t  PndLheCandidate::GetTanAlpha () {
  return   fP.Y()/TMath::Sqrt(fP.X()*fP.X() + fP.Z()*fP.Z());
}

//________________________________________________________________
inline Double_t PndLheCandidate::GetP() const {
  // Returns total momentum.
  return fP.Mag();
}

//________________________________________________________________
inline Double_t PndLheCandidate::GetPt() const {
  // Returns transverse momentum.
  return fP.Pt();
}

//________________________________________________________________
inline Double_t PndLheCandidate::GetPseudoRapidity() const {
  // Returns the pseudorapidity of the particle.
  return 0.5 * TMath::Log((fP.Mag() + fP.Z()) / (fP.Mag() - fP.Z()));  
}

 ///:~

#endif
