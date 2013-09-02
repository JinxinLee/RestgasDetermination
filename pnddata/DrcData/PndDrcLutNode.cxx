// -----------------------------------------
// PndDrcLutNode.h
//
// Created on: 09.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PndDrcLutNode.h"

ClassImp(PndDrcLutNode)

// -----   Default constructor   -------------------------------------------
PndDrcLutNode::PndDrcLutNode()
{ 
  fSize = 0;
}

// -----   Standard constructors   -----------------------------------------
PndDrcLutNode::PndDrcLutNode(Int_t nodeId)
{ 
  fNodeId = nodeId;
  fSize = 0;
}

PndDrcLutNode::PndDrcLutNode(Int_t nodeId, TVector3 dir)
{  
  fNodeId = nodeId;
  AddEntry(dir);
}

void PndDrcLutNode::AddEntry(TVector3 dir) {
  fNodeArray.push_back(dir);
  fSize++;
}

void PndDrcLutNode::AddPathId(Double_t pathid) {
  fPathIdArray.push_back(pathid);
}


TVector3 PndDrcLutNode::GetEntry(Int_t entry) {
  return fNodeArray[entry];
}

Double_t PndDrcLutNode::GetPathId(Int_t entry) {
  return fPathIdArray[entry];
}

