// -------------------------------------------------------------------------
// -----                PndLmdStripClusterTask source file             -----
// -------------------------------------------------------------------------

//LUMI
#include "PndLmdStripClusterTask.h"
//PANDA
#include "PndSdsContFact.h"
//FAIR
#include "FairRun.h"
#include "FairRuntimeDb.h"
//ROOT
#include "TList.h"

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

void PndLmdStripClusterTask::SetParContainers()
{
  // called from the FairRun::Init()
  // Caution: The Parameter Set is not filled from the DB IO, yet.
  // This will be done just before this Tasks Init() is called.

  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  PndSdsContFact* thelmdcontfact = (PndSdsContFact*)rtdb->getContFactory("PndSdsContFact");
  TList* theContNames = thelmdcontfact->GetDigiParNames();
  Info("SetParContainers()","The container names list contains %i entries",theContNames->GetEntries());
  TIter cfIter(theContNames);
  while (TObjString* contname = (TObjString*)cfIter()) {
    TString parsetname = contname->String();
    Info("SetParContainers()",parsetname.Data());
    if(parsetname.BeginsWith("MVDStripDigiPar")){
      PndSdsStripDigiPar* digipar = (PndSdsStripDigiPar*)(rtdb->getContainer(parsetname.Data()));
      fDigiParameterList->Add(digipar);
    }
  }

}

ClassImp(PndLmdStripClusterTask);

