/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  pid classifier                    *
 *                                    *
 * Created: 07-04-2010                *
 * Modified:                          *
 *                                    *
 * ************************************/
#include "PndPidEmcAssociatorTask.h"

//==========================================================
/**
 * Default Constructor.
 */
PndPidEmcAssociatorTask::PndPidEmcAssociatorTask()
  : PndPidMvaAssociatorTask("PndPidMvaAssociatorTaskSTD")
{
  std::cout << "<INFO> Default task constructor. " 
	    << "(PndPidEmcAssociatorTask)"<< std::endl;

  const std::vector<std::string>& vNames = SetEmcParNames();
  SetVarNames(vNames);
}

//___________________________________________________________
/**
 * Constructor.
 */
PndPidEmcAssociatorTask::PndPidEmcAssociatorTask(const char *name, const char *title)
  : PndPidMvaAssociatorTask(name)
{
  std::cout << title << std::endl;

  const std::vector<std::string>& vNames = SetEmcParNames();
  SetVarNames(vNames);
}

//! Destructor
PndPidEmcAssociatorTask::~PndPidEmcAssociatorTask()
{}

//! Set Variables to use
void PndPidEmcAssociatorTask::SetVarNames(const std::vector<std::string>& vNames)
{
  std::cerr<< "<INFO> Filling EMC related Variable names." << std::endl;
  PndPidMvaAssociatorTask::SetVarNames(vNames);
}

//! Set Emc related variable names.
const std::vector<std::string>& PndPidEmcAssociatorTask::SetEmcParNames() const
{
  std::vector<std::string>* VarNames = new std::vector<std::string>();
  VarNames->push_back("p");
  VarNames->push_back("emc");
  VarNames->push_back("z20");
  VarNames->push_back("z53");
  VarNames->push_back("lat");

  return (*VarNames);
}
ClassImp(PndPidEmcAssociatorTask)
