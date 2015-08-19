//--------------------------------------------------------------------------
// Description:
//      Wrapper class for EMC digitization
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndEmcFullDigiTask.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "PndEmcDataTypes.h"

#include "PndEmcHitsToWaveform.h"
#include "PndEmcWaveformToDigi.h"
#include "TClonesArray.h"
#include "TROOT.h"

//---------------
// C++ Headers --
//---------------
#include <iostream>

using std::endl;
using std::cout;
//----------------
// Constructors --
//----------------
PndEmcFullDigiTask::PndEmcFullDigiTask(Int_t verbose, Bool_t persistance):
  FairTask("EMC Full digitization Task"), fVerbose(verbose), fPersistance(persistance)
{
  this->Add(new PndEmcHitsToWaveform());
  this->Add(new PndEmcWaveformToDigi());

  TList* thistasks = this->GetListOfTasks();
  for(Int_t i=0;i<thistasks->GetEntries();i++)
  {
    ((FairTask*)thistasks->At(i))->SetVerbose(fVerbose);
  }
  
	StoreDigi(fPersistance);
}

void PndEmcFullDigiTask::StoreDigi(Bool_t val)
{
  fPersistance=val;
  TList* thistasks = this->GetListOfTasks();
  ((PndEmcHitsToWaveform*)thistasks->At(0))->SetStorageOfData(fPersistance);
  return;
}

void PndEmcFullDigiTask::StoreWaveforms(Bool_t val)
{
	TList* thistasks = this->GetListOfTasks();
	((PndEmcWaveformToDigi*)thistasks->At(1))->SetStorageOfData(val);
	return;
}

//--------------
// Destructor --
//--------------
PndEmcFullDigiTask::~PndEmcFullDigiTask()
{
}

// -----   Public method Init   -------------------------------
InitStatus PndEmcFullDigiTask::Init() {
  return kSUCCESS;
}

void PndEmcFullDigiTask::Exec(Option_t* opt) 
{
	return;
}

void PndEmcFullDigiTask::SetParContainers() {}

ClassImp(PndEmcFullDigiTask)
