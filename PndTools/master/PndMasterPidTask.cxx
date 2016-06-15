// -------------------------------------------------------------------------
// -----                    PndMasterPidTask source file               -----
// -----                   Created 01/02/16  by S. Spataro             -----
// -----                       Wrapper for  pid tasks                  -----
// -------------------------------------------------------------------------


#include "PndMasterPidTask.h"
#include "PndMasterTask.h"

#include "FairGeane.h"
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

/**
 * @brief Default Constructor
 * @details # Pid task list
 * Here all the pid tasks are added to the task, with the standard settings. A check is done after each task if the tasklist enum is broken or not. At the end the event counter is added (each 100 events), and the verbosity is set to 0 to all the tasks (it can be changed afterwards with SetVerbosity() functions.
 **/
// -----   Default constructor   -------------------------------------------
PndMasterPidTask::PndMasterPidTask() :
  PndMasterTask("Master Pid Task")
{
  
  // -----   Geane   ---------------------------------------
  this->Add(new FairGeane()); // 0
  if ((this->GetListOfTasks()->GetSize()-1) != kFairGeane) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));

  // -----   Correlation   ---------------------------------
  PndPidCorrelator* corr = NULL;
  this->Add(corr = new PndPidCorrelator()); // 1
  if ((this->GetListOfTasks()->GetSize()-1) != kPndPidCorrelator) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  corr->SetInputBranch("SttMvdGemGenTrack");
  corr->SetInputIDBranch("SttMvdGemGenTrackID");
  corr->SetInputBranch2("FtsIdealGenTrack");
  corr->SetInputIDBranch2("FtsIdealGenTrackID");
  //corr->SetDebugMode(kTRUE);
  //corr->SetFast(kTRUE);
  //corr->SetBackPropagate(kFALSE);

  // -----   Bremsstrahlung Correction ----------------------
  this->Add(new PndPidBremCorrector()); // 2
  if ((this->GetListOfTasks()->GetSize()-1) != kPndPidBremCorrector) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));

  // -----   MC Cloner   ------------------------------------
  PndMcCloner *clone = NULL;
  this->Add(clone = new PndMcCloner()); // 3
  if ((this->GetListOfTasks()->GetSize()-1) != kPndMcCloner) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  // Option to clean the MCTrack TClonesArray from particles which were not interacting with sensitive detectors
  //clone->SetCleanMc();
  
  // -----   Classifiers   ----------------------------------
  this->Add(new PndPidIdealAssociatorTask()); // 4
  if ((this->GetListOfTasks()->GetSize()-1) != kPndPidIdealAssociatorTask) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  this->Add(new PndPidMvdAssociatorTask()); // 5
  if ((this->GetListOfTasks()->GetSize()-1) != kPndPidMvdAssociatorTask) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  this->Add(new PndPidMdtHCAssociatorTask()); // 6
  if ((this->GetListOfTasks()->GetSize()-1) != kPndPidMdtHCAssociatorTask) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  this->Add(new PndPidDrcAssociatorTask()); // 7
  if ((this->GetListOfTasks()->GetSize()-1) != kPndPidDrcAssociatorTask) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  this->Add(new PndPidDiscAssociatorTask()); // 8
  if ((this->GetListOfTasks()->GetSize()-1) != kPndPidDiscAssociatorTask) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  this->Add(new PndPidSttAssociatorTask()); // 9
  if ((this->GetListOfTasks()->GetSize()-1) != kPndPidSttAssociatorTask) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  this->Add(new PndPidEmcBayesAssociatorTask()); // 10
  if ((this->GetListOfTasks()->GetSize()-1) != kPndPidEmcBayesAssociatorTask) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  this->Add(new PndPidSciTAssociatorTask()); // 11
  if ((this->GetListOfTasks()->GetSize()-1) != kPndPidSciTAssociatorTask) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));

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
