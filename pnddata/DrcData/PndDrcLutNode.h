// -----------------------------------------
// PndDrcLutNode.h
//
// Created on: 09.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------
// Container for look-up table

#ifndef PNDDRCLUTNODE_H
#define PNDDRCLUTNODE_H

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <vector>
#include <iostream>

class PndDrcLutNode : public TObject {

public:    
  
  // Default constructor
  PndDrcLutNode ();

  // Standard constructors
  PndDrcLutNode (Int_t detectorId);
  PndDrcLutNode (Int_t detectorId, TVector3 dir); 

  // Copy constructor 
  PndDrcLutNode (PndDrcLutNode& node) { *this = node; }  

  // Modifiers
  void AddEntry(TVector3 dir);
  void AddEntry(Int_t nodeId, TVector3 dir, Double_t pathid, Double_t time);
  
  // Accessors
  Int_t Entries() { return fSize; }
  Double_t GetDetectorId() { return fDetectorId; }

  TVector3 GetEntry(Int_t entry) { return fNodeArray[entry]; }
  Double_t GetPathId(Int_t entry){ return fPathIdArray[entry]; }
  Double_t GetTime(Int_t entry){ return fTimeArray[entry]; }


protected:

  Int_t fDetectorId;
  Int_t fSize;
  
  std::vector<TVector3> fNodeArray;
  std::vector<Double_t> fPathIdArray;
  std::vector<Double_t> fTimeArray;
  
  ClassDef(PndDrcLutNode,1)
};

#endif //PNDDRCLUTNODE_H
