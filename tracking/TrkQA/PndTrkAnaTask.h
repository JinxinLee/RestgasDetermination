#ifndef PNDTRKANATASK_H
#define PNDTRKANATASK_H

// fairroot
#include "FairTask.h"

#include "TClonesArray.h"
#include "PndGeoSttPar.h"
#include "PndTrkMCTrackInfo.h"
#include "PndTrkRecoTrackInfo.h"
#include "PndTrack.h"
#include "PndTrackID.h"
#include "PndAnalysis.h"
#include "TH2F.h"
#include "PndVtxPRG.h"
#include "Pnd4CFitter.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;

class RhoMassParticleSelector;
class PndAnalysis;
class RhoCandList;
class RhoTuple;

class PndTrkAnaTask : public FairTask {

 public:

  /** Default constructor **/
  PndTrkAnaTask();
  PndTrkAnaTask(TString detectorsToStudy, Int_t verbose);


  /** Destructor **/
  ~PndTrkAnaTask();


  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  void SetParContainers();

  void SetInputTrackBranchName(char* track) {sprintf(fTrackBranch,"%s", track);}
  void SetInputTrackIDBranchName(char* trackid) {sprintf(fTrackIDBranch,"%s", trackid);}
  
  void SetVerbosity(bool verbose) { fVerbose = verbose; }
  int SelectTruePid(PndAnalysis *ana, RhoCandList &l);
  void Finish();

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


  PndAnalysis *fAnalysis;
  TLorentzVector *fIni;


  // **** create and setup some histos for QA plots
  //
  TH2F *hlvpos, *hlbvpos, *hOvpos, *hObvpos, *hp_pimp, *hp_pibar, *hp_rpimp, *hp_rpibar, *hp_thpip, *hp_thpim, *hp_thp, *hp_thpbar;
 

    TH1F *lambdamass , *lambdamassv, *lambdacmass, *lambdabmass, *lbmassv , *lambdabcmass, *l4C_chi2, *hmv_lamb, *hmv_lam, *hlam_prob_vf, *hlamb_prob_vf, *hlvposz , *hlbvposz, *h0_chi2_vf, *h0b_chi2_vf, *hnfw, *hpip_p , *hpim_p , *hp_p  , *hv_lam_diff, *hv_lam_diff1, *hv_lamb_diff, *hv_lamb_diff1, *hpbar_p , *hlamb_diff, *hlamb_diff_vf, *hlam_diff , *hlam_diff_vf, *hpip_diff , *hpim_diff, *hp_diff, *hpbar_diff, *hp_stt, *hp_sttmvd, *hp_sttmvdgem, *hp_mvdgem, *hpip_diff_vf, *hpim_diff_vf, *hp_diff_vf , *hpbar_diff_vf, *hpip_diff_4c, *hpim_diff_4c, *hp_diff_4c, *hpbar_diff_4c, *hl_cos, *hlb_cos, *hl_cosr , *hlb_cosr;
 
  ClassDef(PndTrkAnaTask,1);
};

#endif
