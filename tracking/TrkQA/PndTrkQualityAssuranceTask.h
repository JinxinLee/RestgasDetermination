#ifndef PNDTRKQUALITYASSURANCETASK_H
#define PNDTRKQUALITYASSURANCETASK_H

// fairroot
#include "FairTask.h"

#include "TClonesArray.h"
#include "PndGeoSttPar.h"
#include "PndTrkMCTrackInfo.h"
#include "PndTrkRecoTrackInfo.h"
#include "PndTrack.h"
#include "PndTrackID.h"


class PndTrkQualityAssuranceTask : public FairTask {

 public:

  /** Default constructor **/
  PndTrkQualityAssuranceTask();
  PndTrkQualityAssuranceTask(TString detectorsToStudy, Int_t verbose);


  /** Destructor **/
  ~PndTrkQualityAssuranceTask();


  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  void SetParContainers();

  void SetInputTrackBranchName(char* track) {sprintf(fTrackBranch,"%s", track);}
  void SetInputTrackIDBranchName(char* trackid) {sprintf(fTrackIDBranch,"%s", trackid);}
  
  PndTrkMCTrackInfo GetMCInfoFromIdealTrack(PndTrack *idealtrack, PndTrackID *idealtrackid);
  PndTrkRecoTrackInfo GetRecoInfoFromRecoTrack(int recotrackid, PndTrack *track, PndTrackID *trackid);
  
  Bool_t  Reconstructability(int nofmvdpixpoint, int nofmvdstrpoint, int nofsttparalpoint, int nofsttskewpoint, int nofgempoint, int nofscitilpoint);
  


  void WriteHistograms();

  void SetVerbosity(bool verbose) { fVerbose = verbose; }

 private:

  Int_t fEventCounter;

  /** Input array of PndFtsPoints **/
  TClonesArray* fFtsPointArray;
  /** Input array of PndFtsHit **/
  TClonesArray* fFtsHitArray;

  /** Input array of PndSttPoints **/
  TClonesArray* fSttPointArray;
  /** Input array of PndSttHit **/
  TClonesArray* fSttHitArray;

  TClonesArray* fMvdPointArray;
  /** Input array of MvdPixelHitArray **/
  TClonesArray* fMvdPixelHitArray;
  /** Input array of MvdStripHitArray **/
  TClonesArray* fMvdStripHitArray;

  /** Input array of PndGemPoints **/
  TClonesArray* fGemPointArray;
  /** Input array of PndGemHit **/
  TClonesArray* fGemHitArray;

  /** Input array of PndSciTPoints **/
  TClonesArray* fSciTilPointArray;
  /** Input array of PndSciTHit **/
  TClonesArray* fSciTilHitArray;


  TClonesArray* fMCTrackArray, * fTrackArray, * fTrackIDArray, * fTrackCandArray, *fIdealTrackArray, *fIdealTrackIDArray;
  TClonesArray* fSttTubeArray;

  TClonesArray* fMCTrackInfo, *fRecoTrackInfo;
 
  PndGeoSttPar *fSttParameters;  //  CHECK added
 
  char	fSttBranch[200],
    fMvdPixelBranch[200],
    fMvdStripBranch[200],
    fTrackBranch[200],
    fTrackIDBranch[200],
    fIdealTrackBranch[200],
    fIdealTrackIDBranch[200],
    fGemBranch[200],
    fSciTBranch[200]    ;

  Bool_t fPersistence;





 
  ClassDef(PndTrkQualityAssuranceTask,1);
};

#endif
