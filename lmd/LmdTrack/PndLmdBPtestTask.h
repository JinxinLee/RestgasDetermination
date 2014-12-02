// -------------------------------------------------------------------------
// -----                   PndLmdBPtestTask header file                 -----
// -----                  Created 20/03/07  by R.Kliemt                -----
// -----                  modified for Lmd by M. Michel & A.Karavdina  -----
// -------------------------------------------------------------------------


/** PndLmdBPtestTask.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef PndLmdBPtestTask_H
#define PndLmdBPtestTask_H


// framework includes
#include "FairTask.h"
#include "TH1.h"
//#include "GFTrackCand.h"
#include "FairGeanePro.h"
#include "TMatrixD.h"
#include "TVector3.h"
#include "TNtuple.h"
#include "FairTrackParP.h"
//#include "PndSdsGeoHandling.h"
#include "PndGeoHandling.h"
#include <vector>
#include <map>
#include "PndMultiField.h"
class TClonesArray;
class PndSdsCluster;
//class TGeant3;//GEANE uses Geant3!
//class TGeant4;

class PndLmdBPtestTask : public FairTask
{
 public:

  /** Default constructor **/  
  PndLmdBPtestTask();

  ///Set up beam momuntum value and position of interaction point
  PndLmdBPtestTask(Double_t pBeam, TVector3 IP);

  /** Destructor **/
  ~PndLmdBPtestTask();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
  virtual void Finish();

  FairTrackParP* PropToPlane(FairTrackParP* fStartPst, double zpos,int dir,bool& isProp);//propagation between planes
 private:
  TClonesArray* fTrackParGeane;
  TClonesArray* fTrackParIni;
  TClonesArray* fTrackParFinal;
  TClonesArray* fDetName;
		
  FairGeanePro* fPro;
  PndGeoHandling* fGeoH;
  int fEventNr;
  bool fUseMVDPoint;
  Double_t fPbeam;
  Int_t fPDGid;
  TVector3 vtx;
  FairField *pndField;		
  std::map<int, std::vector<int> > fTrackPixHitIdMap;	 //Track -> PixHitId
  std::map<int, std::vector<int> > fTrackStripHitIdMap;	 //Track -> StripHitId

  // Input Data ----------
  TClonesArray* fMCHits;
  TClonesArray* fMCTracks;

  TClonesArray* fTracks; //Real tracks
  TClonesArray* fHits; //Real hits


  TClonesArray* fRecHits;   //needed for tests only!
  TClonesArray* fRecCandTracks;   //needed for tests only!


  // Output Data -------
  // TEST with forward-backward propagation ----------------------------
  TTree *tprop;//Info for forward-backward propagation
  double fxrec, fyrec, fzrec, fprec, fthetarec, fphirec, fxmc, fymc, fzmc, fpmc, fthetamc, fphimc, fxmclmd, fymclmd, fzmclmd, fpmclmd, fthetamclmd, fphimclmd;
  double fxrec_err, fyrec_err,fxmc_err, fymc_err,  fxmclmd_err, fymclmd_err;
  double fbx,fby,fbz;
  double fvrec,fwrec,ftvrec,ftwrec,fvrec_err,fwrec_err,ftvrec_err,ftwrec_err;
  double fvmc,fwmc,ftvmc,ftwmc,fvmc_err,fwmc_err,ftvmc_err,ftwmc_err;
  double fvmclmd,fwmclmd,ftvmclmd,ftwmclmd,fvmclmd_err,fwmclmd_err,ftvmclmd_err,ftwmclmd_err;

  //----------------------------------------

  void Register();
  
  void Reset();
  
  void ProduceHits();
  
  std::map<int, std::vector<int> > AssignHitsToTracks();
  ClassDef(PndLmdBPtestTask,1);

};

#endif
