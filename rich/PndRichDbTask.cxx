#include "PndRichDbTask.h"
#include "PndRichReco.h"

// fairroot
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeanePro.h"
#include "FairGeaneUtil.h"
#include "FairTrackParP.h"

// general
#include <iostream>
#include <cmath>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndRichDbTask::PndRichDbTask() :
  FairTask("Rich Reco task") { 
  fPersistence = kTRUE;
  fVerbose = 1;
  
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndRichDbTask::~PndRichDbTask() { 
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndRichDbTask::Init() {

  FairRootManager *ioman = FairRootManager::Instance();
  
  if (!ioman) 
   {
      cout << "-E- PndRichDbTask: "
         << "RootManager not instantised!" << endl;
      return kFATAL;
   }

  cout << "-I- PndRichDbTask: Intialisation successfull " << endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndRichDbTask::Exec(Option_t* opt) {
  if(fVerbose > 0) {
    //cout << "==================== EVENT " << evt << endl;
  }
}

void PndRichDbTask::FinishEvent()
{
}

void PndRichDbTask::FinishTask()
{
}

ClassImp(PndRichDbTask)
