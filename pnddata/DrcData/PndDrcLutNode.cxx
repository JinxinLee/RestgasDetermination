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
  AddEntrie(dir);
}

void PndDrcLutNode::AddEntrie(TVector3 dir) {

  fNodeArray.push_back(dir);
  fSize++;
}


TVector3 PndDrcLutNode::GetEntrie(Int_t entrie) {
  return fNodeArray[entrie];
}

