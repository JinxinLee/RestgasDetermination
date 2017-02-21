// -------------------------------------------------------------------------
// -----                    PndMasterRecoIdealTask source file              -----
// -----                   Created 31/01/16  by S. Spataro             -----
// -----                   Wrapper for  recotizing tasks               -----
// -------------------------------------------------------------------------


#include <PndIdealTrackFinder.h>
#include "PndMasterRecoIdealTask.h"
#include "PndMasterTask.h"

//#include "PndMCTrackAssociator.h"
#include "PndRecoKalmanTask.h"
//#include "PndFtsTrackerIdeal.h"

/**
 * @brief Default Constructor
 * @details # Reconstruction task list
 * Here all the reconstruction tasks are added to the task, with the standard settings. A check is done after each task if the tasklist enum is broken or not. At the end the event counter is added (each 100 events), and the verbosity is set to 0 to all the tasks (it can be changed afterwards with SetVerbosity() functions.
 **/
// -----   Default constructor   -------------------------------------------
PndMasterRecoIdealTask::PndMasterRecoIdealTask(TString options) :
  PndMasterTask("Master Reconstruction Task"), fOptions(options)
{
  reco = {-1, -1, -1, -1};
 
  // -----  MVD + STT Pattern Recognition -----------------------------------
  //  use the constructor with input :
  //      printout flag (int) , plotting flag (bool), MC comparison flag (bool), SciTil.

  PndIdealTrackFinder* tracking = NULL;
  this->Add(tracking = new PndIdealTrackFinder()); // 1
  reco.kPndBarrelIdealTracking = GetListOfTasks()->GetSize()-1;
  tracking->SetTrackSelector("NoFtsTrackFunctor");
  tracking->SetRelativeMomentumSmearing(0.05);
  tracking->SetVertexSmearing(0.05, 0.05, 0.05);
  tracking->SetTrackingEfficiency(1.);
  tracking->SetOutputBranchName("SttMvdGemTrack");
  tracking->SetPersistence(kFALSE);


  // ----- Barrel Kalman Task     ----------------------------
  PndRecoKalmanTask* recoKalman = NULL;
  this->Add(recoKalman = new PndRecoKalmanTask()); // 2
  reco.kPndRecoKalmanTask1 = GetListOfTasks()->GetSize()-1;

  recoKalman->SetTrackInBranchName("SttMvdGemTrack");
  recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack");

  recoKalman->SetBusyCut(50); // CHECK to be tuned
  //recoKalman->SetIdealHyp(kTRUE);
  //recoKalman->SetNumIterations(3);
  recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
  //recoKalman->SetPropagateToIP(kFALSE);
  
  
  // -----  FTS Ideal Tracking    ----------------------------
  PndIdealTrackFinder* trackFts = NULL;
  this->Add(trackFts = new PndIdealTrackFinder()); // 3
  reco.kPndFtsTrackerIdeal = GetListOfTasks()->GetSize()-1;
  trackFts->SetTrackSelector("FtsTrackFunctor");
  trackFts->SetRelativeMomentumSmearing(0.05);
  trackFts->SetVertexSmearing(0.05, 0.05, 0.05);
  trackFts->SetTrackingEfficiency(1.);
  trackFts->SetOutputBranchName("FtsIdealTrack");
  trackFts->SetPersistence(kFALSE);


  // ----- Forward Kalman Task     ---------------------------
  PndRecoKalmanTask* recoKalmanFwd = NULL;
  this->Add(recoKalmanFwd = new PndRecoKalmanTask()); // 4
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
void PndMasterRecoIdealTask::SetPersistency(Bool_t pers)
{
    // -----  MVD + STT Pattern Recognition -----------------------------------
  ((PndIdealTrackFinder*)GetListOfTasks()->At(reco.kPndBarrelIdealTracking))->SetPersistence(pers);

  
  // ----- Barrel Kalman Task     ----------------------------
  ((PndRecoKalmanTask*)GetListOfTasks()->At(reco.kPndRecoKalmanTask1))->SetPersistence(pers);
  
  
  // -----  FTS Ideal Tracking    ----------------------------
  ((PndIdealTrackFinder*)GetListOfTasks()->At(reco.kPndFtsTrackerIdeal))->SetPersistence(pers);


  // ----- Forward Kalman Task     ---------------------------
  ((PndRecoKalmanTask*)GetListOfTasks()->At(reco.kPndRecoKalmanTask2))->SetPersistence(pers);


  return;
}


// -----   Destructor   ----------------------------------------------------
PndMasterRecoIdealTask::~PndMasterRecoIdealTask()
{
}
// -------------------------------------------------------------------------

/** @cond CLASSIMP */
ClassImp(PndMasterRecoIdealTask);
/** @endcond */
