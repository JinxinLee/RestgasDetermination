// -----------------------------------------
// PndDrcLutInfo.h
//
// Created on: 18.10.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------

#ifndef PNDDRCAMBIGUITYINFO_H
#define PNDDRCAMBIGUITYINFO_H

#include "PndDrcAmbiguityInfo.h"

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <vector>

class PndDrcAmbiguityInfo : public TObject {

public:    
  
  // Default constructor
  PndDrcAmbiguityInfo ();
  
  ~PndDrcAmbiguityInfo () { };
  
  // Copy constructor 
  PndDrcAmbiguityInfo (const PndDrcAmbiguityInfo& val) { *this = val; }  

  // Mutators
  void SetCherencov(Double_t val)           {fCherenkov = val;}
  void SetBarTime(Double_t val)             {fBarTime = val;}
  void SetEvTime(Double_t val)              {fEvTime = val;}

  // Accessors  
  Double_t GetCherencov() 	            {return fCherenkov;}
  Double_t GetBarTime()	                    {return fBarTime;}
  Double_t GetEvTime()	                    {return fEvTime;}
  
protected:

  Double_t    fCherenkov;
  Double_t    fBarTime;
  Double_t    fEvTime;
    
  ClassDef(PndDrcAmbiguityInfo,1)
};

#endif
