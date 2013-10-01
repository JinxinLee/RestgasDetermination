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

#include "TClonesArray.h"

using namespace std;


PndTrkNeighboringMap::PndTrkNeighboringMap(TClonesArray *tubearray) : fTubeArray(tubearray) {}

PndTrkNeighboringMap::PndTrkNeighboringMap(const PndTrkNeighboringMap &thismap) {
  *this = thismap;
}

PndTrkNeighboringMap::~PndTrkNeighboringMap() {}

// CHECK this might still have problems
PndTrkNeighboringMap& PndTrkNeighboringMap::operator=(const PndTrkNeighboringMap &thismap) {

  TMapIter *it = (TMapIter*) hit2neigh.MakeIterator();
  TObjArray *hits;
  while(PndTrkHit *hit = (PndTrkHit*) it->Next()) {
    hits = (TObjArray*) thismap.hit2neigh.GetValue(hit);
    hit2neigh.Add(hit, hits);
  }

  TMapIter *it2 = (TMapIter*) hit2indiv.MakeIterator();
  TObjArray *hits2;
  while(PndTrkHit *hit = (PndTrkHit*) it2->Next()) {
    hits2 = (TObjArray*) thismap.hit2indiv.GetValue(hit);
    hit2indiv.Add(hit, hits);
  }


  fTubeArray = thismap.fTubeArray;
  fStandalone = thismap.fStandalone;
  fOneNeigh = thismap.fOneNeigh;
  fTwoNeigh = thismap.fTwoNeigh;
  return *this;
}

void PndTrkNeighboringMap::Clear() {

  hit2neigh.Clear();
  hit2indiv.Clear();
  fStandalone.Clear();
  fOneNeigh.Clear();
  fTwoNeigh.Clear();

}

void PndTrkNeighboringMap::AddNeighboringsToHit(PndTrkHit *hit, TObjArray *hits) {

  //  hit->DrawTube(kGreen); // CHECK

 


  hit2neigh.Add(hit, hits);
  int tubeID = hit->GetTubeID();
  PndSttTube *tube = (PndSttTube*) fTubeArray->At(tubeID);
  
  if(hits->GetEntriesFast() == 0) fStandalone.Add(hit);
  else if(hits->GetEntriesFast() == 1) fOneNeigh.Add(hit);
  else if(hits->GetEntriesFast() == 2) fTwoNeigh.Add(hit);
  else if(tube->GetLayerID() == 0) fTwoNeigh.Add(hit);

  // fill indivisible map -------------------------------------------

  // up to 2 hits
  if(hits->GetEntriesFast() <= 2) {
    hit2indiv.Add(hit, hits);
    cout << "Bset up map " << hit->GetHitID() << " " << hits->GetEntriesFast() << endl; 
    return;
  }

  // more hits
  TObjArray *indiv = new TObjArray();
  
  int counter = 0;
  for(int k = 0; k < hits->GetEntriesFast(); k++) {
    PndTrkHit *hit2 = (PndTrkHit*) hits->At(k);
    PndSttTube *tube2 = (PndSttTube* ) fTubeArray->At(hit2->GetTubeID());
    if(tube->GetLayerID() == tube2->GetLayerID()) continue;
      counter++;
      indiv->Add(hit2);

      //      hit2->DrawTube(kRed); // CHECK
  }
  

  if(counter > 2) indiv->Clear();
  hit2indiv.Add(hit, indiv);
  cout << "Aset up map " << hit->GetHitID() << " " << indiv->GetEntriesFast() << endl; 
  
}

TObjArray PndTrkNeighboringMap::GetSeeds() {
  TObjArray seeds;
  for(int ihit = 0; ihit < fOneNeigh.GetEntriesFast(); ihit++) {
    PndTrkHit *hit = (PndTrkHit*) fOneNeigh.At(ihit);
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

 
  for(int ihit = 0; ihit < fTwoNeigh.GetEntriesFast(); ihit++) {
    int samelayer = -1;
    int standalone = -2;
    PndTrkHit *hit = (PndTrkHit*) fTwoNeigh.At(ihit);
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
   
TObjArray PndTrkNeighboringMap::GetIndivisibles() {
  return fTwoNeigh;
}
   
TObjArray *PndTrkNeighboringMap::GetIndivisiblesToHit(PndTrkHit *hit) {
  // cout << "indiv hit "  << hit << " " << hit->GetHitID() << endl;
  return (TObjArray*) hit2indiv.GetValue(hit);
}

void PndTrkNeighboringMap::PrintIndivisibleMap() {
  TMapIter *it2 = (TMapIter*) hit2indiv.MakeIterator();
  TObjArray *hits2;
  while(PndTrkHit *hit = (PndTrkHit*) it2->Next()) {
    hits2 = (TObjArray*) hit2indiv.GetValue(hit);
  }
}

// Returns 0 if not found.
TObjArray * PndTrkNeighboringMap::GetNeighboringsToHit(PndTrkHit *hit) {
  return (TObjArray*) hit2neigh.GetValue(hit);
}

TMapIter *PndTrkNeighboringMap::GetIterator() {
  return (TMapIter*) hit2neigh.MakeIterator();
}

TObjArray PndTrkNeighboringMap::GetHitWithNNeighborings(int nofhits) {
  TObjArray listofhits;

  TMapIter *it = (TMapIter*) hit2neigh.MakeIterator();
  TObjArray *hits;
  while(PndTrkHit *hit = (PndTrkHit*) it->Next()) {
    hits = (TObjArray*) hit2neigh.GetValue(hit);
    if(hits->GetEntriesFast() == nofhits) listofhits.Add(hit);
  }
  return listofhits;
}
