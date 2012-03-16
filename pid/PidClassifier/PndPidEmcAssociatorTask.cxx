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
	    << "(PndPidEmcAssociatorTask)\n";
  
  std::vector<std::string> const* vNames = SetEmcParNames();
  SetVarNames(*vNames);
  delete vNames;
}

//___________________________________________________________
/**
 * Constructor.
 */
PndPidEmcAssociatorTask::PndPidEmcAssociatorTask(char const* name, char const* title)
  : PndPidMvaAssociatorTask(name)
{
  std::cout << title << '\n';
  
  std::vector<std::string> const* vNames = SetEmcParNames();
  SetVarNames(*vNames);
  delete vNames;
}

//! Destructor
PndPidEmcAssociatorTask::~PndPidEmcAssociatorTask()
{}

//! Set Variables to use
void PndPidEmcAssociatorTask::SetVarNames(std::vector<std::string> const& vNames)
{
  std::cout<< "<INFO> Filling EMC related Variable names.\n";
  
  PndPidMvaAssociatorTask::SetVarNames(vNames);
}

//! Set Emc related variable names.
std::vector<std::string> const* PndPidEmcAssociatorTask::SetEmcParNames() const
{
  std::vector<std::string>* VarNames = new std::vector<std::string>();
  VarNames->push_back("emc");
  VarNames->push_back("lat");
  VarNames->push_back("z20");
  VarNames->push_back("z53");
  VarNames->push_back("e1");
  VarNames->push_back("e9");
  VarNames->push_back("e25");
  
  return VarNames;
}
ClassImp(PndPidEmcAssociatorTask)
