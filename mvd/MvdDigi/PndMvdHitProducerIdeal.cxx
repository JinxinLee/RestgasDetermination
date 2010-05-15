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
	fBranchName 	= "MVDPoint";
  fPersistance = kTRUE;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndMvdHitProducerIdeal::~PndMvdHitProducerIdeal()
{
}

void PndMvdHitProducerIdeal::SetBranchNames(TString inBranchname, TString outBranchname, TString folderName)
{
  fBranchName = inBranchname;
  fOutBranchName = outBranchname;
  fFolderName = folderName;
}


void PndMvdHitProducerIdeal::SetBranchNames()
{
  fBranchName = "MVDPoint";
  fOutBranchName = "MVDHit";
  fFolderName = "PndMvd";
}



ClassImp(PndMvdHitProducerIdeal)
