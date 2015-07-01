/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  pid classifier                    *
 *                                    *
 * Created: 07-04-2010                *
 * Modified:                          *
 *                                    *
 * ************************************/
//#pragma once
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
  
  // Set the default variable names for emc
  void SetVarNames(std::vector<std::string> const& vNames);
  
  /*
   * Creates a vector with names of the statndard variables for emc
   * pat. rec.
  */
  std::vector<std::string> const* SetEmcParNames() const;

  ClassDef(PndPidEmcAssociatorTask, 0);
};
#endif//End of interface definition (PndPidEmcAssociatorTask)
