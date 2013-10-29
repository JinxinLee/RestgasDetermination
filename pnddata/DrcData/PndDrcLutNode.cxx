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
  fDetectorId = nodeId;
  fSize = 0;
}

PndDrcLutNode::PndDrcLutNode(Int_t detectorId, TVector3 dir)
{  
  fDetectorId = detectorId;
  AddEntry(dir);
}

void PndDrcLutNode::AddEntry(TVector3 dir) {
  fNodeArray.push_back(dir);
  fSize++;
}

void PndDrcLutNode::AddEntry(Int_t detectorId, TVector3 dir, Double_t pathid, Double_t time) {
  fDetectorId = detectorId; 
  fNodeArray.push_back(dir);
  fPathIdArray.push_back(pathid);
  fTimeArray.push_back(time);
  fSize++;
}
