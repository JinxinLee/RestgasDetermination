/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  pid classifier                    *
 *                                    *
 * Created: 23-03-2010                *
 * Modified:                          *
 *                                    *
 * ************************************/
#ifndef PID_EMC_ASSOCIATOR_H
#define PID_EMC_ASSOCIATOR_H

#include "PndPidMvaAssociatorTask.h"

class PndPidEmcAssociatorTask: public PndPidMvaAssociatorTask
{
 public:
  /**
   * Default Constructor.
   */
  PndPidEmcAssociatorTask();
  
  /**
   * Constructor.
   */
  PndPidEmcAssociatorTask(const char *name, const char *title="PndPidEmcAssociatorTask");
  
  /**
   * Destructor.
   */  
  virtual ~PndPidEmcAssociatorTask();
  
  virtual void SetVarNames(std::vector<std::string> vNames);  
  
  //=============== Private members.
 private:
  // Copy const.
  PndPidEmcAssociatorTask(const PndPidMvaAssociatorTask& other);
  // Assignment operator.
  PndPidEmcAssociatorTask& operator=(const PndPidMvaAssociatorTask& other);
  
  ClassDef(PndPidEmcAssociatorTask, 1)
};
#endif//End of interface definition (PndPidEmcAssociatorTask)
