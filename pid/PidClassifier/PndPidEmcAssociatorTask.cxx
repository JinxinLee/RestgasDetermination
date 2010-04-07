/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  pid classifier                    *
 *                                    *
 * Created: 23-03-2010                *
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
  std::cout << "<INFO> Call Default task constructor. " 
	    << "(PndPidEmcAssociatorTask)"<< std::endl;
}

//___________________________________________________________
/**
 * Constructor.
 */
PndPidEmcAssociatorTask::PndPidEmcAssociatorTask(const char *name, const char *title)
  : PndPidMvaAssociatorTask(name)
{
  std::cout << title << std::endl;
}

PndPidEmcAssociatorTask::~PndPidEmcAssociatorTask()
{}

void PndPidEmcAssociatorTask::SetVarNames(std::vector<std::string> vNames)
{
  std::cerr<< "<INFO> Filling EMC related Variable names." << std::endl;
  vNames.clear();
  vNames.push_back("p");
  vNames.push_back("emc");
  PndPidMvaAssociatorTask::SetVarNames(vNames);
}
ClassImp(PndPidEmcAssociatorTask)
