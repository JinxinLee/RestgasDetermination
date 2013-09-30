// -------------------------------------------------------------------------
// -----                      PndTrackDraw header file                 -----
// -----                Created 09/25/09  by T. Stockmanns             -----
// -------------------------------------------------------------------------


/** PndTrackDraw
 * @author T.Stockmanns
 * @since 25.09.09
 *   MVD event display object
 **
 **/

#ifndef PNDTRACKDRAW_H
#define PNDTRACKDRAW_H

#include "FairTask.h"
#include "TEveTrackPropagator.h"
#include "TEveElement.h"

#include "TEveArrow.h"
#include "TString.h"
#include "FairGeanePro.h"

#include "FairRootManager.h"

#include "PndTrackCandDraw.h"
#include <vector>
class TEveTrackList;
class FairEventManager;
class TObjArray;

class PndTrackDraw : public FairTask {

 public:

  /** Default constructor **/
  PndTrackDraw(Bool_t propagation = kFALSE);


  /** Standard constructor 
  *@param name        Name of task
  *@param iVerbose    Verbosity level
  **/
  PndTrackDraw(const char* name, Bool_t propagation = kFALSE, Int_t iVerbose = 1);

  /** Destructor **/
  virtual ~PndTrackDraw();

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

  Double_t fTimeWindowPlus;
  Double_t fTimeWindowMinus;
  Double_t fStartTime;
  Bool_t fUseEventTime;

protected:

  void PropagateTrack(FairTrackParP& trackPar, Int_t pidHypo, Int_t color = -1, TEveElement* group = 0);
  void PropagateTrackHelix(FairTrackParP& trackPar, Int_t pidHypo, Int_t color = -1, TEveElement* group = 0);
  TClonesArray  *fPndTrackList;  //!
  TEveTrackPropagator *fTrPr; 
  FairEventManager * fEventManager; //!
  TObjArray *fEveTrList;
  TString fEvent; //!
  TEveTrackList *fTrList;  //!
  //TEveElementList *fTrackCont;
  FairGeanePro *fPro;
 // FairRootManager* fManager;
  PndTrackCandDraw* fTrackCandDraw;
  std::vector<TEveArrow*> fArrows;
  Bool_t fDoPropagation;

  TEveElementList* fListOfTracks;
    
   Double_t MinEnergyLimit;
   Double_t MaxEnergyLimit;
   Double_t PEnergy;

   BinaryFunctor* fStartFunctor;
   BinaryFunctor* fStopFunctor;

  ClassDef(PndTrackDraw,1);
    
};


#endif
