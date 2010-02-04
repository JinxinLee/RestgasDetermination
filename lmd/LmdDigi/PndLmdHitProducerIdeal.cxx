// -------------------------------------------------------------------------
// -----               PndLmdHitProducerIdeal source file             -----
// -------------------------------------------------------------------------

#include "PndLmdHitProducerIdeal.h"

// -----   Default constructor   -------------------------------------------
PndLmdHitProducerIdeal::PndLmdHitProducerIdeal()
{

}
// -------------------------------------------------------------------------

void PndLmdHitProducerIdeal::SetBranchNames(TString inBranchname, TString outBranchname, TString folderName)
{
  fBranchName = inBranchname;
  fOutBranchName = outBranchname;
  fFolderName = folderName;
}


void PndLmdHitProducerIdeal::SetBranchNames()
{
  fBranchName = "LMDPoint";
  fOutBranchName = "LMDHit";
  fFolderName = "PndLmd";
}

// -----   Destructor   ----------------------------------------------------
PndLmdHitProducerIdeal::~PndLmdHitProducerIdeal()
{
}


ClassImp(PndLmdHitProducerIdeal);
