/** PndTrkCluster
 ** 
 ** @author Lia Lavezzi 
 **/

#ifndef PNDTRKCLUSTER_H
#define PNDTRKCLUSTER_H 1

#include "TVector3.h"
#include "PndTrkHit.h"
#include "PndTrkHitList.h"
#include "FairHit.h"

#define MAXNOFHITSINCLUSTER 1000  // CHECK consistency

class PndTrkCluster : public TObject
{
  
 public:    

  PndTrkCluster();
  ~PndTrkCluster();    


  Bool_t operator==(const PndTrkCluster cluster) const; // CHECK this needs to be changed
  //  PndTrkCluster(const PndTrkCluster& cluster);

  void AddHit(PndTrkHit *hit);
  /*   void DeleteHit(PndTrkHit *hit); */
  void DeleteHit(Int_t index);
  void DeleteAllHits();
  
  void SetIRegion(int iregion) { fIRegion = iregion; }

  PndTrkHit *GetHit(int index);

  inline Int_t  GetNofHits() { return hitlist.size(); }
  inline Int_t  GetIRegion() { return fIRegion; }
  
  Int_t SearchHit(PndTrkHit *hit);
  /*   PndTrkHit SearchHit(Int_t hitID, Int_t detID); */
  /*   void SortCluster(); */
  /*   void MergeWithCluster(PndTrkCluster cluster2); */
  /*   void AccessList(); */
 
  /*   void SortByDistance(TVector3 startpoint); */

  /*   bool DistanceSorting(PndTrkHit &hit1, PndTrkHit &hit2); */
  void Print();
  void PrintList();
  void Draw(Color_t color = kBlack);
  void LightUp();

  Bool_t DoesContain(PndTrkHit *hit);
  Bool_t IsSimilarTo(PndTrkCluster cluster2);
  PndTrkCluster MergeTo(PndTrkCluster cluster2);

  PndTrkCluster GetMvdStripHitList();
  PndTrkCluster GetMvdPixelHitList();
  PndTrkCluster GetMvdHitList();
  
  PndTrkCluster GetSttParallelHitList();
  PndTrkCluster GetSttSkewHitList();
  PndTrkCluster GetSttHitList();
  
  Bool_t FindExtremitiesFrom(TVector3 frompoint, PndTrkHit &firstextremity, PndTrkHit &secondextremity);
  Bool_t CheckClusterAgainsV(TVector3 frompoint, PndTrkHit *firstextremity, PndTrkHit *secondextremity);

  Bool_t ComputeCircle(TVector3 v1, TVector3 v2, TVector3 v3, double &x0, double &y0, double &R);
  PndTrkHit * GetPocaTo(TVector3 frompoint);
  Bool_t  SplitAtHit(PndTrkHit *athit, PndTrkCluster &cluster1, PndTrkCluster &cluster2);
  Bool_t SplitV(PndTrkHit *athit, PndTrkHit *firstextremity, PndTrkHit* secondextremity, PndTrkCluster &cluster1, PndTrkCluster &cluster2);

 
  void SortFromHit(PndTrkHit *firstextremity, TString criterion);
  friend Bool_t SorterFunction(PndTrkHit *hit1, PndTrkHit *hit2); 
  void Sort();

 void AddCluster(PndTrkCluster *cluster);
 void AddClusterAndSortFrom(PndTrkCluster *cluster, TVector3 frompoint, TString criterion);

 protected:
    TVector3 fFromPoint;
    Int_t fIRegion;
    std::vector< PndTrkHit * > hitlist;

  ClassDef(PndTrkCluster,1);
};


#endif
