// -------------------------------------------------------------------------
// -----              PndLmdDigiQTask header file            -----
// -----            Created 19/09/14  by A.Karavdina          -----
// -------------------------------------------------------------------------

/** PndLmdDigiQTask.h
 *@author A.Karavdina <karavdin@kph.uni-mainz.de>
 **
 ** Digi hits separation on signal and noise
 **/


#ifndef PNDLMDDIGIQTASK_H
#define PNDLMDDIGIQTASK_H


// framework includes
#include "FairTask.h"
#include "TH1.h"
#include "TMatrixD.h"
#include "TVector3.h"
#include "PndGeoHandling.h"
#include <vector>
#include <map>
#include "TDatabasePDG.h"
#include "LmdQA/PndLmdDigiQ.h"
#include "PndLmdDim.h"

class TClonesArray;
class PndSdsCluster;


class PndLmdDigiQTask : public FairTask
{
 public:

  /** Default constructor **/  
  //  PndLmdDigiQTask();
  PndLmdDigiQTask(const PndLmdDigiQTask& ) = delete;
  PndLmdDigiQTask(TString digiBranch="LMDPixelDigis");

  /** Destructor **/
  ~PndLmdDigiQTask();


  /** Virtual method Init **/
  //  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
  virtual void Finish();
  void SetWriteMC(bool wr){fWriteAllMC = wr;}
 private:

  // Input Data ----------
  TString fDigiName;
  TClonesArray* fMCHits;
  TClonesArray* fMCTracks;
  TClonesArray* fDigis;
  /* TClonesArray* fClusters; */
  /* TClonesArray* fRecHits; */
  /* TClonesArray* fRecCandTracks; */
  /* TClonesArray* fRecTracks; */
  /* TClonesArray* fRecBPTracks; */
  /* TClonesArray* fMCHeader; */
  // Output Data -------
  TClonesArray*  fDigiQ;		
 
  int fEventNr;
// bool fUseMVDPoint;
  // Double_t fPbeam;
  // Int_t fPDGid;
  // TVector3 vtx;
  bool fWriteAllMC;
  PndLmdDim* lmddim;
  //  TDatabasePDG *fdbPDG;
  

  ClassDef(PndLmdDigiQTask,1);

};

#endif
