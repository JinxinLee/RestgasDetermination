// -------------------------------------------------------------------------
// -----                      PndSttIsochroneDraw header file                 -----
// -----                Created 09/25/09  by T. Stockmanns             -----
// -------------------------------------------------------------------------


/** PndSttIsochroneDraw
 * @author T.Stockmanns
 * @since 25.09.09
 *   MVD event display object
 **
 **/

#ifndef PndSttIsochroneDraw_H
#define PndSttIsochroneDraw_H

#include "FairTask.h"
#include "TEveTrackPropagator.h"
#include "TEveElement.h"
#include "TEveBoxSet.h"

#include "TEveArrow.h"
#include "TString.h"
#include "FairGeanePro.h"

#include "FairRootManager.h"
#include "PndMCDataCrawler.h";

#include "PndTrackCandDraw.h"
#include "PndGeoSttPar.h"
#include <vector>
class TEveTrackList;
class FairEventManager;
class TObjArray;

class PndSttIsochroneDraw : public FairTask {

 public:

  /** Default constructor **/
  PndSttIsochroneDraw(Bool_t propagation = kFALSE);


  /** Standard constructor 
  *@param name        Name of task
  *@param iVerbose    Verbosity level
  **/
  PndSttIsochroneDraw(const char* name, Bool_t propagation = kFALSE, Int_t iVerbose = 1);

  /** Destructor **/
  virtual ~PndSttIsochroneDraw();

  /** Set verbosity level. For this task and all of the subtasks. **/
  void SetVerbose(Int_t iVerbose){fVerbose = iVerbose;};
   /** Executed task **/ 
  virtual void Exec(Option_t* option);
   virtual InitStatus Init();
   virtual void SetParContainers();
  
  /** Action after each event**/
  virtual void Finish();
  void Reset();
  TEveTrackList *GetTrGroup(Int_t pid);
  virtual Double_t GetTimeWindowPlus() {return fTimeWindowPlus;}
  virtual Double_t GetTimeWindowMinus() {return fTimeWindowMinus;}

  virtual void SetTimeWindowMinus(Double_t val);
  virtual void SetTimeWindowPlus(Double_t val);

  virtual void UseIsochroneTime(){fUseIsochroneTime = kTRUE;}

  Double_t fTimeWindowPlus;
  Double_t fTimeWindowMinus;
  Double_t fStartTime;
  Bool_t fUseEventTime;
  Bool_t fUseIsochroneTime;

protected:

  TClonesArray* fSttHitArray;
  TClonesArray* fSttTubeArray;

  PndGeoSttPar* fSttParameters;

  FairEventManager * fEventManager; //!
  TObjArray *fEveTrList;
  TString fEvent; //!
  TEveBoxSet* fListOfIsochrones;
  //TEveElementList *fTrackCont;

   BinaryFunctor* fStartFunctor;
   BinaryFunctor* fStopFunctor;

   PndMCDataCrawler* fCrawler;
   TBranch* fEventHeaderBranch;

  ClassDef(PndSttIsochroneDraw,1);
    
};


#endif
