// -----------------------------------------
// PndDrcLutNode.h
//
// Created on: 09.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PndDrcLutNode.h"

ClassImp(PndDrcLutNode)

// -----   Default constructor   -------------------------------------------
PndDrcLutNode::PndDrcLutNode(): 
TObject(), fDetectorId(0), fSize(0), fNodePos(TVector3()), fNodeArray(), fPathArray(), fPathIdArray(), fTimeArray()
{ 
}

// -----   Standard constructors   -----------------------------------------
PndDrcLutNode::PndDrcLutNode(Int_t nodeId): 
  TObject(), fDetectorId(nodeId), fSize(0), fNodePos(TVector3()), fNodeArray(), fPathArray(), fPathIdArray(), fTimeArray()
{ 
}

void PndDrcLutNode::AddEntry(Int_t detectorId, TVector3 dir, Double_t path, Int_t pathid, Double_t time, TVector3 pos) {
  fDetectorId = detectorId; 
  fNodeArray.push_back(dir);
  fPathArray.push_back(path);
  fPathIdArray.push_back(pathid);
  fTimeArray.push_back(time);
  fNodePos = pos;
  fSize++;
}
