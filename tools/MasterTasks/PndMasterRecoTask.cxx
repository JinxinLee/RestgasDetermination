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
#include "PndRecoKalmanTask2.h"
#include "PndRecoMultiKalmanTask.h"
#include "PndRecoMultiKalmanTask2.h"

#include "PndMissingPzCleanerTask.h"
//#include "PndFtsTrackerIdeal.h"
#include "PndTrackSmearTask.h"

#include <array>

/**
 * @brief Default Constructor
 * @details # Reconstruction task list
 * Here all the reconstruction tasks are added to the task, with the standard settings. A check is done after each task if the tasklist enum is broken or not. At the end the event counter is added (each 100 events), and the verbosity is set to 0 to all the tasks (it can be changed afterwards with SetVerbosity() functions.
 **/
// -----   Default constructor   -------------------------------------------
PndMasterRecoTask::PndMasterRecoTask(TString options) :
  PndMasterTask("Master Reconstruction Task"), fOptions(options)
{

  std::array<TString,5> hypoName= {"Electron","Muon","Pion","Kaon","Proton"};
  // -----  MVD + STT Pattern Recognition -----------------------------------
  //  use the constructor with input :
  //      printout flag (int) , plotting flag (bool), MC comparison flag (bool), SciTil.
  PndTrkTracking2* tracking = NULL;
  fBranchTasks.push_back(tracking = new PndTrkTracking2(0, false, false, true));
  tracking->SetInputBranchName("STTHit", "MVDHitsPixel", "MVDHitsStrip");
  // tracking->SetInputBranchName("STTHitMix","MVDHitsPixelMix","MVDHitsStripMix");
  //  don't do the Pattern Recognition second part, starting from the Mvd;
  tracking->NoMvdAloneTracking();
  // do Cleanup only when there is Mixing;
  // tracking->Cleanup();
  fFixedPersistency[tracking] = kFALSE;

  // ----- MVD + STT + GEM Pattern Recognition --------------
  if ((!fOptions.Contains("nogem")) && (!fOptions.Contains("gem0"))) {
    PndSttMvdGemTracking *SttMvdGemTracking = NULL;
    fBranchTasks.push_back(SttMvdGemTracking = new PndSttMvdGemTracking(0));
    fFixedPersistency[SttMvdGemTracking] = kFALSE;
  }

  // ----- Barrel Kalman Task     ----------------------------
  if (!fOptions.Contains("multikalman"))
  { //no multikalman
    PndRecoKalmanTask* recoKalman = NULL;
    fBranchTasks.push_back(recoKalman = new PndRecoKalmanTask());
    if ((!fOptions.Contains("nogem")) && (!fOptions.Contains("gem0"))) {
      recoKalman->SetTrackInBranchName("SttMvdGemTrack");
      //      recoKalman->SetTrackInIDBranchName("SttMvdGemTrackID");
      recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack"); // PID appendix follows;
    } else {
      recoKalman->SetTrackInBranchName("SttMvdTrack");
      //      recoKalman->SetTrackInIDBranchName("SttMvdTrackID");
      recoKalman->SetTrackOutBranchName("SttMvdGenTrack"); // PID appendix follows
    }
    recoKalman->SetBusyCut(50); // CHECK to be tuned
    //recoKalman->SetIdealHyp(kTRUE);
    //recoKalman->SetNumIterations(3);
    recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
    //recoKalman->SetPropagateToIP(kFALSE);
  }
  else //yes, multikalman
  {
    PndRecoMultiKalmanTask* recoKalman = NULL;
    fBranchTasks.push_back(recoKalman = new PndRecoMultiKalmanTask());
    recoKalman->SetFitHypotheses(fOptions);  //Walter added
    if ((!fOptions.Contains("nogem")) && (!fOptions.Contains("gem0"))) {
      //if ((!fOptions.Contains("day1")) || (fOptions.Contains("gem"))) {
      recoKalman->SetTrackInBranchName("SttMvdGemTrack");
      //      recoKalman->SetTrackInIDBranchName("SttMvdGemTrackID");
      recoKalman->SetTrackOutBranchName("SttMvdGemGenTrack"); // PID appendix follows
    } else {
      recoKalman->SetTrackInBranchName("SttMvdTrack");
      //      recoKalman->SetTrackInIDBranchName("SttMvdTrackID");
      recoKalman->SetTrackOutBranchName("SttMvdGenTrack"); // PID appendix follows
    }
    recoKalman->SetBusyCut(50); // CHECK to be tuned
    //recoKalman->SetIdealHyp(kTRUE);
    //recoKalman->SetNumIterations(3);
    recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
    //recoKalman->SetPropagateToIP(kFALSE);
  }

  if (fOptions.Contains("filtered")) {
    PndMissingPzCleanerTask* cleaner = NULL;
    fBranchTasks.push_back(cleaner = new PndMissingPzCleanerTask());
    if ((!fOptions.Contains("nogem")) && (!fOptions.Contains("gem0"))) {
      cleaner->SetInputTrackBranch("SttMvdGemGenTrack"); // _filtered appendix follows
    } else {
      cleaner->SetInputTrackBranch("SttMvdGenTrack");// _filtered appendix follows
    }
    cleaner->SetRemoveTrack(kTRUE);
  }


  // -----  FTS Ideal Tracking    ----------------------------
  TString ftstrkname="FtsIdealTrack", ftstrkgenname="FtsIdealGenTrack";
  if(fOptions.Contains("ftsca")) {
    ftstrkname="FtsCaTrack";
    ftstrkgenname="FtsCaGenTrack";
  }
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
  fFixedPersistency[trackFts] = kFALSE;

  // ----- Forward Kalman Task     ---------------------------
  if (!fOptions.Contains("multikalman")) { // no multikalman
    if(fOptions.Contains("genfit2")) {
      PndRecoKalmanTask2* recoKalman = NULL;
      fBranchTasks.push_back(recoKalman = new PndRecoKalmanTask2());
      recoKalman->SetTrackInBranchName(ftstrkname);
      recoKalman->SetTrackOutBranchName(ftstrkgenname);
    } else {
      PndRecoKalmanTask* recoKalman = NULL;
      fBranchTasks.push_back(recoKalman = new PndRecoKalmanTask());

      recoKalman->SetTrackInBranchName(ftstrkname);
      recoKalman->SetTrackOutBranchName(ftstrkgenname);
    }
  } else { //yes, multikalman!
    if(fOptions.Contains("genfit2")) {
      PndRecoMultiKalmanTask2* recoKalman = NULL;
      fBranchTasks.push_back(recoKalman = new PndRecoMultiKalmanTask2());
      recoKalman->SetTrackInBranchName(ftstrkname);
      recoKalman->SetTrackOutBranchName(ftstrkgenname);
    } else {
      PndRecoMultiKalmanTask* recoKalman = NULL;
      fBranchTasks.push_back(recoKalman = new PndRecoMultiKalmanTask());
      recoKalman->SetTrackInBranchName(ftstrkname);
      recoKalman->SetTrackOutBranchName(ftstrkgenname);
    }
  }

  // --- Online Emulator smearing
  if(fOptions.Contains("fakeonline"))
  {
    PndTrackSmearTask* smearer=NULL;
    fBranchTasks.push_back(smearer=new PndTrackSmearTask());
    TString barrelbranchname="SttMvdGemGenTrack";
    TString ftsbranchname="FtsIdealGenTrack";
    if (fOptions.Contains("nogem")||fOptions.Contains("gem0")) {
      barrelbranchname="SttMvdGenTrack";
    }

    if(fOptions.Contains("filtered")) barrelbranchname+="_filtered";

    if(fOptions.Contains("multikalman")) { //yes, multikalman
      for(int k=0; k<5; ++k) {
        smearer->AddInputTrackBranch(Form("%s%s",barrelbranchname.Data(),hypoName[k].Data()));
        smearer->AddInputTrackBranch(Form("%s%s",ftsbranchname.Data(),hypoName[k].Data()));
      }
    } else { // no multikalman
      smearer->AddInputTrackBranch(barrelbranchname);
      smearer->AddInputTrackBranch(ftsbranchname);
    }
  }

  std::for_each(fBranchTasks.begin(), fBranchTasks.end(),
  [this](const FairTask* task) {
    Add((TTask*)task);
  });

  SetVerbose(0);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMasterRecoTask::~PndMasterRecoTask() {
}
// -------------------------------------------------------------------------

/** @cond CLASSIMP */
ClassImp(PndMasterRecoTask);
/** @endcond */

