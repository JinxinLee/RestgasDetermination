#include "PndMasterTask.h"

#include <iostream>

// -----   Default constructor   -------------------------------------------
PndMasterTask::PndMasterTask() :
  PndBlackBoxTask("Master  Task")
{ 
}

// -----   Cconstructor -------  -------------------------------------------
PndMasterTask::PndMasterTask(const char* name) :
  PndBlackBoxTask(name)
{
}
// -------------------------------------------------------------------------

/** 
 * @bried Print the list of the task which are included in the list
 * @details This function print the task number, its title and its name. This can be important when it is needed to modify a particular task and the task number is needed. In any case, the use of enum should be preferred.
 * @remark The task names are not well defined in the classes, some of them are missing. We should define a better scheme.
 */
void PndMasterTask::PrintTaskList()
{
  TList* thistasks = this->GetListOfTasks();

  Int_t counter = 0;
  TIter next(thistasks->MakeIterator());
  while (FairTask *task = (FairTask*)next())
    {
      if (counter < 10)
	std::cout << "Task  #" << counter << "\tTitle: " << task->GetTitle() << "\tName: " << task->GetName() << std::endl;
      else
	std::cout << "Task #" << counter << "\tTitle: " << task->GetTitle() << "\tName: " << task->GetName() << std::endl;
      counter++;
    }
 
  return;
}

/** Set the Verbosity of all the tasks at the same number **/
void PndMasterTask::SetVerbose(Int_t iVerbose)
{
  TList* thistasks = this->GetListOfTasks();

  TIter next(thistasks->MakeIterator());
  while (FairTask *task = (FairTask*)next())
    {
      task->SetVerbose(iVerbose);
    }
 
  return;
}

/** Set the Verbosity of a single task  **/
void PndMasterTask::SetVerbose(Int_t nTask, Int_t iVerbose)
{
  TList* thistasks = this->GetListOfTasks();
  ((FairTask*)thistasks->At(nTask))->SetVerbose(iVerbose);
  
  return;
}

/** Set the Persistency of all the tasks in the same way **/
void PndMasterTask::SetPersistency(Bool_t pers)
{
  return;
}

/** Retrieve the pointer to a Task in the list **/
FairTask* PndMasterTask::GetTask(Int_t nTask)
{
  TList* thistasks = this->GetListOfTasks();
  
  return ((FairTask*)thistasks->At(nTask));
}

// -----   Destructor   ----------------------------------------------------
PndMasterTask::~PndMasterTask()
{
}
// -------------------------------------------------------------------------

/** @cond CLASSIMP */
ClassImp(PndMasterTask);
/** @endcond */
