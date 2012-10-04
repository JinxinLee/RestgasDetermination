// -------------------------------------------------------------------------
// -----                      PndLmdTrackFinderTask                    -----
// -----                  Created 22/10/09  by M. Michel               -----
// -------------------------------------------------------------------------


#ifndef PNDLMDTRACKFINDERCATASK_H
#define PNDLMDTRACKFINDERCATASK_H
#include "FairTask.h"
#include "PndMCTrack.h"
#include "PndSdsGeoPar.h"
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"
#include "PndSdsClusterStrip.h"
#include "TrackData/PndTrackCand.h"

#include <string>
#include <vector>

class TClonesArray;

class PndLmdTrackFinderCATask : public FairTask
{
 public:

    /** Default constructor **/
    PndLmdTrackFinderCATask();
    /** Constructor **/
    PndLmdTrackFinderCATask(const bool missPl, const double setdmax, Int_t innSensPP=8, Int_t innP=4, TString hitBranch="LMDHitsStrip", TString clusterBranch="LMDStripClusterCand", TString digiBranch="LMDStripDigis");
    /** Destructor **/
    virtual ~PndLmdTrackFinderCATask();


    /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    void SetVerbose(Int_t verbose){ fVerbose = verbose; };
    void SetInaccuracy(Double_t accu) { dXY = accu; };
    

 private:
   Double_t dXY;
   double d_max;
   int nSensPP;//number of sensors on one plane
   int nP;//number of planes
   bool SortHitsByDet(std::vector< std::vector< std::pair<Int_t,bool> > > &hitsd, Int_t nStripHits);

   bool SortHitsByDetSimple(std::vector< std::vector< Int_t > > &hitsd, Int_t nStripHits);
   bool SortHitsByZ(std::vector< std::vector< std::pair<Int_t,bool> > > &hitsd, Int_t nStripHits);
//    std::vector<Int_t> GetHitPerCluster(PndSdsClusterStrip* clusterCand);
   Double_t GetTrackDip(PndMCTrack* myTrack);
   Double_t GetTrackCurvature(PndMCTrack* myTrack);

   TString fHitBranchStrip;
   TString fClusterBranchStrip;
   TString fDigiBranchStrip;
   bool missPlAlgo;


    /** Input array of PndSdsDigis **/
     TClonesArray* fStripHitArray;
     TClonesArray* fStripClusterArray;
     TClonesArray* fStripDigiArray;

     /** Output array of PndSdsHits **/
     TClonesArray* fTrackCandArray;
     //    TClonesArray* fTrackCandArrayTemp; //Template 
     void Register();
     void Reset();
     void ProduceHits();
     TH1D *hdist;


  ClassDef(PndLmdTrackFinderCATask,2);

};

#endif /* PndLmdTrackFinderCATASK_H */

