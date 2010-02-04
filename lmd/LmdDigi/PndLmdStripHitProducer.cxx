// -------------------------------------------------------------------------
// -----                PndLmdStripHitProducer source file             -----
// -------------------------------------------------------------------------

#include "PndLmdStripHitProducer.h"

// -----   Default constructor   -------------------------------------------
PndLmdStripHitProducer::PndLmdStripHitProducer()
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndLmdStripHitProducer::~PndLmdStripHitProducer()
{
  delete fGeoH;
}
// -------------------------------------------------------------------------

void PndLmdStripHitProducer::SetBranchNames(TString inBranchname, TString outBranchname, TString folderName)
{
  fBranchName = inBranchname;
  fOutBranchName = outBranchname;
  fFolderName = folderName;
}

void PndLmdStripHitProducer::SetBranchNames()
{
  fBranchName = "LMDPoint";
  fOutBranchName = "LMDStripDigis";
  fFolderName = "PndLmd";
}

ClassImp(PndLmdStripHitProducer);

