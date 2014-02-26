// -------------------------------------------------------------------------
// -----                   PndLmdSigCleanTask header file            -----
// -----                  Created 26/02/14  by A.Karavdina            -----
// -------------------------------------------------------------------------


/** PndLmdSigCleanTask.h
 *@author A.Karavdina <karavdin@kph.uni-mainz.de>
 **
 ** Clean sample with reconstructed tracks from background
 ** by using momentum cut or multivariable cut
 **/


#ifndef PndLmdSigCleanTask_H
#define PndLmdSigCleanTask_H


// framework includes
#include "FairTask.h"
#include "TH1.h"
//#include "GFTrackCand.h"
//#include "FairGeanePro.h"
#include "TMatrixD.h"
#include "TVector3.h"
#include "TNtuple.h"
#include "FairTrackParH.h"
//#include "PndSdsGeoHandling.h"
#include "PndGeoHandling.h"
#include <vector>
#include <map>
//TMVA includes
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
//#include "PndMultiField.h"
class TClonesArray;
class PndSdsCluster;
class PndLmdSigCleanTask : public FairTask
{
 public:

  /** Default constructor **/  
  PndLmdSigCleanTask();

  ///Set up beam momuntum value
  PndLmdSigCleanTask(Double_t pBeam, TString dir);

  /** Destructor **/
  ~PndLmdSigCleanTask();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
  virtual void Finish();

  bool CheckMVA(FairTrackParH* fTrk);
  bool CheckMom(double prec);
  bool Check(FairTrackParH* fTrk);

 private:

  TClonesArray* fTrackParFinal;

  PndGeoHandling* fGeoH;
  int fEventNr;
  Double_t fPbeam;


  // Input Data ----------
  TClonesArray* fRecBPTracks;
  TClonesArray* fMCHits;
  TClonesArray* fMCTracks;


  // MVA Data -------
  TString fdir;
  TString fmethodName;
  Float_t axrec, ayrec, aprec;
  Float_t azrec, athrec, aphrec;
  TMVA::Reader *reader;

  //----------------------------------------

// void Register();
  
// void Reset();
  
  ClassDef(PndLmdSigCleanTask,1);
};

#endif
