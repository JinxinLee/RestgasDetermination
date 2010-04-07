/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  pid classifier                    *
 *                                    *
 * Created: 07-04-2010                *
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
  
  //=============== Private members.
 private:
  // Copy const.
  PndPidEmcAssociatorTask(const PndPidMvaAssociatorTask& other);
  // Assignment operator.
  PndPidEmcAssociatorTask& operator=(const PndPidMvaAssociatorTask& other);
  
  void SetVarNames(const std::vector<std::string>& vNames);
  const std::vector<std::string>& SetEmcParNames()const;

  ClassDef(PndPidEmcAssociatorTask, 1)
};
#endif//End of interface definition (PndPidEmcAssociatorTask)
