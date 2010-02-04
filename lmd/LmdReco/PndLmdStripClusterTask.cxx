// -------------------------------------------------------------------------
// -----                PndLmdStripClusterTask source file             -----
// -------------------------------------------------------------------------


#include "PndLmdStripClusterTask.h"


// -----   Default constructor   -------------------------------------------

PndLmdStripClusterTask::PndLmdStripClusterTask() :
  PndSdsStripClusterTask()
{
/*  fChargeCut = 1.e8; // this ist really large and shall have no effect
  fGeoFile = "";
  fClusterMod=ClusterMod;
  if(fClusterMod>1 || fClusterMod<0) fClusterMod=0;
  fRadChannel=RadChannel;
  fRadTime=RadTime;*/
}


// -----   Destructor   ----------------------------------------------------
PndLmdStripClusterTask::~PndLmdStripClusterTask()
{
  //if(0!=fGeoH)  delete fGeoH;
  //if(0!=fChargeAlgos) delete fChargeAlgos;
}
// -------------------------------------------------------------------------

void PndLmdStripClusterTask::SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName)
{
  fBranchName = inBranchname;
  fHitBranchName = outHitBranchname;
  fClustBranchName = outClustBranchname;
  fFolderName = folderName;
}

void PndLmdStripClusterTask::SetBranchNames()
{
  fBranchName = "LMDStripDigis";
  fHitBranchName = "LMDHitsStrip";
  fClustBranchName = "LMDStripClusterCand";
  fFolderName = "PndLmd";
}

ClassImp(PndLmdStripClusterTask);

