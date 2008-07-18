/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//    PndLhePidTrack class - implementation of track for LHE pid               //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

#ifndef PND_LHE_PID_TRACK_H
#define PND_LHE_PID_TRACK_H

#include "PndTpcLheTrack.h"

class PndLhePidTrack: public PndTpcLheTrack {
  
  //  using namespace std;

protected:

  Double_t fMvdELoss;               // Energy Loss in MVD
  Int_t fMvdHitCount;               // Number of mvd hits

  Double_t fSttELoss;               // Energy Loss in  STT
  Int_t fSttHitCount;               // Number of STT hits

  Int_t   fTofIndex;                // Index of PndTofHit object
  Float_t fTofDeltaPhi;             // phi (TRACK-TOF)
  Float_t fTofDeltaZ;               // Z (TRACK-TOF)
  Float_t fTofQuality;              // chi2 TRACK-TOF correlation
  Float_t fTofPathLength;           // path length to TOF hit

  Int_t   fEmcIndex;                // Index of PndEmcCluster object
  Float_t fEmcDeltaPhi;             // phi (TRACK-EMC)
  Float_t fEmcDeltaZ;               // Z (TRACK-EMC)
  Float_t fEmcQuality;              // chi2 TRACK-EMC correlation
   
  Int_t   fMdtIndex;                // Index of PndMdtHit object
  Float_t fMdtDeltaPhi;             // phi (TRACK-MDT)
  Float_t fMdtDeltaZ;               // Z (TRACK-MDT)
  Float_t fMdtQuality;              // chi2 TRACK-MDT correlation
  Int_t   fMdtModule;               // MDT module number (1 barrel, 2 endcup)
  Int_t   fMdtLayerCount;           // Number of fired layers
  Float_t fMdtChi2;                 // MDT chi2 
  
  Float_t fTimeOfFlight;            // time-of-flight
  Float_t fEmcELoss;                // EMC energy loss
  
  
  
public:
  
  PndLhePidTrack ();                              // constructor

  /** Copy constructor **/
  PndLhePidTrack (const PndTpcLheTrack& track);
  PndLhePidTrack (const PndLhePidTrack& track);
 
  Double_t GetMvdELoss()              { return fMvdELoss;    };
  Int_t GetMvdHitCounts()             { return fMvdHitCount; };

  Double_t GetSttELoss()              { return fSttELoss;    };
  Int_t GetSttHitCounts()             { return fSttHitCount; };

  Int_t GetTofIndex()                 { return fTofIndex; }
  Float_t GetTofDeltaPhi()            { return fTofDeltaPhi; }
  Float_t GetTofDeltaZ()              { return fTofDeltaZ; }
  Float_t GetTofQuality()             { return fTofQuality; }
  Float_t GetTofPathLength()          { return fTofPathLength; }
 
  Int_t GetEmcIndex()                 { return fEmcIndex; }
  Float_t GetEmcDeltaPhi()            { return fEmcDeltaPhi; }
  Float_t GetEmcDeltaZ()              { return fEmcDeltaZ; }
  Float_t GetEmcQuality()             { return fEmcQuality; }
  
  Int_t GetMdtIndex()                 { return fMdtIndex; }
  Float_t GetMdtDeltaPhi()            { return fMdtDeltaPhi; }
  Float_t GetMdtDeltaZ()              { return fMdtDeltaZ; }
  Float_t GetMdtQuality()             { return fMdtQuality; }
  Int_t GetMdtModule()                { return fMdtModule; }
  Int_t GetMdtLayerCount()            { return fMdtLayerCount; }
  Float_t GetMdtChi2()                { return fMdtChi2; }

  Float_t GetTof()                    { return fTimeOfFlight; }
  Float_t GetEmcELoss()               { return fEmcELoss; }
  Float_t GetBeta()                   { return fTimeOfFlight>0. ? fTofPathLength/fTimeOfFlight : 0.; }
  
  void SetMvdELoss(Double_t eloss)    { fMvdELoss = eloss;    };
  void SetMvdHitCounts(Int_t nhit)    { fMvdHitCount = nhit;  };
 
  void SetSttELoss(Double_t eloss)    { fSttELoss = eloss;    };
  void SetSttHitCounts(Int_t nhit)    { fSttHitCount = nhit;  };

  void SetTofIndex(Int_t ind)         { fTofIndex = ind; };
  void SetTofDeltaPhi(Float_t dphi)   { fTofDeltaPhi = dphi; };
  void SetTofDeltaZ(Float_t dz)       { fTofDeltaZ = dz; };
  void SetTofQuality(Float_t qq)      { fTofQuality = qq; };
  void SetTofPathLength(Float_t len)  { fTofPathLength = len; };

  void SetEmcIndex(Int_t ind)         { fEmcIndex = ind; };
  void SetEmcDeltaPhi(Float_t dphi)   { fEmcDeltaPhi = dphi; };
  void SetEmcDeltaZ(Float_t dz)       { fEmcDeltaZ = dz; };
  void SetEmcQuality(Float_t qq)      { fEmcQuality = qq; };

  void SetMdtIndex(Int_t ind)         { fMdtIndex = ind; };
  void SetMdtDeltaPhi(Float_t dphi)   { fMdtDeltaPhi = dphi; };
  void SetMdtDeltaZ(Float_t dz)       { fMdtDeltaZ = dz; };
  void SetMdtQuality(Float_t qq)      { fMdtQuality = qq; };
  void SetMdtModule(Int_t mod)        { fMdtModule = mod; };
  void SetMdtLayerCount(Int_t lay)    { fMdtLayerCount = lay; };
  void SetMdtChi2(Float_t chi2)       { fMdtChi2 = chi2; };


  void SetTof(Float_t tof)            { fTimeOfFlight = tof; };
  void SetEmcELoss(Float_t eloss)     { fEmcELoss = eloss; };
  
  virtual  ~PndLhePidTrack ();
  
  ClassDef(PndLhePidTrack , 3)   

    };


#endif
