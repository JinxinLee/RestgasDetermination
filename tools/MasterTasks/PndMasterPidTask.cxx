// -------------------------------------------------------------------------
// -----                    PndMasterPidTask source file               -----
// -----                   Created 01/02/16  by S. Spataro             -----
// -----                       Wrapper for  pid tasks                  -----
// -------------------------------------------------------------------------


#include "PndMasterPidTask.h"
#include "PndMasterTask.h"

#include "PndPidCorrelator.h"
#include "PndPidBremCorrector.h"
#include "PndMcCloner.h"
#include "PndPidIdealAssociatorTask.h"
#include "PndPidMvdAssociatorTask.h"
#include "PndPidMdtHCAssociatorTask.h"
#include "PndPidDrcAssociatorTask.h"
#include "PndPidDiscAssociatorTask.h"
#include "PndPidSttAssociatorTask.h"
#include "PndPidEmcBayesAssociatorTask.h"
#include "PndPidSciTAssociatorTask.h"
#include "PndPidFtofAssociatorTask.h"
#include "PndPidRichAssociatorTask.h"

/**
 * @brief Default Constructor
 * @details # Pid task list
 * Here all the pid tasks are added to the task, with the standard settings. A check is done after each task if the tasklist enum is broken or not. At the end the event counter is added (each 100 events), and the verbosity is set to 0 to all the tasks (it can be changed afterwards with SetVerbosity() functions.
 **/
// -----   Default constructor   -------------------------------------------
PndMasterPidTask::PndMasterPidTask(TString options) :
  PndMasterTask("Master Pid Task"), fOptions(options)
{

  // -----   Correlation   ---------------------------------
  PndPidCorrelator* corr = new PndPidCorrelator();

  TString barrelbranchname="SttMvdGemGenTrack";
  if (fOptions.Contains("nogem")||fOptions.Contains("gem0")) {
    barrelbranchname="SttMvdGenTrack";
  }
  if (fOptions.Contains("filtered")) barrelbranchname+="_filtered";
  if (fOptions.Contains("fakeonline")) barrelbranchname+="_fakeonline";
  corr->SetBarrelTrackBranch(barrelbranchname);

  TString fwdbranchname="FtsIdealGenTrack";
  if (fOptions.Contains("fakeonline")) fwdbranchname+="_fakeonline";
  corr->SetForwardTrackBranch(fwdbranchname);

  if (fOptions.Contains("piddebug")) corr->SetDebugMode(kTRUE);
  if (fOptions.Contains("pidfast")) corr->SetFast(kTRUE);
  if (fOptions.Contains("pidnoswim")) corr->SetBackPropagate(kFALSE);

  fStandardTasks.push_back(corr);

  // -----   Bremsstrahlung Correction ----------------------
  fStandardTasks.push_back(new PndPidBremCorrector());

  // -----   Classifiers   ----------------------------------
  fStandardTasks.push_back(new PndPidIdealAssociatorTask());

  fStandardTasks.push_back(new PndPidMvdAssociatorTask());
  fStandardTasks.push_back(new PndPidMdtHCAssociatorTask());
  fStandardTasks.push_back(new PndPidDrcAssociatorTask());

  if ( !fOptions.Contains("day1") && !fOptions.Contains("phase1") )
  {
    fStandardTasks.push_back(new PndPidDiscAssociatorTask());
  }

  fStandardTasks.push_back(new PndPidSttAssociatorTask());
  fStandardTasks.push_back(new PndPidEmcBayesAssociatorTask());
  fStandardTasks.push_back(new PndPidSciTAssociatorTask());
  fStandardTasks.push_back(new PndPidFtofAssociatorTask());

  if ( !fOptions.Contains("day1") && !fOptions.Contains("phase1") )
  {
    fStandardTasks.push_back(new PndPidRichAssociatorTask());
  }

  // -----   MC Cloner   ------------------------------------
  PndMcCloner *clone = NULL;
  fStandardTasks.push_back(clone = new PndMcCloner());
  // Option to clean the MCTrack TClonesArray from particles which were not interacting with sensitive detectors
  clone->SetCleanMc();

  std::for_each(fStandardTasks.begin(), fStandardTasks.end(), [this](const FairTask* task){ Add((TTask*)task); } );

  SetVerbose(0);

}
// -------------------------------------------------------------------------

/** Set the Persistency of all the tasks in the same way **/
void PndMasterPidTask::SetPersistency(Bool_t pers)
{
  if (!pers) LOG(INFO) << "It makes no sense to have pid persistency switched OFF!" << FairLogger::endl;
  if (!pers) LOG(INFO) << "Or, if you prefer... this functionality has not been implemented yet" << FairLogger::endl;

  return;
}

// -----   Destructor   ----------------------------------------------------
PndMasterPidTask::~PndMasterPidTask()
{
}
// -------------------------------------------------------------------------

/** @cond CLASSIMP */
ClassImp(PndMasterPidTask);
/** @endcond */

