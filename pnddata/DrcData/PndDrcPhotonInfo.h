// -----------------------------------------
// PndDrcLutInfo.h
//
// Created on: 18.10.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------
// Container for DrcTrackInfo

#ifndef PNDDRCPHOTONINFO_H
#define PNDDRCPHOTONINFO_H

#include "PndDrcAmbiguityInfo.h"

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <vector>

class PndDrcPhotonInfo : public TObject {

public:    
  
  // Default constructor
  PndDrcPhotonInfo ();

  // Default destructor
  ~PndDrcPhotonInfo ();

  // Copy constructor 
  PndDrcPhotonInfo (const PndDrcPhotonInfo& val) { *this = val; }  

  // Mutators
  void SetHitTime(Double_t val)               {fHitTime = val;}
  void SetReflected(Bool_t val)               {fReflected = val;}
  void SetEvReflections(Int_t val)            {fEvReflections = val;}

  void AddAmbiguity(PndDrcAmbiguityInfo ambiguity);

  // Accessors  
  Double_t    GetHitTime()	              {return fHitTime;}    
  Bool_t GetReflected()	                      {return fReflected;}
  Int_t GetEvReflections()	              {return fEvReflections;}

  Int_t GetAmbiguitySize()	              {return fAmbiguitySize;}
  PndDrcAmbiguityInfo GetAmbiguity(Int_t id)  {return fAmbiguityArray[id];}
protected:

  std::vector<PndDrcAmbiguityInfo> fAmbiguityArray;
  Int_t fAmbiguitySize;

  Double_t fHitTime;
  Bool_t   fReflected;
  Int_t    fEvReflections;
 
  ClassDef(PndDrcPhotonInfo,1)
};

#endif
