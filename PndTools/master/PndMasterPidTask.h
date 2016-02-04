/**
 * @class PndMasterPidTask
 * @brief The default pid tasks
 * @details # Master Pid Task Class
 * This class includes all the pid tasks which need to be used in the default pid macros. 
 * @remark If you find some obsolete task which needs to be changed, contact the computing coordinator.
 * @author Stefano Spataro <spataro@to.infn.it>, Torino University
 * @version 1.0
 * @date Feb 1, 2016
 **
 **/

#ifndef PNDMASTERPIDTASK_H
#define PNDMASTERPIDTASK_H

#include "PndMasterTask.h"

class TClonesArray;

class PndMasterPidTask : public PndMasterTask
{
 public:

  /** 
   * @brief Default constructor 
   */
  PndMasterPidTask();
  
  /**
   * @brief Destructor
   */
  virtual ~PndMasterPidTask();
  
 private:
  
  /**
   * @brief enum of the task list
   * @detail This enum avoids to retrieve tasks using the integer value (misleading), but uses an easier scheme. The enum is "k" + the class name, i.e. "kPndPidCorrelator" for the class PndPidCorrelator. The function PrintTaskList() can be used to check the list of the tasks and the corresponding number.
   */
  enum pidTaskList {
    kFairGeane,
    kPndPidCorrelator,
    kPndPidBremCorrector,
    kPndMcCloner,
    kPndPidIdealAssociatorTask,
    kPndPidMvdAssociatorTask,
    kPndPidMdtHCAssociatorTask,
    kPndPidDrcAssociatorTask,
    kPndPidDiscAssociatorTask,
    kPndPidSttAssociatorTask,
    kPndPidEmcBayesAssociatorTask,
    kPndEventCounterTask
  };
  
  /** @cond CLASSIMP */
  ClassDef(PndMasterPidTask,1);
  /** @endcond */
};

#endif /* PNDMASTERPIDTASK_H */
