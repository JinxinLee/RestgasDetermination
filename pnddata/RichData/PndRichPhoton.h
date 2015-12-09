// -------------------------------------------------------------------------
// -----                     PndRichPhoton header file                    -----
// -----               Created 01/11/14 by Konstantin Beloborodov      -----
// -----                                                               -----
// -------------------------------------------------------------------------


#ifndef PNDRICHPHOTON_H
#define PNDRICHPHOTON_H

#include "FairHit.h"

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
  virtual Double_t GetLength()  {return fLength;}
  virtual Double_t GetTime()  {return fTime;}
  virtual Double_t GetTheta()  {return fTheta;}
  virtual Double_t GetPhi()  {return fPhi;}
  virtual UInt_t GetTMask()  {return fTMask;}
  virtual UInt_t GetMirror()  {return fMirror;}
  virtual TVector3 GetHitPos() {return fHitPosition;}
  virtual TVector3 GetMirrRefPos() {return fMirrRefPosition;}
  void SetLength( Double_t length ) { fLength = length; }
  void SetTime( Double_t time ) { fTime = time; }
  void SetTheta( Double_t theta ) { fTheta = theta; }
  void SetPhi( Double_t phi ) { fPhi = phi; }
  void SetHitPos ( TVector3 pos ) { fHitPosition = pos; }
  void SetMirrRefPos ( TVector3 pos ) { fMirrRefPosition = pos; }
  void SetMirror ( UInt_t mirror ) { fMirror = mirror; }

 protected:
  
  Double_t fTime;
  Double_t fTheta, fPhi;
  UInt_t fTMask, fMirror;
  TVector3 fHitPosition;
  TVector3 fMirrRefPosition;
  Double_t fLength;
   
  ClassDef(PndRichPhoton,1)
};

#endif //PNDRICHPHOTON_H
