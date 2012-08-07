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
  PndPidEmcAssociatorTask(char const* name);
  
  /**
   * Destructor.
   */  
  virtual ~PndPidEmcAssociatorTask();
  
  //=============== Private members.
 private:
  // Copy const.
  PndPidEmcAssociatorTask(PndPidMvaAssociatorTask const& other);

  // Assignment operator.
  PndPidEmcAssociatorTask& operator=(PndPidMvaAssociatorTask const& other);
  
  void SetVarNames(std::vector<std::string> const& vNames);

  std::vector<std::string> const* SetEmcParNames() const;

  ClassDef(PndPidEmcAssociatorTask, 0);
};
#endif//End of interface definition (PndPidEmcAssociatorTask)
