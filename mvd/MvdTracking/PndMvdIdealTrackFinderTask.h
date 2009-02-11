
/** PndMvdIdealTrackFinderTask.h
 *@author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Ideal cluster finding task
 */


#ifndef PNDMVDIDEALTRACKFINDERTASK_H
#define PNDMVDIDEALTRACKFINDERTASK_H

#include "FairTask.h"
#include "PndMCTrack.h"
#include "PndMvdGeoPar.h"
#include "PndMvdHit.h"
#include "PndMvdMCPoint.h"
#include "PndMvdPixel.h"
#include "PndMvdCluster.h"
#include "TrackCand.h"

#include <string>
#include <vector>

class TClonesArray;

class PndMvdIdealTrackFinderTask : public FairTask
{
 public:

    /** Default constructor **/
    PndMvdIdealTrackFinderTask();
    /** Destructor **/
    virtual ~PndMvdIdealTrackFinderTask();


    /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    void PrintResult();
    void SetVerbose(Int_t verbose){ fVerbose = verbose;};


 private:

//    std::vector<Int_t> GetHitPerCluster(PndMvdCluster* clusterCand);
   void ClearTrackCandMap();
   void AddAndExpand(Int_t trackID, Int_t detnum, Int_t iHit);
   Double_t GetTrackDip(PndMCTrack* myTrack);
   Double_t GetTrackCurvature(PndMCTrack* myTrack);

   TString fHitBranchStrip;
   TString fHitBranchPixel;
   TString fClusterBranchStrip;
   TString fClusterBranchPixel;
   TString fDigiBranchStrip;
   TString fDigiBranchPixel;
   TString fMcBranch;
   TString fTrackBranch;

    /** Input array of PndMvdDigis **/
     TClonesArray* fStripHitArray;
     TClonesArray* fPixelHitArray;
     TClonesArray* fStripClusterArray;
     TClonesArray* fPixelClusterArray;
     TClonesArray* fStripDigiArray;
     TClonesArray* fPixelDigiArray;
     TClonesArray* fMcArray;
     TClonesArray* fTrackArray;

  /** Output array of PndMvdHits **/
      TClonesArray* fTrackCandArray;
      std::map<Int_t, TrackCand*> fTrackCandMap;


  void Register();
  void Reset();
  void ProduceHits();


  ClassDef(PndMvdIdealTrackFinderTask,2);

};

#endif /* PndMvdIdealTrackFinderTASK_H */

