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
  void AddEntrie(TVector3 dir);
  
  // Accessors
  Int_t Entries() { return fSize; }
  Int_t GetNodeId() { return fNodeId; }

  TVector3 GetEntrie(Int_t entrie);

protected:

  Int_t fNodeId;
  Int_t fSize;
  std::vector<TVector3> fNodeArray;
  
  ClassDef(PndDrcLutNode,2)
};

#endif //PNDDRCLUTNODE_H
