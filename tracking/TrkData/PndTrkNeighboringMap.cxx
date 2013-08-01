//
// PndTrkNeighboringMap.cxx
// 
// 
// 
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//
////////////////////////////////////////////////////////////

#include "PndTrkNeighboringMap.h"

#include "PndTrkHit.h"
#include "PndSttTube.h"

using namespace std;


PndTrkNeighboringMap::PndTrkNeighboringMap(TClonesArray *tubearray) : fTubeArray(tubearray) {}

PndTrkNeighboringMap::~PndTrkNeighboringMap() {}


void PndTrkNeighboringMap::AddNeighboringsToHit(PndTrkHit *hit, TObjArray *hits) {
  hit2neigh.Add(hit, hits);
  int tubeID = hit->GetTubeID();
  PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
  
  if(hits->GetEntriesFast() == 0) fStandalone.Add(hit);
  else if(hits->GetEntriesFast() == 1) fSeeds.Add(hit);
  else if(hits->GetEntriesFast() == 2) fCandseeds.Add(hit);
  else if(tube->GetLayerID() == 0) fCandseeds.Add(hit);
}

TObjArray PndTrkNeighboringMap::GetSeeds() {
  TObjArray seeds;
  for(int ihit = 0; ihit < fSeeds.GetEntriesFast(); ihit++) {
    PndTrkHit *hit = (PndTrkHit*) fSeeds.At(ihit);
    int tubeID = hit->GetTubeID();
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
    TObjArray *neighs = GetNeighboringsToHit(hit);

    // if it has only one neighboring tube
    if(neighs->GetEntriesFast() == 1) {
      PndTrkHit *hit2 = (PndTrkHit*) neighs->At(0);
      int tubeID2 = hit2->GetTubeID();
      PndSttTube *tube2 = (PndSttTube*) fTubeArray->At(tubeID2);
      bool difflayer = false;
      // if the k-neigh is on the same layer...
      if(tube->GetLayerID() == tube2->GetLayerID()) {
	TObjArray *neighs2 = GetNeighboringsToHit(hit2);
	if(neighs2->GetEntriesFast() <= 1) continue;
	// ...and has more than 1 neighboring, then loop over them:
	// if there is at least one on a different layer, then hit is not a seed
	for(int jhit = 0; jhit < neighs2->GetEntriesFast(); jhit++) {
	  PndTrkHit *hit2b = (PndTrkHit*) neighs2->At(jhit);
	  if(hit2b == hit) continue;
	  Int_t tubeID2b = hit2b->GetTubeID();
	  PndSttTube *tube2b = (PndSttTube*) fTubeArray->At(tubeID2b);
	  if(tube2->GetLayerID() != tube2b->GetLayerID()) difflayer = true;
	}
	if(difflayer == true)  continue;
      }
      seeds.Add(hit);
    }
 //    else { // else, it means it belongs to the first layer
//       bool difflayer = false;
//       for(int jhit = 0; jhit < neighs->GetEntriesFast(); jhit++) {      
// 	PndTrkHit *hit2 = (PndTrkHit*) neighs->At(jhit);
// 	int tubeID2 = hit2->GetTubeID();
// 	PndSttTube *tube2 = (PndSttTube*) fTubeArray->At(tubeID2);
// 	if(tube2->GetLayerID() != tube->GetLayerID()) difflayer = true;
//       }
//       if(difflayer == false)  continue;
//       seeds.Add(hit);
//     }
  } 
  return seeds;
}


TObjArray PndTrkNeighboringMap::GetCandseeds() {
  TObjArray candidateseeds;

 
  for(int ihit = 0; ihit < fCandseeds.GetEntriesFast(); ihit++) {
    int samelayer = -1;
    int standalone = -2;
    PndTrkHit *hit = (PndTrkHit*) fCandseeds.At(ihit);
    int tubeID = hit->GetTubeID();
    PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
    TObjArray *neighs = GetNeighboringsToHit(hit);

    // if it is a candidate seed because it comes from the 1st layer
    if(tube->GetLayerID() == 0) {
      bool difflayer = false;
      for(int jhit = 0; jhit < neighs->GetEntriesFast(); jhit++) {      
	PndTrkHit *hit2 = (PndTrkHit*) neighs->At(jhit);
	int tubeID2 = hit2->GetTubeID();
	PndSttTube *tube2 = (PndSttTube*) fTubeArray->At(tubeID2);
	if(tube2->GetLayerID() != tube->GetLayerID()) difflayer = true;
      }
      if(difflayer == false)  continue;
      candidateseeds.Add(hit);
    }
    else { // because it has 2 neigh, whose one on the same layer
      for(int jhit = 0; jhit < neighs->GetEntriesFast(); jhit++) {
	PndTrkHit *hit2 = (PndTrkHit*) neighs->At(jhit);
	if(GetNeighboringsToHit(hit2)->GetEntriesFast() == 1) standalone = jhit;
	int tubeID2 = hit2->GetTubeID();
	PndSttTube *tube2 = (PndSttTube*) fTubeArray->At(tubeID2);
	if(tube->GetLayerID() == tube2->GetLayerID()) samelayer = jhit;
      }
      if(samelayer == standalone) candidateseeds.Add(hit);
    }
  }
  return candidateseeds;
}
   

// Returns 0 if not found.
TObjArray * PndTrkNeighboringMap::GetNeighboringsToHit(PndTrkHit *hit) {
  return (TObjArray*) hit2neigh.GetValue(hit);
}

TMapIter *PndTrkNeighboringMap::GetIterator() {
  return (TMapIter*) hit2neigh.MakeIterator();
}
