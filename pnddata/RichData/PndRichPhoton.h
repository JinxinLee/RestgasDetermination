// -------------------------------------------------------------------------
// -----                     PndRichPhoton header file                 -----
// -----               Created 01/11/14 by Konstantin Beloborodov      -----
// -----                                                               -----
// -------------------------------------------------------------------------


#ifndef PNDRICHPHOTON_H
#define PNDRICHPHOTON_H

#include "FairHit.h"
#include "PndRichMirrorSegment.h"
#include "PndRichPDHit.h"
#include "PndRichBarPoint.h"
#include <vector>

class PndRichMirrorSegment;
class PndRichPDHit;

class PndRichPhoton {

public:    
  
  /** Default constructor **/
  PndRichPhoton();

  PndRichPhoton(Double_t theta,
                Double_t phi,
                UInt_t tmask); 
   

/** Copy constructor **/
  PndRichPhoton(const PndRichPhoton& photon) { *this = photon; };

  /** Destructor **/
  virtual ~PndRichPhoton();    
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt ="") const;
  
  /** Modifiers **/
  
    
  /** Accessors **/
  virtual bool  TrackCalc ();
  virtual Double_t GetLength();
  virtual Double_t GetTime();
  virtual Double_t GetTheta();
  virtual Double_t GetPhi();
  virtual UInt_t GetTMask()  {return fTMask;}
  virtual std::vector<PndRichMirrorSegment*> GetMirror()  {return fMirrors;}
  virtual TVector3 GetHitPos() {return fHitPosition;}
  virtual std::vector<TVector3> GetMirrRefPos() {return fMirrRefPosition;}
  virtual PndRichBarPoint* GetTrack() {return fTrack;}
    
  void SetLength( Double_t length ) { fLength = length; }
  void SetDTime( Double_t time ) { fDTime = time; }
  void SetTime( Double_t time ) { fTime = time; }
  void SetTheta( Double_t theta ) { fTheta = theta; }
  void SetPhi( Double_t phi ) { fPhi = phi; }
  void SetPDHit ( PndRichPDHit* hit ) { fPDHit = hit; }
  void SetHitTime ( Double_t hitTime ) { fHitTime = hitTime; }
  void SetHitPos ( TVector3 hit ) { fHitPosition = hit; }
  void SetTrackPos ( TVector3 pos ) { fTrackPosition = pos; }
  void SetTrackDir ( TVector3 dir ) { fTrackDirection = dir; }
  void SetTrack ( PndRichBarPoint* track ) { fTrack = track; }
  void SetMirrRefPos ( std::vector<TVector3> pos ) { fMirrRefPosition = pos; }
  void SetMirror ( std::vector<PndRichMirrorSegment*> mirrors ) { fMirrors = mirrors; }

 protected:

  PndRichPDHit* fPDHit;
  Double_t fDTime;
  Double_t fTime;
  Double_t fHitTime;
  Double_t fTheta, fPhi;
  UInt_t fTMask;
  std::vector<PndRichMirrorSegment*> fMirrors;
  TVector3 fHitPosition;
  TVector3 fTrackPosition;
  TVector3 fTrackDirection;
  TVector3 fTrackPositionOld;
  TVector3 fTrackDirectionOld;
  std::vector<TVector3> fMirrRefPosition;
  Double_t fLength;
  PndRichBarPoint* fTrack;
   
  ClassDef(PndRichPhoton,1)
};

#endif //PNDRICHPHOTON_H
