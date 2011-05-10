//
//  This class COPIES PndTrack objects from all specified input branches
//  into one output branch. Linking is still to be done...
//
//  Aurthor: R.Kliemt, May 2011

#include "PndTrackArrayMerger.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include <iostream>


PndTrackArrayMerger::PndTrackArrayMerger()
:fPersistance(kTRUE),
fOutputBranch("ALLTracks")
{}

PndTrackArrayMerger::PndTrackArrayMerger(TString s)
:fPersistance(kTRUE)
{fOutputBranch = s;}

PndTrackArrayMerger::~PndTrackArrayMerger()
{}

void PndTrackArrayMerger::SetParContainers()
{return;}

InitStatus PndTrackArrayMerger::ReInit()
{return kSUCCESS;}


InitStatus PndTrackArrayMerger::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman )
  {
    std::cout << "-E- PndSdsStripHitProducer::Init: "
    << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  
  //setup input arrays
  TClonesArray* tmparray;
  for(std::vector<TString>::iterator iter = fInputBranchList.begin(); iter!=fInputBranchList.end();++iter)
  {
    tmparray = (TClonesArray*) ioman->GetObject((*iter).Data());
    if ( ! tmparray )
    {
      Error("Init()","No %s array! Skipping that name.",(*iter).Data());
      continue;
    }
    fInputArrayList.push_back(tmparray);    
  }
  
  //setup output array
  fOutputArray = new TClonesArray("PndTrack");
  ioman->Register(fOutputBranch, "AllTracks", fOutputArray, fPersistance);
  
  return kSUCCESS;
}

void PndTrackArrayMerger::Exec(Option_t* opt)
{
  // copy data from input arrays to output array
  TClonesArray* tmparray;
  PndTrack* tmptrk;
  Int_t namenum=0;
  TString brname;
  Int_t entries=0;
  for(std::vector<TClonesArray*>::iterator iter = fInputArrayList.begin(); iter!=fInputArrayList.end();++iter)
  {
    brname=fInputBranchList[namenum];
    tmparray=*iter;
    for ( Int_t i=0;i<tmparray->GetEntriesFast();i++)
    {
      tmptrk=(PndTrack*)tmparray->At(i);
      entries=fOutputArray->GetEntriesFast();
      PndTrack* mynewtrack = new ((*fOutputArray)[entries]) 
           PndTrack(tmptrk->GetParamFirst(),tmptrk->GetParamLast(),*(tmptrk->GetTrackCandPtr()),
                    tmptrk->GetFlag(), tmptrk->GetChi2(), tmptrk->GetNDF(), 
                    tmptrk->GetPidHypo(),-1,-1);
      mynewtrack->Reset(); //resetting links 
      for(int nlin=0;nlin<tmptrk->GetNLinks();nlin++)
      {
        mynewtrack->AddLink(tmptrk->GetLink(nlin));
      }
    }
    namenum++;
  }
  return;
}

void PndTrackArrayMerger::FinishEvent()
{
  // called after all Tasks did their Exex() and the data is copied to the file
  fOutputArray->Clear();
  FinishEvents();
}

ClassImp(PndTrackArrayMerger);

