// -------------------------------------------------------------------------
// -----              PndLmdTrkQTask header file            -----
// -----            Created 18/06/13  by A.Karavdina          -----
// -------------------------------------------------------------------------

/** PndLmdTrkQTask.h
 *@author A.Karavdina <karavdin@kph.uni-mainz.de>
 **
 ** Assigns reconstructed track to simulated one, checks good/ghost/missed
 **/


#ifndef PNDLMDTRKQTASK_H
#define PNDLMDTRKQTASK_H


// framework includes
#include "FairTask.h"
#include "TH1.h"
#include "TMatrixD.h"
#include "TVector3.h"
#include "PndGeoHandling.h"
#include <vector>
#include <map>
#include "TDatabasePDG.h"
#include "LmdQA/PndLmdTrackQ.h"

class TClonesArray;
class PndSdsCluster;


class PndLmdTrkQTask : public FairTask
{
 public:

  /** Default constructor **/  
  PndLmdTrkQTask();

  ///Set up beam momuntum value
  PndLmdTrkQTask(Double_t pBeam);

  /** Destructor **/
  ~PndLmdTrkQTask();


  /** Virtual method Init **/
  //  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
  virtual void Finish();
  void SetWriteMC(bool wr){fWriteAllMC = wr;}
 private:

  // Input Data ----------
  TClonesArray* fMCHits;
  TClonesArray* fMCTracks;
  TClonesArray* fDigis;
  TClonesArray* fClusters;
  TClonesArray* fRecHits;
  TClonesArray* fRecCandTracks;
  TClonesArray* fRecTracks;
  TClonesArray* fRecBPTracks;

  // Output Data -------
  TClonesArray*  fTrackQ;		
 
  int fEventNr;
// bool fUseMVDPoint;
  Double_t fPbeam;
  // Int_t fPDGid;
  // TVector3 vtx;
  bool fWriteAllMC;

  //  TDatabasePDG *fdbPDG;
  


  

  ClassDef(PndLmdTrkQTask,1);

};

#endif
