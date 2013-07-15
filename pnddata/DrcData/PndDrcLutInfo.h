// -----------------------------------------
// PndDrcLutInfo.h
//
// Created on: 13.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------
// Container for look-up table

#ifndef PNDDRCLUTINFO_H
#define PNDDRCLUTINFO_H

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <vector>

class PndDrcLutInfo : public TObject {

public:    
  
  // Default constructor
  PndDrcLutInfo ();

  // Standard constructors


  // Copy constructor 
  PndDrcLutInfo (PndDrcLutInfo& node) { *this = node; }  

  // Modifiers
  void AddEntrie(Double_t val);
  
  // Accessors
  Int_t Entries() { return fSize; }

  TVector3 GetEntrie(Int_t entrie);

protected:

  Int_t fSize;
  std::vector<Double_t> fAnglesArray;
  
  ClassDef(PndDrcLutInfo,2)
};

#endif
