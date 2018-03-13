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

  // -----  MVD + STT Pattern Recognition -----------------------------------
  //  use the constructor with input :
  //      printout flag (int) , plotting flag (bool), MC comparison flag (bool), SciTil.
  PndTrkTracking2* tracking = NULL;
  fBranchTasks.push_back(tracking = new PndTrkTracking2(0,false,false,true));
  tracking->SetInputBranchName("STTHit","MVDHitsPixel","MVDHitsStrip");
  // tracking->SetInputBranchName("STTHitMix","MVDHitsPixelMix","MVDHitsStripMix");
  //  don't do the Pattern Recognition second part, starting from the Mvd;
  tracking->NoMvdAloneTracking();
  // do Cleanup only when there is Mixing;
  // tracking->Cleanup();
  fFixedPersistency[tracking]=kFALSE;

  // ----- MVD + STT + GEM Pattern Recognition --------------
  if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
    {
      PndSttMvdGemTracking *SttMvdGemTracking = NULL;
      fBranchTasks.push_back(SttMvdGemTracking = new PndSttMvdGemTracking(0));
      fFixedPersistency[SttMvdGemTracking]=kFALSE;
    }
  
  
  // ----- Barrel Kalman Task     ----------------------------
  PndRecoKalmanTask* recoKalman = NULL;
  fBranchTasks.push_back(recoKalman = new PndRecoKalmanTask());
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
	  fBranchTasks.push_back(cleaner = new PndMissingPzCleanerTask());
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
  fBranchTasks.push_back(trackFts = new PndIdealTrackFinder());
  trackFts->SetTrackSelector("FtsTrackFunctor");
  trackFts->AddBranchName("FTSHit");
  trackFts->AddBranchName("MVDHitsPixel");
  trackFts->AddBranchName("MVDHitsStrip");
  trackFts->SetRelativeMomentumSmearing(0.05);
  trackFts->SetVertexSmearing(0.05, 0.05, 0.05);
  trackFts->SetTrackingEfficiency(1.);
  trackFts->SetOutputBranchName("FtsIdealTrack");
  fFixedPersistency[trackFts]=kFALSE;



  // ----- Forward Kalman Task     ---------------------------
  PndRecoKalmanTask* recoKalmanFwd = NULL;
  fBranchTasks.push_back(recoKalmanFwd = new PndRecoKalmanTask());
  recoKalmanFwd->SetTrackInBranchName("FtsIdealTrack");
  //recoKalmanFwd->SetTrackInIDBranchName("FtsIdealTrackID");
  recoKalmanFwd->SetTrackOutBranchName("FtsIdealGenTrack");
  recoKalmanFwd->SetBusyCut(50); // CHECK to be tuned
  //recoKalmanFwd->SetIdealHyp(kTRUE);
  //recoKalmanFwd->SetNumIterations(3);
  recoKalmanFwd->SetTrackRep(0); // 0 Geane (default), 1 RK
  //recoKalmanFwd->SetPropagateToIP(kFALSE);

  std::for_each(fBranchTasks.begin(), fBranchTasks.end(), [this](const FairTask* task){ Add((TTask*)task); } );


  SetVerbose(0);
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMasterRecoTask::~PndMasterRecoTask()
{
}
// -------------------------------------------------------------------------

/** @cond CLASSIMP */
ClassImp(PndMasterRecoTask);
/** @endcond */
