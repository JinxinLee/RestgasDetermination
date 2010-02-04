// -------------------------------------------------------------------------
// -----                PndLmdIdealRecoTask source file                -----
// -----                 Created 04/02/10  by M.Michel                 -----
// -----             Based on PndMvdIdealRecoTask by R.Kliemt          -----
// -------------------------------------------------------------------------

// framework includes
#include "PndLmdIdealRecoTask.h"

// -----   Default constructor   -------------------------------------------
PndLmdIdealRecoTask::PndLmdIdealRecoTask()
{
  fSigmaX=0.;
  fSigmaY=0.;
  fSigmaZ=0.;
}
// -------------------------------------------------------------------------

// -----   Constructor   ---------------------------------------------------
PndLmdIdealRecoTask::PndLmdIdealRecoTask(Double_t sx, Double_t sy, Double_t sz) :
  PndSdsIdealRecoTask(sx, sy, sz)
{
  fSigmaX=sx;
  fSigmaY=sy;
  fSigmaZ=sz;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndLmdIdealRecoTask::~PndLmdIdealRecoTask()
{

}

void PndLmdIdealRecoTask::SetBranchNames(TString inBranchname, TString outHitBranchname, TString folderName)
{
  fBranchName = inBranchname;
  fHitBranchName = outHitBranchname;
  fFolderName = folderName;
}

void PndLmdIdealRecoTask::SetBranchNames()
{
  fBranchName = "LMDPoint";
  fHitBranchName = "LMDHit";
  fFolderName = "PndLmd";
}

ClassImp(PndSdsIdealRecoTask);
