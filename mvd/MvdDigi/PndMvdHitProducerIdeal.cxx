// -------------------------------------------------------------------------
// -----               PndMvdHitProducerIdeal source file             -----
// -------------------------------------------------------------------------


#include "PndMvdHitProducerIdeal.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"


// -----   Default constructor   -------------------------------------------
PndMvdHitProducerIdeal::PndMvdHitProducerIdeal() :
  PndSdsHitProducerIdeal("Ideal MVD Hit Producer")
{
	fInBranchName 	= "MVDPoint";
  fPersistance = kTRUE;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndMvdHitProducerIdeal::~PndMvdHitProducerIdeal()
{
}

void PndMvdHitProducerIdeal::SetBranchNames(TString inBranchname, TString outBranchname, TString folderName)
{
  fInBranchName = inBranchname;
  fOutBranchName = outBranchname;
  fFolderName = folderName;
  SetInBranchId();
}


void PndMvdHitProducerIdeal::SetBranchNames()
{
  fInBranchName = "MVDPoint";
  fOutBranchName = "MVDHit";
  fFolderName = "PndMvd";
  SetInBranchId();
}



ClassImp(PndMvdHitProducerIdeal)
