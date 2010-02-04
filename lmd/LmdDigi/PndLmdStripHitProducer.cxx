// -------------------------------------------------------------------------
// -----                PndLmdStripHitProducer source file             -----
// -------------------------------------------------------------------------

//LUMI
#include "PndLmdStripHitProducer.h"
//PANDA
#include "PndSdsContFact.h"
//FAIR
#include "FairRun.h"
#include "FairRuntimeDb.h"
//ROOT
#include "TList.h"

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

void PndLmdStripHitProducer::SetParContainers()
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

ClassImp(PndLmdStripHitProducer);

