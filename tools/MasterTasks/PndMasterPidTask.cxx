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
  pid = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
   
  // -----   Correlation   ---------------------------------
  PndPidCorrelator* corr = NULL;
  this->Add(corr = new PndPidCorrelator()); // 1
  pid.kPndPidCorrelator = GetListOfTasks()->GetSize()-1;
  if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
    {
	  if (fOptions.Contains("filtered")){
		  corr->SetInputBranch("SttMvdGemGenTrack_filtered");
	  } else {
		  corr->SetInputBranch("SttMvdGemGenTrack");
	  }
//      corr->SetInputIDBranch("SttMvdGemGenTrackID");
    }
  else
    {
	  if (fOptions.Contains("filtered")){
		  corr->SetInputBranch("SttMvdGenTrack_filtered");
	  } else {
		  corr->SetInputBranch("SttMvdGenTrack");
	  }
//      corr->SetInputIDBranch("SttMvdGenTrackID");
    }
  corr->SetInputBranch2("FtsIdealGenTrack");
//  corr->SetInputIDBranch2("FtsIdealGenTrackID");
  corr->SetDebugMode(kTRUE);
  //corr->SetFast(kTRUE);
  //corr->SetBackPropagate(kFALSE);

  // -----   Bremsstrahlung Correction ----------------------
  this->Add(new PndPidBremCorrector()); // 2
  pid.kPndPidBremCorrector = GetListOfTasks()->GetSize()-1;
  
  // -----   MC Cloner   ------------------------------------
//  PndMcCloner *clone = NULL;
//  this->Add(clone = new PndMcCloner()); // 3
//  pid.kPndMcCloner = GetListOfTasks()->GetSize()-1;
  // Option to clean the MCTrack TClonesArray from particles which were not interacting with sensitive detectors
  //clone->SetCleanMc();
  
  // -----   Classifiers   ----------------------------------
  this->Add(new PndPidIdealAssociatorTask()); // 4
  pid.kPndPidIdealAssociatorTask = GetListOfTasks()->GetSize()-1;
  
  this->Add(new PndPidMvdAssociatorTask()); // 5
  pid.kPndPidMvdAssociatorTask = GetListOfTasks()->GetSize()-1;
    
  this->Add(new PndPidMdtHCAssociatorTask()); // 6
  pid.kPndPidMdtHCAssociatorTask = GetListOfTasks()->GetSize()-1;
  
  this->Add(new PndPidDrcAssociatorTask()); // 7
  pid.kPndPidDrcAssociatorTask = GetListOfTasks()->GetSize()-1;

  if ( (!fOptions.Contains("day1")) || (fOptions.Contains("gem")) )
    {
      this->Add(new PndPidDiscAssociatorTask()); // 8
      pid.kPndPidDiscAssociatorTask = GetListOfTasks()->GetSize()-1;
    }
 
  this->Add(new PndPidSttAssociatorTask()); // 9
  pid.kPndPidSttAssociatorTask = GetListOfTasks()->GetSize()-1;
    
  this->Add(new PndPidEmcBayesAssociatorTask()); // 10
  pid.kPndPidEmcBayesAssociatorTask = GetListOfTasks()->GetSize()-1;
   
  this->Add(new PndPidSciTAssociatorTask()); // 11
  pid.kPndPidSciTAssociatorTask = GetListOfTasks()->GetSize()-1;

  this->Add(new PndPidFtofAssociatorTask()); // 12
  pid.kPndPidFtofAssociatorTask = GetListOfTasks()->GetSize()-1;

  if ( (!fOptions.Contains("day1")) )
    {
      this->Add(new PndPidRichAssociatorTask()); // 13
      pid.kPndPidRichAssociatorTask = GetListOfTasks()->GetSize()-1;
    }
  
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
