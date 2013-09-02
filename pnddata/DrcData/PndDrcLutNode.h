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
  PndDrcLutNode (Int_t nodeId);
  PndDrcLutNode (Int_t nodeId, TVector3 dir); 

  // Copy constructor 
  PndDrcLutNode (PndDrcLutNode& node) { *this = node; }  

  // Modifiers
  void AddEntry(TVector3 dir);
  void AddPathId(Double_t pathid);
  
  // Accessors
  Int_t Entries() { return fSize; }
  Double_t GetNodeId() { return fNodeId; }

  TVector3 GetEntry(Int_t entry);
  Double_t GetPathId(Int_t entry);

protected:

  Int_t fNodeId;
  Int_t fSize;
  
  std::vector<TVector3> fNodeArray;
  std::vector<Double_t> fPathIdArray;

  ClassDef(PndDrcLutNode,1)
};

#endif //PNDDRCLUTNODE_H
