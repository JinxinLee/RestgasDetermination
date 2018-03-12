// -------------------------------------------------------------------------
// -----                    PndMasterRecoTask source file              -----
// -----                   Created 31/01/16  by S. Spataro             -----
// -----                   Wrapper for  recotizing tasks               -----
// -------------------------------------------------------------------------


#include <PndIdealTrackFinder.h>
#include "PndMasterRecoTask.h"
#include "PndMasterTask.h"

#include "PndTrkTracking2.h"
#include "PndSttMvdGemTracking.h"
#include "PndRecoKalmanTask.h"
#include "PndMissingPzCleanerTask.h"
//#include "PndFtsTrackerIdeal.h"

/**
 * @brief Default Constructor
 * @details # Reconstruction task list
 * Here all the reconstruction tasks are added to the task, with the standard settings. A check is done after each task if the tasklist enum is broken or not. At the end the event counter is added (each 100 events), and the verbosity is set to 0 to all the tasks (it can be changed afterwards with SetVerbosity() functions.
 **/
// -----   Default constructor   -------------------------------------------
PndMasterRecoTask::PndMasterRecoTask(TString options) :
  PndMasterTask("Master Reconstruction Task"), fOptions(options)
{
	reco = {-1, -1, -1, -1, -1, -1};
 
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
  tracking->SetPersistency(kFALSE);

  // ----- MVD + STT + GEM Pattern Recognition --------------
  if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
    {
      PndSttMvdGemTracking *SttMvdGemTracking = NULL;
      this->Add(SttMvdGemTracking = new PndSttMvdGemTracking(0)); // 2
      reco.kPndSttMvdGemTracking = GetListOfTasks()->GetSize()-1;
      SttMvdGemTracking->SetPersistency(kFALSE);
    }
  
  
  // ----- Barrel Kalman Task     ----------------------------
  PndRecoKalmanTask* recoKalman = NULL;
  this->Add(recoKalman = new PndRecoKalmanTask()); // 3
  reco.kPndRecoKalmanTask1 = GetListOfTasks()->GetSize()-1;
  if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
    {
      recoKalman->SetTrackInBranchName("SttMvdGemTrack");
//      recoKalman->SetTrackInIDBranchName("SttMvdGemTrackID");
      recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack");
    }
  else
    {
      recoKalman->SetTrackInBranchName("SttMvdTrack");
//      recoKalman->SetTrackInIDBranchName("SttMvdTrackID");
      recoKalman->SetTrackOutBranchName("SttMvdGenTrack");
    }
  recoKalman->SetBusyCut(50); // CHECK to be tuned
  //recoKalman->SetIdealHyp(kTRUE);
  //recoKalman->SetNumIterations(3);
  recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
  //recoKalman->SetPropagateToIP(kFALSE);
  
  if (fOptions.Contains("filtered")){
	  PndMissingPzCleanerTask* cleaner = NULL;
	  this->Add(cleaner = new PndMissingPzCleanerTask()); //4
	  reco.kPndTrackCleaner = GetListOfTasks()->GetSize()-1;
	  if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
	  {
		  cleaner->SetInputTrackBranch("SttMvdGemGenTrack");
	  } else {
		  cleaner->SetInputTrackBranch("SttMvdGenTrack");
	  }
	  cleaner->SetRemoveTrack(kTRUE);
  }

  

  // -----  FTS Ideal Tracking    ----------------------------
  PndIdealTrackFinder* trackFts = NULL;
  this->Add(trackFts = new PndIdealTrackFinder()); // 5
  reco.kPndFtsTrackerIdeal = GetListOfTasks()->GetSize()-1;
  trackFts->SetTrackSelector("FtsTrackFunctor");
  trackFts->AddBranchName("FTSHit");
  trackFts->AddBranchName("MVDHitsPixel");
  trackFts->AddBranchName("MVDHitsStrip");
  trackFts->SetRelativeMomentumSmearing(0.05);
  trackFts->SetVertexSmearing(0.05, 0.05, 0.05);
  trackFts->SetTrackingEfficiency(1.);
  trackFts->SetOutputBranchName("FtsIdealTrack");
  trackFts->SetPersistence(kFALSE);


  // ----- Forward Kalman Task     ---------------------------
  PndRecoKalmanTask* recoKalmanFwd = NULL;
  this->Add(recoKalmanFwd = new PndRecoKalmanTask()); // 6
  reco.kPndRecoKalmanTask2 = GetListOfTasks()->GetSize()-1;
  recoKalmanFwd->SetTrackInBranchName("FtsIdealTrack");
  //recoKalmanFwd->SetTrackInIDBranchName("FtsIdealTrackID");
  recoKalmanFwd->SetTrackOutBranchName("FtsIdealGenTrack");
  recoKalmanFwd->SetBusyCut(50); // CHECK to be tuned
  //recoKalmanFwd->SetIdealHyp(kTRUE);
  //recoKalmanFwd->SetNumIterations(3);
  recoKalmanFwd->SetTrackRep(0); // 0 Geane (default), 1 RK
  //recoKalmanFwd->SetPropagateToIP(kFALSE);


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
      ((PndSttMvdGemTracking*)GetListOfTasks()->At(reco.kPndSttMvdGemTracking))->SetPersistency(pers);
    }
  
  
  // ----- Barrel Kalman Task     ----------------------------
  ((PndRecoKalmanTask*)GetListOfTasks()->At(reco.kPndRecoKalmanTask1))->SetPersistence(pers);

  ((PndMissingPzCleanerTask*)GetListOfTasks()->At(reco.kPndTrackCleaner))->SetPersistence(pers);


  
  // -----  FTS Ideal Tracking    ----------------------------
  ((PndIdealTrackFinder*)GetListOfTasks()->At(reco.kPndFtsTrackerIdeal))->SetPersistence(pers);


  // ----- Forward Kalman Task     ---------------------------
  ((PndRecoKalmanTask*)GetListOfTasks()->At(reco.kPndRecoKalmanTask2))->SetPersistence(pers);


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
