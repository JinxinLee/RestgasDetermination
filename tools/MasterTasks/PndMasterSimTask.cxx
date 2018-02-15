// -------------------------------------------------------------------------
// -----                    PndMasterSimTask source file               -----
// -----                   Created 04/02/16  by S. Spataro             -----
// -----                       Wrapper for  sim tasks                  -----
// -------------------------------------------------------------------------


#include "PndMasterSimTask.h"
#include "PndMasterTask.h"

#include "PndEmcHitProducer.h"

/**
 * @brief Default Constructor
 * @details # Sim task list
 * Here all the sim tasks are added to the task, with the standard settings. A check is done after each task if the tasklist enum is broken or not. At the end the event counter is added (each 100 events), and the verbosity is set to 0 to all the tasks (it can be changed afterwards with SetVerbosity() functions.
 **/
// -----   Default constructor   -------------------------------------------
PndMasterSimTask::PndMasterSimTask() :
  PndMasterTask("Master Sim Task")
{
  // -----   Emc hit Producer   ----------------------------
  this->Add(new PndEmcHitProducer()); // 0
  if ((this->GetListOfTasks()->GetSize()-1) != kPndEmcHitProducer) Error("PndMasterDigiTask","Error in task #%i", (this->GetListOfTasks()->GetSize()-1));
  
  SetVerbose(0);
}
// -------------------------------------------------------------------------

/** Set the Persistency of all the tasks in the same way **/
void PndMasterSimTask::SetPersistency(Bool_t pers)
{
  ((PndEmcHitProducer*)GetListOfTasks()->At(kPndEmcHitProducer))->SetStorageOfData(pers);
  
  return;
}

// -----   Destructor   ----------------------------------------------------
PndMasterSimTask::~PndMasterSimTask()
{
}
// -------------------------------------------------------------------------

/** @cond CLASSIMP */
ClassImp(PndMasterSimTask);
/** @endcond */
