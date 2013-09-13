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

PndTrkCluster::PndTrkCluster() : fIRegion(-1), fFromPoint(0., 0., 0.), hitlist(TObjArray()) {
} 

PndTrkCluster::PndTrkCluster(const PndTrkCluster& cluster) {
  *this = cluster;
}

PndTrkCluster::~PndTrkCluster() {
  hitlist.Clear();
}

// operator equals
PndTrkCluster& PndTrkCluster::operator=(const PndTrkCluster &cluster) {
  fFromPoint = cluster.fFromPoint;
  fIRegion = cluster.fIRegion;
 
  // hitlist = *((TObjArray*) (&(cluster.hitlist)->Clone()));
  hitlist = TObjArray(cluster.hitlist);
  //  hitlist = cluster.hitlist;
  return *this;
}


Bool_t PndTrkCluster::operator==(PndTrkCluster cluster) const {
  return hitlist.GetEntriesFast() == cluster.GetNofHits(); // CHECK
}


void PndTrkCluster::AddHit(PndTrkHit *hit) {
  // fIRegion = hit->GetIRegion();
  hitlist.Add(hit);
  hit->SetUsedFlag(1); // CHECK
}

// CHECK if it works, never tried
void PndTrkCluster::DeleteHit(PndTrkHit *hit) {
  hit->SetUsedFlag(0);
  hitlist.Remove(hit);
}

// CHECK if it works, never tried
void PndTrkCluster::DeleteHitAndCompress(PndTrkHit *hit) {
  DeleteHit(hit);
  hitlist.Compress(); // CHECK
}

// CHECK if it works, never tried
void PndTrkCluster::DeleteHit(Int_t index) {
  PndTrkHit* hit = (PndTrkHit*) hitlist[index];
  hitlist.RemoveAt(index);
  hitlist.Compress(); // CHECK
}

// CHECK if it works, never tried
void PndTrkCluster::DeleteHitAndCompress(Int_t index) {
  DeleteHit(index);
  hitlist.Compress(); // CHECK
}

void PndTrkCluster::DeleteAllHits() {
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    DeleteHit(ihit);
  }
  hitlist.Compress();
}

void PndTrkCluster::DeleteHits(std::vector< int > todelete) {
  for(int ihit = 0; ihit < todelete.size(); ihit++) {
    int hitno = todelete[ihit];
    DeleteHit(hitno);
  }
  hitlist.Compress();
}

// =========================== GET DETECTOR SPECIFIC HIT LIST ============================

PndTrkCluster PndTrkCluster::GetMvdStripHitList() {
  PndTrkCluster cluster;
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkHit *hit =  (PndTrkHit*) hitlist[ihit];
      if(hit->IsMvdStrip()) {
	cluster.AddHit(hit);
	cluster.SetIRegion(MVDSTRIP);
	//      cout << "found one" << hit->GetHitID() << endl;
      }
  }
  return cluster;
}

PndTrkCluster PndTrkCluster::GetMvdPixelHitList() {
  PndTrkCluster cluster;
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkHit *hit =  (PndTrkHit*) hitlist[ihit];
      if(hit->IsMvdPixel()) {
	cluster.AddHit(hit);
	cluster.SetIRegion(MVDPIXEL);
	//      cout << "found one" << hit->GetHitID() << endl;
      }
  }
  return cluster;
}

PndTrkCluster PndTrkCluster::GetMvdHitList() {
  PndTrkCluster cluster;
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkHit *hit =  (PndTrkHit*) hitlist[ihit];
      if(hit->IsMvd()) {
	cluster.AddHit(hit);
	cluster.SetIRegion(hit->GetIRegion()); 
	//      cout << "found one" << hit->GetHitID() << endl;
      }
  }
  return cluster;
}

PndTrkCluster PndTrkCluster::GetSttParallelHitList()  {
  PndTrkCluster cluster;
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkHit *hit =  (PndTrkHit*) hitlist[ihit];
      if(hit->IsSttParallel()) {
	cluster.AddHit(hit);
	cluster.SetIRegion(hit->GetIRegion()); 
	//      cout << "found one" << hit->GetHitID() << endl;
      }
  }
  return cluster;
}

PndTrkCluster PndTrkCluster::GetSttSkewHitList() 
{
  PndTrkCluster cluster;
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkHit *hit =  (PndTrkHit*) hitlist[ihit];
      if(hit->IsSttSkew()) {
	cluster.AddHit(hit);
	cluster.SetIRegion(hit->GetIRegion()); 
	//      cout << "found one" << hit->GetHitID() << endl;
      }
  }
  return cluster;
}

PndTrkCluster PndTrkCluster::GetSttHitList() 
 {
  PndTrkCluster cluster;
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkHit *hit =  (PndTrkHit*) hitlist[ihit];
  if(hit->IsStt()) {
      cluster.AddHit(hit);
      cluster.SetIRegion(hit->GetIRegion()); 
      //      cout << "found one" << hit->GetHitID() << endl;
    }
  }
  return cluster;
}
// =======================================================================


// finds the first instance of the hit in the cluster (there should be only one!)
PndTrkHit* PndTrkCluster::SearchHit(PndTrkHit *hit) {
  return (PndTrkHit*) hitlist.FindObject(hit);
}


// ==================== V SHAPED CLUSTER ===============
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
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkHit *hit1 =  (PndTrkHit*) hitlist[ihit];
     for(int jhit = 0; jhit < GetNofHits(); jhit++) {
       if(ihit == jhit) continue;
       PndTrkHit *hit2 =  (PndTrkHit*) hitlist[jhit];

       if(hit1->GetXYDistance(hit2) < STTPARALDISTANCE) {
	 associations[ihit]++;
       }
     }
 }

  int counter = 0;
  Double_t tmpdistance1 = 1000, tmpdistance2 = -1;

  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    if(associations[ihit] != 1) continue;
    counter++;
    PndTrkHit *hit =  (PndTrkHit*) hitlist[ihit];
    double distance = hit->GetXYDistance(frompoint);
    if(distance < tmpdistance1) {
      tmpdistance1 = distance;
      firstextremity = *hit;
    }
    
    if(distance > tmpdistance2) {
      tmpdistance2 = distance;
      secondextremity = *hit;
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
    cout << "circle " << x0 << " " << y0 << " " << radius << endl;

    int counter = 0;
    for(int ihit = 0; ihit < GetNofHits(); ihit++) {
      PndTrkHit *hit =  (PndTrkHit*) hitlist[ihit];
      if(fabs(  hit->GetXYDistance(TVector3(x0, y0, 0.)) - radius) > 2) counter++;
    }

    if(GetNofHits() - counter < 3) cout << "***************** YES ***************** " << counter << " on " << GetNofHits() << endl;
    else cout << " ********************** NO ******************* " << counter << " " << GetNofHits() << endl;
    return kTRUE;

  }
  //  else cout << " DISTANCE " << fabs(firstextremity->GetPosition().Perp() - secondextremity->GetPosition().Perp()) << endl;

  return kFALSE;
}

// CHECK if it works now
Bool_t PndTrkCluster::SplitAtHit(PndTrkHit *athit, PndTrkCluster &cluster1, PndTrkCluster &cluster2) {

  PndTrkHit *tmphit =  (PndTrkHit*) hitlist[0];

  cluster1.AddHit(athit);
  cluster2.AddHit(athit);
  TVector3 tmpposition = athit->GetPosition();
  PndTrkHit *secondhit = NULL;
 
  cout << "CLUS1: " << tmphit->GetHitID() << endl;
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkHit *hit =  (PndTrkHit*) hitlist[ihit];
    if(*hit == *athit) {
      cout << "hit == ahit " << endl;
      continue;
    }
    cout << "*** " << hit->GetHitID() << " " << hit->GetXYDistance(tmpposition) << " " << STTPARALDISTANCE << endl; 
    if(hit->GetXYDistance(tmpposition) < STTPARALDISTANCE) { // CHECK
      cluster1.AddHit(hit);
      tmpposition = hit->GetPosition();
      if(secondhit == NULL) {
	cout << "secondhit " << endl;
	secondhit = hit;
      }
      else cout << "sechit " << secondhit << endl;
      cout << "CLUS1: " << hit->GetHitID() << endl;

    }
  }
  
  cout << "CLUS2: " << tmphit->GetHitID() << endl;

  tmpposition = athit->GetPosition();
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkHit *hit =  (PndTrkHit*) hitlist[ihit];
    cout << "athit " << athit << endl;
    cout << "hit " << hit << endl;
    cout << "secondhit " << secondhit <<  endl;
    if(*hit == *athit) continue;
    if(*hit == *secondhit) continue;

    if(hit->GetXYDistance(tmpposition) < STTPARALDISTANCE) { // CHECK
      cluster2.AddHit(hit);
      tmpposition = hit->GetPosition();
      cout << "CLUS2: " << hit->GetHitID() << endl;
    }
  }
  

}
// CHECK??
// splits the V shaped cluster at the point athit
// ----------------------------------------------
// starts from one extremity and stops @ athit,
// the same for the other extremity.
Bool_t PndTrkCluster::SplitV(PndTrkHit *athit, PndTrkHit *firstextremity, PndTrkHit* secondextremity, PndTrkCluster &cluster1, PndTrkCluster &cluster2) {
  //  cout << "splitV" << endl;
  

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
    for(int ihit = 0; ihit < GetNofHits(); ihit++) {
      hit =  (PndTrkHit*) hitlist[ihit];
      hit->SetUsedFlag(kFALSE);
    }


    hit = NULL;
 
    if(iclus == 1) {
      tmpposition = secondextremity->GetPosition();
      tmphit = secondextremity;
    }

    double tmpdistance2 = 1000;
    while(!hit || (hit && (hit->GetPosition() - athit->GetPosition()).Perp() > STTPARALDISTANCE)) {
      //     if(!hit) cout << "here!" << endl;
      //       if(hit) cout << "here " << (hit->GetPosition() - athit->GetPosition()).Perp() << endl;
      for(int ihit = 0; ihit < GetNofHits(); ihit++) {
	hit =  (PndTrkHit*) hitlist[ihit];

	//	if(hit->IsUsed()) continue;
	//	cout << "test " << hit->GetHitID() << " " << hit->GetDistance(tmpposition) << endl;
	cout << (*hit == *firstextremity) << " " << (*hit == *secondextremity) << " " << (*hit == *tmphit) << endl;

	// CHECK SearchHit
	if((iclus == 0 && cluster1.SearchHit(hit) != 0) || (iclus == 1 && cluster2.SearchHit(hit) != 0)) {
	  cout << hit->GetHitID() << " is already there .................." << endl;
	  continue;
	}
	if(*hit == *firstextremity || *hit == *secondextremity || *hit == *tmphit) continue;

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
      }
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


void PndTrkCluster::SortFromHit(PndTrkHit *firstextremity, TString criterion) {

  //  cout << "------------------------------------ sorting" << endl;
  fFromPoint = firstextremity->GetPosition();
  PndTrkHit *hit;
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    hit = (PndTrkHit*) hitlist[ihit];

    if(criterion.CompareTo("xydistance") == 0) {      // CHECK 
      double distance = hit->GetDistance(fFromPoint);
      //   cout << "distance " << distance << endl;
      hit->SetSortVariable(distance);
    }

  }
  // sorting 
  Sort() ; 
}

void PndTrkCluster::Sort()
{
  // CHECK each TObject must return IsSortable = kTRUE

  hitlist.Sort(); 
}

void PndTrkCluster::ReverseSort()
{
  // CHECK each TObject must return IsSortable = kTRUE
  PndTrkHit *hit;
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    hit = (PndTrkHit* ) hitlist[ihit];
    int srt = hit->GetSortVariable();
    hit->SetSortVariable(-srt);
  }
  hitlist.Sort(); 
}
// =======================================================================================
// MERGE CLUSTERS

void PndTrkCluster::AddCluster(PndTrkCluster *cluster) {
  for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    AddHit(hit);
  }
  
  TString iregion1;
  iregion1 += fIRegion;
  TString iregion2;
  iregion2 += cluster->GetIRegion();
}

void  PndTrkCluster::AddClusterAndSortFrom(PndTrkCluster *cluster, TVector3 frompoint, TString criterion) {

  fFromPoint = frompoint;
  PndTrkHit *hit;

  for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    hit = cluster->GetHit(ihit);
    AddHit(hit);
  }
  
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    hit = (PndTrkHit* ) hitlist[ihit];
    if(criterion.CompareTo("xydistance") == 0) {      // CHECK 
      double distance = hit->GetDistance(fFromPoint);
      //   cout << "distance " << distance << endl;
      hit->SetSortVariable(distance);
    }
  }
  // sorting
  Sort();
  

}


// =======================================================================================
PndTrkHit * PndTrkCluster::GetPocaTo(TVector3 frompoint) {
  //  cout << "getPocaTo" << endl;
  Double_t tmpdistance = 1000;
  
  PndTrkHit *tmphit = (PndTrkHit* ) hitlist[0];

  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkHit *hit = (PndTrkHit* ) hitlist[ihit];

    double distance = hit->GetXYDistance(frompoint);
    if(distance < tmpdistance) {
      tmpdistance = distance;
      tmphit = hit;
    }
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
Bool_t PndTrkCluster::DoesContain(PndTrkHit *hit) {
  bool isthere = false;
  if(hitlist.FindObject(hit) == 0) return kFALSE;
  return kTRUE;  

}


void PndTrkCluster::Replace(PndTrkHit *hit) {
   PndTrkHit *oldhit = (PndTrkHit*) SearchHit(hit);
   //   cout << "oldhit " << oldhit << " " << hit << endl;
   DeleteHitAndCompress(oldhit);
   AddHit(hit);
}

PndTrkHit *PndTrkCluster::GetHit(int index) {
  //  cout << index << " hitlist " << hitlist.GetEntriesFast() << " " << hitlist.GetEntries() << endl;
  //  cout << hitlist.At(index) << endl;

  return (PndTrkHit*) hitlist.At(index);
}

PndTrkHit *PndTrkCluster::GetPreviousHit(int index) {
  if(index == 0) return NULL;
  return (PndTrkHit*) hitlist[index - 1];
}

PndTrkHit *PndTrkCluster::GetNextHit(int index) {
  if(index == GetNofHits() - 1) return NULL;
  return (PndTrkHit*) hitlist[index + 1];
}


Bool_t PndTrkCluster::IsSimilarTo(PndTrkCluster *cluster2) {
  int similarity = 0;
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkHit *hit = (PndTrkHit*) hitlist[ihit];
    if(cluster2->DoesContain(hit)) similarity++;
  }
  if(((double) similarity/GetNofHits()) > 0.5 || ((double) similarity/cluster2->GetNofHits()) > 0.5) return kTRUE;
  return kFALSE;
}


int PndTrkCluster::MergeTo(PndTrkCluster *cluster2) {
  
  std::vector<int> tobeadded;
  for(int ihit = 0; ihit < cluster2->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster2->GetHit(ihit);
    if(DoesContain(hit)) continue;
    tobeadded.push_back(ihit);
  }
  
  for(int ihit = 0; ihit < tobeadded.size(); ihit++) {
    int hitno = tobeadded.at(ihit);
    PndTrkHit *hit = cluster2->GetHit(hitno);
    AddHit(hit);
  }
  return GetNofHits();
}




Double_t  PndTrkCluster::GetMinimumXYDistanceFromHit(PndTrkHit *hit) {
  double distance = 1000000;
 //  cout << " from hit " << hit->GetHitID() << " " << hit->GetDetectorID() << endl;

  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkHit *clushit = GetHit(ihit);
    double tmpdistance = hit->GetXYDistance(clushit);
    if(tmpdistance < distance) {
      distance = tmpdistance;
      //      cout << " new minimum " << clushit->GetHitID() << " " << clushit->GetDetectorID() << endl;
    }
  }
  return distance;
}


// PndTrkCluster PndTrkCluster::MergeTo(PndTrkCluster cluster2) {
//   std::vector<int> tobeadded;
//   for(int ihit = 0; ihit < GetNofHits(); ihit++) {
//     PndTrkHit *hit = (PndTrkHit*) hitlist[ihit];
//     if(cluster2.DoesContain(hit)) continue;
//     tobeadded.push_back(ihit);
//   }

//   for(int ihit = 0; ihit < tobeadded.size(); ihit++) {
//     int hitno = tobeadded.at(ihit);
//     cluster2.AddHit((PndTrkHit*) hitlist[hitno]);
//   }
//   return cluster2;

// }

void PndTrkCluster::Print() {
  cout << "###############################" << endl;
  cout << "iregion " << fIRegion << " with nhits " << GetNofHits() << endl;
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkHit *hit = (PndTrkHit*) hitlist[ihit];
    cout << " " <<  hit->GetHitID() << " " << hit->GetDetectorID() << " " << hit->GetSortVariable() << endl;
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
  for(int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkHit *hit = (PndTrkHit*) hitlist[ihit];
    hit->Draw(color);
  }
}

void PndTrkCluster::LightUp() { 
  Draw(kOrange); // Yellow);
}



ClassImp(PndTrkCluster)
 
