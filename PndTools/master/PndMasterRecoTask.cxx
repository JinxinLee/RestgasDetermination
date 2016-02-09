// -------------------------------------------------------------------------
// -----                    PndMasterRecoTask source file              -----
// -----                   Created 31/01/16  by S. Spataro             -----
// -----                   Wrapper for  recotizing tasks               -----
// -------------------------------------------------------------------------


#include "PndMasterRecoTask.h"
#include "PndMasterTask.h"

#include "FairGeane.h"
#include "PndTrkTracking2.h"
#include "PndSttMvdGemTracking.h"
#include "PndMCTrackAssociator.h"
#include "PndRecoKalmanTask.h"
#include "PndFtsTrackerIdeal.h"

/**
 * @brief Default Constructor
 * @details # Reconstruction task list
 * Here all the reconstruction tasks are added to the task, with the standard settings. A check is done after each task if the tasklist enum is broken or not. At the end the event counter is added (each 100 events), and the verbosity is set to 0 to all the tasks (it can be changed afterwards with SetVerbosity() functions.
 **/
// -----   Default constructor   -------------------------------------------
PndMasterRecoTask::PndMasterRecoTask() :
  PndMasterTask("Master Reconstruction Task")
{
  
  // -----   Geane   ---------------------------------------
  this->Add(new FairGeane()); // 0
  if ((this->GetListOfTasks()->GetSize()-1) != kFairGeane) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));

  // -----  MVD + STT Pattern Recognition -----------------------------------
  //  use the constructor with input :
  //      printout flag (int) , plotting flag (bool), MC comparison flag (bool), SciTil.
  PndTrkTracking2* tracking = NULL;
  this->Add(tracking = new PndTrkTracking2(0,false,false,true)); // 1
  if ((this->GetListOfTasks()->GetSize()-1) != kPndTrkTracking2) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  tracking->SetInputBranchName("STTHit","MVDHitsPixel","MVDHitsStrip");
  // tracking->SetInputBranchName("STTHitMix","MVDHitsPixelMix","MVDHitsStripMix");
  //  don't do the Pattern Recognition second part, starting from the Mvd;
  tracking->NoMvdAloneTracking();
  // do Cleanup only when there is Mixing;
  // tracking->Cleanup();
  tracking->SetPersistence(kFALSE);

  // ----- MVD + STT + GEM Pattern Recognition --------------
  PndSttMvdGemTracking *SttMvdGemTracking = NULL;
  this->Add(SttMvdGemTracking = new PndSttMvdGemTracking(0)); // 2
  if ((this->GetListOfTasks()->GetSize()-1) != kPndSttMvdGemTracking) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  SttMvdGemTracking->SetPersistence(kFALSE);
  
  // ----- MC Association #1 ---------------------------------
  // Useful only if you want to use ideal hypothesis in barrel kalman
  PndMCTrackAssociator* trackMC = NULL;
  this->Add(trackMC = new PndMCTrackAssociator()); // 3
  if ((this->GetListOfTasks()->GetSize()-1) != kPndMCTrackAssociator1) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  trackMC->SetTrackInBranchName("SttMvdGemTrack");
  trackMC->SetTrackOutBranchName("SttMvdGemTrackID");
  trackMC->SetPersistence(kFALSE);

  // ----- Barrel Kalman Task     ----------------------------
  PndRecoKalmanTask* recoKalman = NULL;
  this->Add(recoKalman = new PndRecoKalmanTask()); // 4
  if ((this->GetListOfTasks()->GetSize()-1) != kPndRecoKalmanTask1) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  recoKalman->SetTrackInBranchName("SttMvdGemTrack");
  recoKalman->SetTrackInIDBranchName("SttMvdGemTrackID");
  recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack");
  recoKalman->SetBusyCut(50); // CHECK to be tuned
  //recoKalman->SetIdealHyp(kTRUE);
  //recoKalman->SetNumIterations(3);
  recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
  //recoKalman->SetPropagateToIP(kFALSE);
  
  // ----- MC Association #2 ---------------------------------
  PndMCTrackAssociator* trackMC2 = NULL;
  this->Add(trackMC2 = new PndMCTrackAssociator()); // 5
  if ((this->GetListOfTasks()->GetSize()-1) != kPndMCTrackAssociator2) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  trackMC2->SetTrackInBranchName("SttMvdGemGenTrack"); 
  trackMC2->SetTrackOutBranchName("SttMvdGemGenTrackID");
  
  // -----  FTS Ideal Tracking    ----------------------------
  PndFtsTrackerIdeal* trackFts = NULL;
  this->Add(trackFts = new PndFtsTrackerIdeal()); // 6
  if ((this->GetListOfTasks()->GetSize()-1) != kPndFtsTrackerIdeal) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  trackFts->SetRelativeMomentumSmearing(0.05);
  trackFts->SetVertexSmearing(0.05, 0.05, 0.05);
  trackFts->SetTrackingEfficiency(1.);
  trackFts->SetTrackOutput("FtsIdealTrack");
  trackFts->SetPersistence(kFALSE);

  // ----- MC Association #3 ---------------------------------
  // Useful only if you want to use ideal hypothesis in fwd kalman
  PndMCTrackAssociator* trackMCfwd = NULL;
  this->Add(trackMCfwd = new PndMCTrackAssociator()); // 7
  if ((this->GetListOfTasks()->GetSize()-1) != kPndMCTrackAssociator3) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  trackMCfwd->SetTrackInBranchName("FtsIdealTrack");
  trackMCfwd->SetTrackOutBranchName("FtsIdealTrackID");

  // ----- Forward Kalman Task     ---------------------------
  PndRecoKalmanTask* recoKalmanFwd = NULL;
  this->Add(recoKalmanFwd = new PndRecoKalmanTask()); // 8
  if ((this->GetListOfTasks()->GetSize()-1) != kPndRecoKalmanTask2) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  recoKalmanFwd->SetTrackInBranchName("FtsIdealTrack");
  //recoKalmanFwd->SetTrackInIDBranchName("FtsIdealTrackID");
  recoKalmanFwd->SetTrackOutBranchName("FtsIdealGenTrack");
  recoKalmanFwd->SetBusyCut(50); // CHECK to be tuned
  //recoKalmanFwd->SetIdealHyp(kTRUE);
  //recoKalmanFwd->SetNumIterations(3);
  recoKalmanFwd->SetTrackRep(0); // 0 Geane (default), 1 RK
  //recoKalmanFwd->SetPropagateToIP(kFALSE);

  // ----- MC Association #4 ---------------------------------
  PndMCTrackAssociator* trackMC3 = NULL;
  this->Add(trackMC3 = new PndMCTrackAssociator()); // 9
  if ((this->GetListOfTasks()->GetSize()-1) != kPndMCTrackAssociator4) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  trackMC3->SetTrackInBranchName("FtsIdealGenTrack");
  trackMC3->SetTrackOutBranchName("FtsIdealGenTrackID");
 
  SetVerbose(0);
}
// -------------------------------------------------------------------------

/** Set the Persistency of all the tasks in the same way **/
void PndMasterRecoTask::SetPersistency(Bool_t pers)
{
    // -----  MVD + STT Pattern Recognition -----------------------------------
  ((PndTrkTracking2*)GetListOfTasks()->At(kPndTrkTracking2))->SetPersistence(pers);

  // ----- MVD + STT + GEM Pattern Recognition --------------
  ((PndSttMvdGemTracking*)GetListOfTasks()->At(kPndSttMvdGemTracking))->SetPersistence(pers);
  
  // ----- MC Association #1 ---------------------------------
  ((PndMCTrackAssociator*)GetListOfTasks()->At(kPndMCTrackAssociator1))->SetPersistence(pers);
  
  // ----- Barrel Kalman Task     ----------------------------
  ((PndRecoKalmanTask*)GetListOfTasks()->At(kPndRecoKalmanTask1))->SetPersistence(pers);
  
  // ----- MC Association #2 ---------------------------------
  ((PndMCTrackAssociator*)GetListOfTasks()->At(kPndMCTrackAssociator2))->SetPersistence(pers);
  
  // -----  FTS Ideal Tracking    ----------------------------
  ((PndFtsTrackerIdeal*)GetListOfTasks()->At(kPndFtsTrackerIdeal))->SetPersistence(pers);

  // ----- MC Association #3 ---------------------------------
  ((PndMCTrackAssociator*)GetListOfTasks()->At(kPndMCTrackAssociator3))->SetPersistence(pers);

  // ----- Forward Kalman Task     ---------------------------
  ((PndRecoKalmanTask*)GetListOfTasks()->At(kPndRecoKalmanTask2))->SetPersistence(pers);

  // ----- MC Association #4 ---------------------------------
  ((PndMCTrackAssociator*)GetListOfTasks()->At(kPndMCTrackAssociator4))->SetPersistence(pers);

  return;
}


// -----   Destructor   ----------------------------------------------------
PndMasterRecoTask::~PndMasterRecoTask()
{
}
// -------------------------------------------------------------------------

/** @cond CLASSIMP */
ClassImp(PndMasterRecoTask);
/** @endcond */
