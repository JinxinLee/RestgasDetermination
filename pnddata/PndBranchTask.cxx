// -------------------------------------------------------------------------
// -----                  PndBranchTask source file                    -----
// -----             Created 28/02/18  by T.Stockmanns                 -----
// -------------------------------------------------------------------------
#include "PndBranchTask.h"



// -----   Default constructor   -------------------------------------------
PndBranchTask::PndBranchTask() : fPersistency(kFALSE)
{
}

// -------------------------------------------------------------------------

PndBranchTask::PndBranchTask(const char* name, Int_t iVerbose) : FairTask(name, iVerbose), fPersistency(kFALSE)
{

}

// -----   Destructor   ----------------------------------------------------
PndBranchTask::~PndBranchTask() {
}
// -------------------------------------------------------------------------


ClassImp(PndBranchTask)
