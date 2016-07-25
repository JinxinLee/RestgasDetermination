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
PndMasterRecoTask::PndMasterRecoTask(TString options) :
  PndMasterTask("Master Reconstruction Task"), fOptions(options)
{
  reco = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
  
  // -----   Geane   ---------------------------------------
  this->Add(new FairGeane()); // 0
  reco.kFairGeane = GetListOfTasks()->GetSize()-1;
  
  // -----  MVD + STT Pattern Recognition -----------------------------------
  //  use the constructor with input :
  //      printout flag (int) , plotting flag (bool), MC comparison flag (bool), SciTil.
  PndTrkTracking2* tracking = NULL;
  this->Add(tracking = new PndTrkTracking2(0,false,false,true)); // 1
  reco.kPndTrkTracking2 = GetListOfTasks()->GetSize()-1;
  tracking->SetInputBranchName("STTHit","MVDHitsPixel","MVDHitsStrip");
  // tracking->SetInputBranchName("STTHitMix","MVDHitsPixelMix","MVDHitsStripMix");
  //  don't do the Pattern Recognition second part, starting from the Mvd;
  tracking->NoMvdAloneTracking();
  // do Cleanup only when there is Mixing;
  // tracking->Cleanup();
  tracking->SetPersistence(kFALSE);

  // ----- MVD + STT + GEM Pattern Recognition --------------
  if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
    {
      PndSttMvdGemTracking *SttMvdGemTracking = NULL;
      this->Add(SttMvdGemTracking = new PndSttMvdGemTracking(0)); // 2
      reco.kPndSttMvdGemTracking = GetListOfTasks()->GetSize()-1;
      SttMvdGemTracking->SetPersistence(kFALSE);
    }
  
  // ----- MC Association #1 ---------------------------------
  // Useful only if you want to use ideal hypothesis in barrel kalman
  PndMCTrackAssociator* trackMC = NULL;
  this->Add(trackMC = new PndMCTrackAssociator()); // 3
  reco.kPndMCTrackAssociator1 = GetListOfTasks()->GetSize()-1;
  if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
    {
      trackMC->SetTrackInBranchName("SttMvdGemTrack");
      trackMC->SetTrackOutBranchName("SttMvdGemTrackID");
    }
  else
    {
      trackMC->SetTrackInBranchName("SttMvdTrack");
      trackMC->SetTrackOutBranchName("SttMvdTrackID");
    }
  trackMC->SetPersistence(kFALSE);
  
  // ----- Barrel Kalman Task     ----------------------------
  PndRecoKalmanTask* recoKalman = NULL;
  this->Add(recoKalman = new PndRecoKalmanTask()); // 4
  reco.kPndRecoKalmanTask1 = GetListOfTasks()->GetSize()-1;
  if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
    {
      recoKalman->SetTrackInBranchName("SttMvdGemTrack");
      recoKalman->SetTrackInIDBranchName("SttMvdGemTrackID");
      recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack");
    }
  else
    {
      recoKalman->SetTrackInBranchName("SttMvdTrack");
      recoKalman->SetTrackInIDBranchName("SttMvdTrackID");
      recoKalman->SetTrackOutBranchName("SttMvdGenTrack");
    }
  recoKalman->SetBusyCut(50); // CHECK to be tuned
  //recoKalman->SetIdealHyp(kTRUE);
  //recoKalman->SetNumIterations(3);
  recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
  //recoKalman->SetPropagateToIP(kFALSE);
  
  // ----- MC Association #2 ---------------------------------
  PndMCTrackAssociator* trackMC2 = NULL;
  this->Add(trackMC2 = new PndMCTrackAssociator()); // 5
  reco.kPndMCTrackAssociator2 = GetListOfTasks()->GetSize()-1;
  if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
    {
      trackMC2->SetTrackInBranchName("SttMvdGemGenTrack"); 
      trackMC2->SetTrackOutBranchName("SttMvdGemGenTrackID");
    }
  else
    {
      trackMC2->SetTrackInBranchName("SttMvdGenTrack"); 
      trackMC2->SetTrackOutBranchName("SttMvdGenTrackID");
    }
  
  // -----  FTS Ideal Tracking    ----------------------------
  PndFtsTrackerIdeal* trackFts = NULL;
  this->Add(trackFts = new PndFtsTrackerIdeal()); // 6
  reco.kPndFtsTrackerIdeal = GetListOfTasks()->GetSize()-1;
  trackFts->SetRelativeMomentumSmearing(0.05);
  trackFts->SetVertexSmearing(0.05, 0.05, 0.05);
  trackFts->SetTrackingEfficiency(1.);
  trackFts->SetTrackOutput("FtsIdealTrack");
  trackFts->SetPersistence(kFALSE);

  // ----- MC Association #3 ---------------------------------
  // Useful only if you want to use ideal hypothesis in fwd kalman
  PndMCTrackAssociator* trackMCfwd = NULL;
  this->Add(trackMCfwd = new PndMCTrackAssociator()); // 7
  reco.kPndMCTrackAssociator3 = GetListOfTasks()->GetSize()-1;
  trackMCfwd->SetTrackInBranchName("FtsIdealTrack");
  trackMCfwd->SetTrackOutBranchName("FtsIdealTrackID");

  // ----- Forward Kalman Task     ---------------------------
  PndRecoKalmanTask* recoKalmanFwd = NULL;
  this->Add(recoKalmanFwd = new PndRecoKalmanTask()); // 8
  reco.kPndRecoKalmanTask2 = GetListOfTasks()->GetSize()-1;
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
  reco.kPndMCTrackAssociator4 = GetListOfTasks()->GetSize()-1;
  trackMC3->SetTrackInBranchName("FtsIdealGenTrack");
  trackMC3->SetTrackOutBranchName("FtsIdealGenTrackID");
 
  SetVerbose(0);
}
// -------------------------------------------------------------------------

/** Set the Persistency of all the tasks in the same way **/
void PndMasterRecoTask::SetPersistency(Bool_t pers)
{
    // -----  MVD + STT Pattern Recognition -----------------------------------
  ((PndTrkTracking2*)GetListOfTasks()->At(reco.kPndTrkTracking2))->SetPersistence(pers);

  if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
    {
      // ----- MVD + STT + GEM Pattern Recognition --------------
      ((PndSttMvdGemTracking*)GetListOfTasks()->At(reco.kPndSttMvdGemTracking))->SetPersistence(pers);
    }
  
  // ----- MC Association #1 ---------------------------------
  ((PndMCTrackAssociator*)GetListOfTasks()->At(reco.kPndMCTrackAssociator1))->SetPersistence(pers);
  
  // ----- Barrel Kalman Task     ----------------------------
  ((PndRecoKalmanTask*)GetListOfTasks()->At(reco.kPndRecoKalmanTask1))->SetPersistence(pers);
  
  // ----- MC Association #2 ---------------------------------
  ((PndMCTrackAssociator*)GetListOfTasks()->At(reco.kPndMCTrackAssociator2))->SetPersistence(pers);
  
  // -----  FTS Ideal Tracking    ----------------------------
  ((PndFtsTrackerIdeal*)GetListOfTasks()->At(reco.kPndFtsTrackerIdeal))->SetPersistence(pers);

  // ----- MC Association #3 ---------------------------------
  ((PndMCTrackAssociator*)GetListOfTasks()->At(reco.kPndMCTrackAssociator3))->SetPersistence(pers);

  // ----- Forward Kalman Task     ---------------------------
  ((PndRecoKalmanTask*)GetListOfTasks()->At(reco.kPndRecoKalmanTask2))->SetPersistence(pers);

  // ----- MC Association #4 ---------------------------------
  ((PndMCTrackAssociator*)GetListOfTasks()->At(reco.kPndMCTrackAssociator4))->SetPersistence(pers);

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
