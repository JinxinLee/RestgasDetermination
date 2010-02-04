// -------------------------------------------------------------------------
// -----                      PndLmdTrackFinderTask                    -----
// -----                  Created 22/10/09  by M. Michel               -----
// -------------------------------------------------------------------------


#ifndef PNDLMDTRACKFINDERTASK_H
#define PNDLMDTRACKFINDERTASK_H

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

class PndLmdTrackFinderTask : public FairTask
{
 public:

    /** Default constructor **/
    PndLmdTrackFinderTask();
    /** Destructor **/
    virtual ~PndLmdTrackFinderTask();


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

//    std::vector<Int_t> GetHitPerCluster(PndSdsClusterStrip* clusterCand);
   Double_t GetTrackDip(PndMCTrack* myTrack);
   Double_t GetTrackCurvature(PndMCTrack* myTrack);

   TString fHitBranchStrip;
   TString fClusterBranchStrip;
   TString fDigiBranchStrip;

    /** Input array of PndSdsDigis **/
     TClonesArray* fStripHitArray;
     TClonesArray* fStripClusterArray;
     TClonesArray* fStripDigiArray;

  /** Output array of PndSdsHits **/
      TClonesArray* fTrackCandArray;


  void Register();
  void Reset();
  void ProduceHits();


  ClassDef(PndLmdTrackFinderTask,2);

};

#endif /* PndLmdTrackFinderTASK_H */

