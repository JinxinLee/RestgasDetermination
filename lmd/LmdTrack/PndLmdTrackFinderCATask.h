// -------------------------------------------------------------------------
// -----            PndLmdTrackFinderCATask                    -----
// -----       Created 18/05/11  by A. Karavdina              -----
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
#include "PndLmdDim.h"
#include <string>
#include <vector>
#include "TH2.h"
#include "TNtuple.h"
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
    void SetSensStripFlag(bool fS){ flagStipSens = fS; };    
    void SetSensPixelFlag(bool fS){ flagPixelSens = fS; }; 
    void SetTrkCandCutsFlag(bool fS){ flagTrkCandCuts = fS;};
    void FinishTask();
 private:
   Double_t dXY;
   double rule_max;
   int nSensPP;//number of sensors on one plane
   int nP;//number of planes
   bool SortHitsByDet(std::vector< std::vector< std::pair<Int_t,bool> > > &hitsd, Int_t nStripHits);
   bool SortHitsByDetSimple(std::vector< std::vector< Int_t > > &hitsd, Int_t nStripHits);
   bool SortHitsByDetSimple2(std::vector< std::vector< Int_t > > &hitsd, Int_t nStripHits);//Uses PmdLmdDim class
   bool SortHitsByZ(std::vector< std::vector< std::pair<Int_t,bool> > > &hitsd, Int_t nStripHits);
   Double_t GetTrackDip(PndMCTrack* myTrack);
   Double_t GetTrackCurvature(PndMCTrack* myTrack);

   TString fHitBranchStrip;
   TString fClusterBranchStrip;
   TString fDigiBranchStrip;
   bool missPlAlgo;
   bool flagStipSens;
   bool flagPixelSens;
   bool flagTrkCandCuts;
    /** Input array of PndSdsDigis and PndSdsHits**/
     TClonesArray* fStripHitArray;
     TClonesArray* fStripClusterArray;
     TClonesArray* fStripDigiArray;

     /** Output array of Track-Cand **/
     TClonesArray* fTrackCandArray;

     TClonesArray* fCellArray;
     TClonesArray* fCellArray_tmp;
     void Register();
     void Reset();
     void ProduceHits();
  
     TString mtxpath, mtx_perfect;
     PndLmdDim* lmddim;

  ClassDef(PndLmdTrackFinderCATask,2);

};

#endif /* PndLmdTrackFinderCATASK_H */

