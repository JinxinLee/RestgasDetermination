#include <iostream>
#include "EvtGenTask.h"
#include "CbmRootManager.h"
#include "TClonesArray.h"
using namespace std;


EvtGenTask::EvtGenTask()
{
}

EvtGenTask::~EvtGenTask()
{
}


InitStatus EvtGenTask::Init()
{
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman ) {
    std::cout << "-E- EvtGenTask::Init: "
         << "RootManager not instantised!" << std::endl;
    return kFATAL;
  }



  fEvtArray = new TClonesArray("Event");
  ioman->Register("Event","gpid", fEvtArray, kTRUE);
  return kSUCCESS;
}

 void EvtGenTask::Exec(Option_t* opt )
{
  TClonesArray& clref2 = *fEvtArray;
  Int_t size2 = clref2.GetEntriesFast();
  Event *fEvt = new(clref2[size2]) Event();
  Float_t var1,var2,var3;

    var1 =  fRan.BreitWigner(750,100);
    var2  =  fRan.Gaus(10,2.5);
    var3  =  fRan.Poisson(10);
  fEvt->Set("vara",var1);
  fEvt->Set("varb",var2);
  fEvt->Set("varc",var3);

}

ClassImp(EvtGenTask);
