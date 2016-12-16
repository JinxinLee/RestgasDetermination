
/** PndMvdIdealTrackFinderTask.h
 *@author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Ideal cluster finding task
 */


#ifndef PNDMVDIDEALTRACKFINDERTASK_H
#define PNDMVDIDEALTRACKFINDERTASK_H

#include "FairTask.h"
#include "PndMCTrack.h"
//#include "PndMvdGeoPar.h"
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"
//#include "PndMvdPixel.h"
#include "PndSdsCluster.h"
#include "PndTrackCand.h"
//#include "TrackCand.h"

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
    PndMvdIdealTrackFinderTask(const PndMvdIdealTrackFinderTask& o) :
      fHitBranchStrip(o.fHitBranchStrip),
      fHitBranchPixel(o.fHitBranchPixel),
      fClusterBranchStrip(o.fClusterBranchStrip),
      fClusterBranchPixel(o.fClusterBranchPixel),
      fDigiBranchStrip(o.fDigiBranchStrip),
      fDigiBranchPixel(o.fDigiBranchPixel),
      fMcBranch(o.fMcBranch),
      fTrackBranch(o.fTrackBranch),
      fStripHitArray(o.fStripHitArray),
      fPixelHitArray(o.fPixelHitArray),
      fStripClusterArray(o.fStripClusterArray),
      fPixelClusterArray(o.fPixelClusterArray),
      fStripDigiArray(o.fStripDigiArray),
      fPixelDigiArray(o.fPixelDigiArray),
      fMcArray(o.fMcArray),
      fTrackArray(o.fTrackArray),
      fTrackCandArray(o.fTrackCandArray),
      fTrackCandMap(o.fTrackCandMap)
   {};
    PndMvdIdealTrackFinderTask& operator=(const PndMvdIdealTrackFinderTask& o)
    {
      fHitBranchStrip=o.fHitBranchStrip;
      fHitBranchPixel=o.fHitBranchPixel;
      fClusterBranchStrip=o.fClusterBranchStrip;
      fClusterBranchPixel=o.fClusterBranchPixel;
      fDigiBranchStrip=o.fDigiBranchStrip;
      fDigiBranchPixel=o.fDigiBranchPixel;
      fMcBranch=o.fMcBranch;
      fTrackBranch=o.fTrackBranch;
      fStripHitArray=o.fStripHitArray;
      fPixelHitArray=o.fPixelHitArray;
      fStripClusterArray=o.fStripClusterArray;
      fPixelClusterArray=o.fPixelClusterArray;
      fStripDigiArray=o.fStripDigiArray;
      fPixelDigiArray=o.fPixelDigiArray;
      fMcArray=o.fMcArray;
      fTrackArray=o.fTrackArray;
      fTrackCandArray=o.fTrackCandArray;
      fTrackCandMap=o.fTrackCandMap;
    return *this;
    };


    /** Virtual method Init **/
    virtual void SetParContainers();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    void PrintResult();
    void SetVerbose(Int_t verbose){ fVerbose = verbose;};


 private:

//    std::vector<Int_t> GetHitPerCluster(PndSdsCluster* clusterCand);
   void ClearTrackCandMap();
   void AddAndExpand(Int_t trackID, Int_t detnum, Int_t iHit,PndSdsHit* theHit);
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

    /** Input array of PndSdsDigis **/
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
      std::map<Int_t, PndTrackCand*> fTrackCandMap;


  void Register();
  void Reset();
  void ProduceHits();


  ClassDef(PndMvdIdealTrackFinderTask,2);

};

#endif /* PndMvdIdealTrackFinderTASK_H */

