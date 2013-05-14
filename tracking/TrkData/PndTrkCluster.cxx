//
// PndTrkCluster.cxx
// 
// 
// 
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//
////////////////////////////////////////////////////////////

#include "PndTrkCluster.h"

#include "PndDetectorList.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <iterator>
#include <algorithm>

#include "TMath.h"
#include "PndTrkSttHitList.h"
#include "PndTrkSdsHitList.h"
#include "PndTrkHit.h"

using namespace std;

Bool_t SorterFunction(PndTrkHit *hit1, PndTrkHit *hit2) {
  return *hit1 < *hit2;
}

PndTrkCluster::PndTrkCluster() : fIRegion(-1) {
  hitlist.clear();
  hitlist.reserve(100); 
}
 
PndTrkCluster::~PndTrkCluster() {
  hitlist.clear();
}

Bool_t PndTrkCluster::operator==(PndTrkCluster cluster) const {
  return hitlist.size() == cluster.GetNofHits(); // CHECK
}


void PndTrkCluster::AddHit(PndTrkHit *hit) {
  // fIRegion = hit->GetIRegion();
  hitlist.push_back(hit);
  hit->SetUsedFlag(1); // CHECK
}

// void PndTrkCluster::DeleteHit(PndTrkHit *hit) {
//   Int_t index = SearchHit(hit);
//   if(index != -1) DeleteHit(index);
//   else cout << "PndTrkCluster::DeleteHit: trying to delete a hit which is not in this cluster " << hit->GetHitID() << " " << hit->GetDetectorID() << endl;
// }

// CHECK delete does not work fine!
void PndTrkCluster::DeleteHit(Int_t index) {
  hitlist[index]->SetUsedFlag(0);
//   std::vector< PndTrkHit * > tmphitlist;
//   for(int ihit = 0; ihit < index; ihit++) {
//     cout << ihit << endl;
//     tmphitlist.push_back(hitlist[ihit]);
//   }
//   for(int ihit = index + 1; ihit < hitlist.size(); ihit++)  {
//     cout << ihit << endl;
//     tmphitlist.push_back(hitlist[ihit]);
//   }
//   hitlist = tmphitlist;
//   cout << "tmp/hitlist " << hitlist.size() << " " << tmphitlist.size() << endl;
  hitlist.erase(hitlist.begin() + index);

}

void PndTrkCluster::DeleteAllHits() {
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    DeleteHit(ihit);
  }
}

// =========================== GET DETECTOR SPECIFIC HIT LIST ============================

PndTrkCluster PndTrkCluster::GetMvdStripHitList() {
  PndTrkCluster cluster;
  std::vector< PndTrkHit* >::iterator itr = hitlist.begin();
  while(itr != hitlist.end()) {
    if((*itr)->IsMvdStrip()) {
      cluster.AddHit((*itr));
      cluster.SetIRegion(MVDSTRIP);
      //      cout << "found one" << (*itr)->GetHitID() << endl;
    }
    itr++;
  }
  return cluster;
}

PndTrkCluster PndTrkCluster::GetMvdPixelHitList() {
  PndTrkCluster cluster;
  std::vector< PndTrkHit* >::iterator itr = hitlist.begin();
  while(itr != hitlist.end()) {
    if((*itr)->IsMvdPixel()) {
      cluster.AddHit((*itr));
      cluster.SetIRegion(MVDPIXEL);
      //      cout << "found one" << (*itr)->GetHitID() << endl;
    }
    itr++;
  }
  return cluster;
}

PndTrkCluster PndTrkCluster::GetMvdHitList() {
  PndTrkCluster cluster;
  std::vector< PndTrkHit* >::iterator itr = hitlist.begin();
  while(itr != hitlist.end()) {
    if((*itr)->IsMvd()) {
      cluster.AddHit((*itr));
      cluster.SetIRegion((*itr)->GetIRegion()); 
      //      cout << "found one" << (*itr)->GetHitID() << endl;
    }
    itr++;
  }
  return cluster;
}

PndTrkCluster PndTrkCluster::GetSttParallelHitList()  {
  PndTrkCluster cluster;
  std::vector< PndTrkHit* >::iterator itr = hitlist.begin();
  while(itr != hitlist.end()) {
    if((*itr)->IsSttParallel()) {
      cluster.AddHit((*itr));
      cluster.SetIRegion((*itr)->GetIRegion()); 
      //      cout << "found one" << (*itr)->GetHitID() << endl;
    }
    itr++;
  }
  return cluster;
 }

PndTrkCluster PndTrkCluster::GetSttSkewHitList() 
   {
  PndTrkCluster cluster;
  std::vector< PndTrkHit* >::iterator itr = hitlist.begin();
  while(itr != hitlist.end()) {
    if((*itr)->IsSttSkew()) {
      cluster.AddHit((*itr));
      cluster.SetIRegion((*itr)->GetIRegion()); 
      //      cout << "found one" << (*itr)->GetHitID() << endl;
    }
    itr++;
  }
  return cluster;
}

PndTrkCluster PndTrkCluster::GetSttHitList() 
 {
  PndTrkCluster cluster;
  std::vector< PndTrkHit* >::iterator itr = hitlist.begin();
  while(itr != hitlist.end()) {
    if((*itr)->IsStt()) {
      cluster.AddHit((*itr));
      cluster.SetIRegion((*itr)->GetIRegion()); 
      //      cout << "found one" << (*itr)->GetHitID() << endl;
    }
    itr++;
  }
  return cluster;
}
// =======================================================================================


// finds the first instance of the hit in the cluster (there should be only one!)
Int_t PndTrkCluster::SearchHit(PndTrkHit *hit) {

  std::vector< PndTrkHit * >::iterator itr = hitlist.begin();
   itr = find(hitlist.begin(), hitlist.end(), hit);
   if(itr == hitlist.end()) return -1;
   return itr - hitlist.begin();
}

// PndTrkHit PndTrkCluster::SearchHit(Int_t hitID, Int_t detID) {}

// // CHECK ??? needed?
// void PndTrkCluster::MergeWithCluster(PndTrkCluster cluster2) {}

// ====================== when you will need this ===================
// void PndTrkCluster::AccessList() {

//   PndTrkSttHitList *instance1 = PndTrkSttHitList::Instance();
//   PndTrkSdsHitList *instance2 = PndTrkSdsHitList::InstancePixel();
//   PndTrkSdsHitList *instance3 = PndTrkSdsHitList::InstanceStrip();
  
//   /**
//      cout << "number of stt    hits " << instance1->GetNofHits() << endl;
//      cout << "number of mvdpix hits " << instance2->GetNofHits() << endl;
//      cout << "number of mvdstr hits " << instance3->GetNofHits() << endl;
//   **/

// }

// ==================================== V SHAPED CLUSTER =================================
// finds the extremities (first/last) of the cluster
// -------------------------------------------------
// considering the distance from the point frompoint;
// an extremity is a hit with only one neighbour.
// checks if there are correctly 2 extremities only (-> kTRUE)
// if there are 0 extremities  -> CHECK             (-> kFALSE)
// if there are >2 extremities -> CHECK             (-> kFALSE)
Bool_t PndTrkCluster::FindExtremitiesFrom(TVector3 frompoint, PndTrkHit &firstextremity, PndTrkHit &secondextremity) {
const  int nhits = MAXNOFHITSINCLUSTER; // hitlist.size(); // CHECK
 int associations[nhits] = {0};
  std::vector< PndTrkHit * >::iterator itr = hitlist.begin();
  std::vector< PndTrkHit * >::iterator jtr = hitlist.begin();
  while(itr != hitlist.end()) {
    PndTrkHit *hit = *itr;
    jtr = hitlist.begin();
    while(jtr != hitlist.end()) {
      if(itr == jtr) { 
	jtr++; 
	continue;
      }
      PndTrkHit *hit2 = *jtr;
      if(hit->GetXYDistance(hit2) < STTPARALDISTANCE) {
	//	cout << "d/i/j/a " << hit->GetXYDistance(hit2) << " " << itr - hitlist.begin() << " " << jtr - hitlist.begin() << " " << associations[itr - hitlist.begin()] << endl;
	associations[itr - hitlist.begin()]++;
      }
      jtr++;
    }
    itr++;
  } 

  int counter = 0;
  Double_t tmpdistance1 = 1000, tmpdistance2 = -1;

  for(int ihit = 0; ihit < hitlist.size(); ihit++) {
    //    cout << associations[ihit] << endl;
    if(associations[ihit] != 1) continue;
    counter++;
    double distance = hitlist[ihit]->GetXYDistance(frompoint);
    if(distance < tmpdistance1) {
      tmpdistance1 = distance;
       firstextremity = *hitlist[ihit];
    }
    
    if(distance > tmpdistance2) {
      tmpdistance2 = distance;
      secondextremity = *hitlist[ihit];
    }
  }

  if(counter == 2) return kTRUE;

  if(counter == 0) cout << "NO EXTREMITY FOUND :-(" << endl;
  if(counter > 2) cout << "TOO MANY EXTREMITIES FOUND :-( " << counter << endl;
  return kFALSE;
}

// checks if the cluster is V shaped:
// ----------------------------------
// it is run only when the distances between each of the two 
// found extremities and the frompoint are too similar.
// It computes the poca to frompoint and a circle which passes 
// among poca, firstextremity and secondextremity, then it
// checks the distance of each hit from this circle and if
// too many are too distant it means the cluster has V shape.
Bool_t PndTrkCluster::CheckClusterAgainsV(TVector3 frompoint,  PndTrkHit *firstextremity, PndTrkHit *secondextremity) {

  if(fabs(firstextremity->GetPosition().Perp() - secondextremity->GetPosition().Perp()) < 2) { 
    cout << "WARNING, V detected?" << endl;

    if(firstextremity == NULL || secondextremity == NULL) {
      cout << "NULL POINTER" << endl;
      return kFALSE;
    }

    if(*firstextremity == *secondextremity) {
      cout << "COINCIDENT EXTREMITIES" << endl; // CHECK
      return kFALSE;
    }


    PndTrkHit *pca = GetPocaTo(frompoint);
    if(*pca == *firstextremity || *pca == *secondextremity) {
      cout << "POCA COINCIDENT TO ONE OF THE EXTREMITIES" << endl; // CHECK
      return kFALSE;
    }

    Double_t x0, y0, radius = -1;
    Bool_t computation = ComputeCircle(firstextremity->GetPosition(), secondextremity->GetPosition(), pca->GetPosition(), x0, y0, radius);
    if(!computation) {
      cout << "TWO POINTS ALIGNED" << endl;
      return kFALSE;
    }
    std::vector< PndTrkHit * >::iterator itr = hitlist.begin();
    cout << "circle " << x0 << " " << y0 << " " << radius << endl;

    int counter = 0;
    while(itr != hitlist.end()) {
      PndTrkHit *hit = *itr;
      if(fabs(  hit->GetXYDistance(TVector3(x0, y0, 0.)) - radius) > 2) counter++;
      itr++;
    }

    if(hitlist.size() - counter < 3) cout << "***************** YES ***************** " << counter << " on " << hitlist.size() << endl;
    else cout << " ********************** NO ******************* " << counter << " " << hitlist.size() << endl;
    return kTRUE;

  }
  //  else cout << " DISTANCE " << fabs(firstextremity->GetPosition().Perp() - secondextremity->GetPosition().Perp()) << endl;

  return kFALSE;
}

// splits the cluster at the point athit (DOES NOT WORK)
Bool_t PndTrkCluster::SplitAtHit(PndTrkHit *athit, PndTrkCluster &cluster1, PndTrkCluster &cluster2) {

  std::vector< PndTrkHit * >::iterator itr = hitlist.begin();
  PndTrkHit *tmphit = *itr;

  cluster1.AddHit(athit);
  cluster2.AddHit(athit);
  TVector3 tmpposition = athit->GetPosition();
  PndTrkHit *secondhit = NULL;
  cout << "CLUS1: " << tmphit->GetHitID() << endl;
  while(itr != hitlist.end()) {
    PndTrkHit *hit = *itr;
    if(*hit == *athit) {
      cout << "hit == ahit " << endl;
      itr++;
      continue;
    }

    if(hit->GetXYDistance(tmpposition) < STTPARALDISTANCE) { // CHECK
      cluster1.AddHit(hit);
      tmpposition = hit->GetPosition();
      if(secondhit == NULL) secondhit = hit;

      cout << "CLUS1: " << hit->GetHitID() << endl;

    }
    itr++;
  }
  cout << "CLUS2: " << tmphit->GetHitID() << endl;

  tmpposition = athit->GetPosition();
  while(itr != hitlist.end()) {
    PndTrkHit *hit = *itr;
    if(*hit == *athit || *hit == *secondhit) {
      itr++;
      continue;
    }

    if(hit->GetXYDistance(tmpposition) < STTPARALDISTANCE) { // CHECK
      cluster2.AddHit(hit);
      tmpposition = hit->GetPosition();
   cout << "CLUS2: " << hit->GetHitID() << endl;
   }
    itr++;
  }
  

}

// splits the V shaped cluster at the point athit
// ----------------------------------------------
// starts from one extremity and stops @ athit,
// the same for the other extremity.
Bool_t PndTrkCluster::SplitV(PndTrkHit *athit, PndTrkHit *firstextremity, PndTrkHit* secondextremity, PndTrkCluster &cluster1, PndTrkCluster &cluster2) {
  //  cout << "splitV" << endl;
  std::vector< PndTrkHit * >::iterator itr = hitlist.begin();

  cluster1.AddHit(firstextremity);
  cluster2.AddHit(secondextremity);
  cluster1.SetIRegion(fIRegion);
  cluster2.SetIRegion(fIRegion);

  TVector3 tmpposition = firstextremity->GetPosition();

  PndTrkHit *hit = NULL, *tmphit = firstextremity;


 //  cout << "first " << firstextremity->GetHitID() << endl;
//   cout << "lat " << secondextremity->GetHitID() << endl;
//   cout << "athit " << athit->GetHitID() << endl;
//   cout << endl;

//   cluster1.Print();
//   cluster2.Print();
  for(int iclus = 0; iclus < 2; iclus++) {
//     cout << "iclus " << iclus << endl;
//  cluster2.Print();
    // set all unused
    while(itr != hitlist.end()) {
      (*itr)->SetUsedFlag(kFALSE);
      itr++;
    }
    itr = hitlist.begin();
    hit = NULL;
 
    if(iclus == 1) {
      tmpposition = secondextremity->GetPosition();
      tmphit = secondextremity;
    }
    double tmpdistance2 = 1000;
    while(!hit || (hit && (hit->GetPosition() - athit->GetPosition()).Perp() > STTPARALDISTANCE)) {
  //     if(!hit) cout << "here!" << endl;
//       if(hit) cout << "here " << (hit->GetPosition() - athit->GetPosition()).Perp() << endl;
      while(itr != hitlist.end()) {
	hit = *itr;
	//	if(hit->IsUsed()) continue;
	//	cout << "test " << hit->GetHitID() << " " << hit->GetDistance(tmpposition) << endl;
	cout << (*hit == *firstextremity) << " " << (*hit == *secondextremity) << " " << (*hit == *tmphit) << endl;

	if((iclus == 0 && cluster1.SearchHit(hit) != -1) || (iclus == 1 && cluster2.SearchHit(hit) != -1)) {
	  cout << hit->GetHitID() << " is already there .................." << endl;
	  itr++;
	  continue;
	}
	if(*hit == *firstextremity || *hit == *secondextremity || *hit == *tmphit) {
	  itr++;
	  continue;
	}

	if(hit->GetXYDistance(tmpposition) < STTPARALDISTANCE && (hit->GetPosition() - athit->GetPosition()).Perp() < tmpdistance2) { // CHECK
	//   cout << "add " << hit->GetHitID() << " to  cluster " << iclus + 1 << endl;
	  if(iclus == 0) cluster1.AddHit(hit);
	  else cluster2.AddHit(hit);
	  hit->SetUsedFlag(kTRUE);
	  tmpposition = hit->GetPosition();
	  tmphit = hit;
	  tmpdistance2 = (hit->GetPosition() - athit->GetPosition()).Perp();
	  break;
	}
	itr++;
      }
      itr = hitlist.begin();
    }
    cout << endl;
  }
  cout << "here2" << endl;

  // add the common hit
  cluster1.AddHit(athit);
  cluster2.AddHit(athit);

  if(cluster1.GetNofHits() == 0 || cluster2.GetNofHits() == 0) return kFALSE;  // CHECK!!!!!!!!!!!!!!!!!



  // set the fUsed flags to true for all used hits
  for(int ihit = 0; ihit < cluster1.GetNofHits(); ihit++) (cluster1.GetHit(ihit))->SetUsedFlag(kTRUE);
  for(int ihit = 0; ihit < cluster2.GetNofHits(); ihit++) (cluster2.GetHit(ihit))->SetUsedFlag(kTRUE);

  return kTRUE;
}

// =======================================================================================
// Bool_t PndTrkCluster::SorterFunction(PndTrkHit *hit1, PndTrkHit *hit2) {

// return *hit1 < *hit2;

// }


void PndTrkCluster::SortFromHit(PndTrkHit *firstextremity, TString criterion) {

  //  cout << "------------------------------------ sorting" << endl;
  fFromPoint = firstextremity->GetPosition();
  std::vector< PndTrkHit * >::iterator itr = hitlist.begin();

  PndTrkHit *hit;
  while(itr != hitlist.end()) {
    hit = *itr;
    if(criterion.CompareTo("xydistance") == 0) {      // CHECK 
      double distance = hit->GetDistance(fFromPoint);
      //   cout << "distance " << distance << endl;
      hit->SetSortVariable(distance);
    }

    // cout << hit->GetHitID() << " " << hit->GetDetectorID() << " sort var " << hit->GetSortVariable() << endl;
    itr++;
  }
  // sorting
  sort(hitlist.begin(), hitlist.end(), SorterFunction);
}

void PndTrkCluster::Sort()
{
  sort(hitlist.begin(), hitlist.end(), SorterFunction);

}


// =======================================================================================
// MERGE CLUSTERS

void PndTrkCluster::AddCluster(PndTrkCluster *cluster) {
  for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    hitlist.push_back(hit);
  }

  TString iregion1;
  iregion1 += fIRegion;
  TString iregion2;
  iregion2 += cluster->GetIRegion();

  // iregions



// fIRegion = 

}

void  PndTrkCluster::AddClusterAndSortFrom(PndTrkCluster *cluster, TVector3 frompoint, TString criterion) {

  fFromPoint = frompoint;
  std::vector< PndTrkHit * >::iterator itr = hitlist.begin();
  PndTrkHit *hit;
  for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    hit = cluster->GetHit(ihit);
    hitlist.push_back(hit);
  }

  while(itr != hitlist.end()) {
    hit = *itr;
    if(criterion.CompareTo("xydistance") == 0) {      // CHECK 
      double distance = hit->GetDistance(fFromPoint);
      //   cout << "distance " << distance << endl;
      hit->SetSortVariable(distance);
    }
    itr++;
  }
  // sorting
  sort(hitlist.begin(), hitlist.end(), SorterFunction);
  

}


// =======================================================================================
PndTrkHit * PndTrkCluster::GetPocaTo(TVector3 frompoint) {
  //  cout << "getPocaTo" << endl;
  Double_t tmpdistance = 1000;
  
  std::vector< PndTrkHit * >::iterator itr = hitlist.begin();
  PndTrkHit *tmphit = *itr;

  while(itr != hitlist.end()) {
    PndTrkHit *hit = *itr;
    double distance = hit->GetXYDistance(frompoint);
    if(distance < tmpdistance) {
      tmpdistance = distance;
      tmphit = hit;
    }
    itr++;
  } 
  return tmphit;
}  

Bool_t PndTrkCluster::ComputeCircle(TVector3 v1, TVector3 v2, TVector3 v3, double &x0, double &y0, double &R) {

  Double_t d1 = 2 * (v1 - v2).X();
  Double_t d2 = 2 * (v1 - v3).X();

  Double_t n1 = (v1 - v2).Y();
  Double_t n2 = (v1 - v3).Y();
  
  Double_t m1 = v1.Mag2() - v2.Mag2();
  Double_t m2 = v1.Mag2() - v3.Mag2();

  if((d2 * n1 - d1 * n2) == 0) {
    cout << "allineati" << endl; 
    return kFALSE;
  }

  y0 = 0.5 * (d2 * m1 - d1 * m2) / (d2 * n1 - d1 * n2); 
  if(d1 != 0)  x0 = (m1 - 2 * n1 * y0)/d1;
  else   x0 = (m2 - 2 * n2 * y0)/d2;
  
  R = (v1 - TVector3(x0, y0, 0)).Mag();
  return kTRUE;
}
// =======================================================================================


PndTrkHit *PndTrkCluster::GetHit(int index) {
  return hitlist[index];
}

void PndTrkCluster::Print() {
  cout << "###############################" << endl;
  cout << "iregion " << fIRegion << endl;
  std::vector< PndTrkHit* >::iterator itr;
  itr = hitlist.begin();
  while(itr != hitlist.end()) {
    cout << " " <<  (*itr)->GetHitID() << " " << (*itr)->GetDetectorID() << " " << (*itr)->GetSortVariable() << endl;
    itr++;
  }
}

void PndTrkCluster::PrintList(){
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkHit *hit = GetHit(ihit);
    cout << " " <<  hit->GetHitID();
  }
  cout << endl;
}



void PndTrkCluster::Draw(Color_t color) { 
  //  cout << "S: " <<  hitlist.size() << endl;
  std::vector< PndTrkHit* >::iterator itr = hitlist.begin();
  while(itr != hitlist.end()) {
    (*itr)->Draw(color);
    itr++;
  }
}

void PndTrkCluster::LightUp() { 
  Draw(kYellow);
}



ClassImp(PndTrkCluster)
 
