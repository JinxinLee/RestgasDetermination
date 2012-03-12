//
//  PndAnaPidCombiner.cxx
//  PANDAROOT
//
//  Created by Ralf Kliemt on 12/16/11.
//
//  This class reads the different information from the PID algorithms 
//  and combines them according to the users specifications
//  
//

#include "PndAnaPidCombiner.h"

#include <cmath>
#include "TDatabasePDG.h"

#include "RhoBase/TRho.h"
#include "RhoBase/TCandidate.h"
#include "RhoBase/TCandList.h"

#include "RhoBase/VAbsMicroCandidate.h"

#include "TClonesArray.h"
#include "TPRegexp.h"

#include "PndPidProbability.h"


ClassImp(PndAnaPidCombiner)

PndAnaPidCombiner::PndAnaPidCombiner(const char *name, TString tcanames) : 
TNamed(name,"Panda PID Combiner") ,
fRootManager(0),
fPidArrays(),//FIXME: What should the initializing constructor contain here?
fPidResult(0),
fInitialized(kFALSE)
{
  if(tcanames=="") SetDefaults();
  else SetTcaNames(tcanames);
  fRootManager=FairRootManager::Instance();
  fPidResult = new PndPidProbability();
}

void PndAnaPidCombiner::Init()
{
  // Initialize the TClonesArray lists
  if(fInitialized) return; //if we did initilize, don't do it again.
  for(std::map<TString,TClonesArray*>::iterator iter=fPidArrays.begin();
      iter!=fPidArrays.end();iter++)
  {
    std::cout<<"PidCombiner:Init() Add array'"<<(iter->first).Data()<<"'"<<std::endl;
    iter->second = ReadTCA(iter->first);
  }
  std::cout<<"PidCombiner initialized."<<std::endl;
  fInitialized=kTRUE;
}

Bool_t PndAnaPidCombiner::Apply(TCandList &tcl)
{
  if(!fInitialized) Init();
  Bool_t check;
  Bool_t checkall=kFALSE;
  for (int j=0;j<tcl.GetLength();j++){
    check = Apply(tcl[j]);
    checkall = checkall && check;
  }
  return checkall;
}

Bool_t PndAnaPidCombiner::Apply(TCandidate &tc)
{
  // Apply the multiplied pdf's to the TCandidate
  // If on of the pdf's is not available, it is skipped
  
  if(!fInitialized) Init();
  Bool_t check=kTRUE;
  //TODO: Merge PID info now.
  fPidResult->Reset();
  // combine algorithms
  TClonesArray* aTca=0;
  PndPidProbability* aProb=0;
  Int_t trackIndex = tc.GetTrackNumber();
  std::cout<<"PidCombiner: Try TCandidate uid:"<<tc.Uid()<<" trknr:"<<trackIndex<<std::endl;
  if(trackIndex<0)
  {
    ApplyFlat(tc);
    return kFALSE;
  }
  for(std::map<TString,TClonesArray*>::iterator iter=fPidArrays.begin();
      iter!=fPidArrays.end();iter++)
  {
    aTca=iter->second;
    if(0==aTca){
      Error("Apply", "PID Probability array not found, skip setting pid for candidate %i.",trackIndex);
      check=kFALSE;
      continue;
    }
    if(0==aTca->GetEntriesFast()){
      Error("Apply", "PID Probability array '%s' at %p of size zero, skip setting pid for candidate %i.", aTca->GetName(), aTca, trackIndex);
      continue;
      check=kFALSE;
    }
    if(trackIndex>=aTca->GetEntriesFast()){
      Error("Apply", "Index out of '%s' array (%p) bounds, skip setting pid for candidate %i.",aTca->GetName(),aTca,trackIndex);
      continue;
      check=kFALSE;
    }
    aProb=(PndPidProbability*)aTca->At(trackIndex);
    if(aProb == 0) {
      Error("Apply", "PID Probability object in array '%s' at %p not found, skip setting pid for candidate %i.",aTca->GetName(),aTca,trackIndex);
      continue;
      check=kFALSE;
    }
    if(trackIndex!=aProb->GetIndex()) { 
      Error("Apply", "PID Probability object index (%i) is not the track index (%i). Is that bad?",aProb->GetIndex(),trackIndex);
      continue;
      check=kFALSE;
    }
    
    // catch Zeros to avoid NAN from Div/Zero 
    if(aProb->GetSumProb() == 0) continue;   
    // avoid numerics with low numbers 
    //aProb->NormalizeTo(1.); 
    //now multiply
    *fPidResult *= *aProb;
  }
  
  // renormalizing is done in the Pid object upon request
  // numbering see PndPidListMaker 
  // No flux implemented! To come for each Detector!
  tc.SetPidInfo(0,fPidResult->GetElectronPidProb());
  tc.SetPidInfo(1,fPidResult->GetMuonPidProb());
  tc.SetPidInfo(2,fPidResult->GetPionPidProb());
  tc.SetPidInfo(3,fPidResult->GetKaonPidProb());
  tc.SetPidInfo(4,fPidResult->GetProtonPidProb());
  
  return check;
}

void PndAnaPidCombiner::ApplyFlat(TCandidate &tc)
{
  tc.SetPidInfo(0,1.);
  tc.SetPidInfo(1,1.);
  tc.SetPidInfo(2,1.);
  tc.SetPidInfo(3,1.);
  tc.SetPidInfo(4,1.);
  return;
}


void PndAnaPidCombiner::SetDefaults()
{
  // Set list of names and weights to the default PANDA
  //TString names = "PidAlgoMvd;PidAlgoStt;PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoMdtHardCuts;";
  TString names = "PidAlgoIdealCharged";
  //TString names = "PidMvaChargedProbability";
  SetTcaNames(names);
  fInitialized=kFALSE;
  return;
}

void PndAnaPidCombiner::SetTcaNames(TString &names)
{
  fPidArrays.clear();
  // Tokenizer, cool thingy!
  TStringToken list(names,";"); 
  //use TString class part (inherited, Tokenizer stores data there)
  while(list.NextToken()) AddTcaName( (TString)list ); 
  fInitialized=kFALSE;
  return;
}

TClonesArray* PndAnaPidCombiner::ReadTCA(const TString &tcaname)
{
  // Fetch a TCLonesArray from the framework by its root name
  
  TClonesArray* tca = (TClonesArray*) fRootManager->GetObject(tcaname.Data());
  if (! tca) 
    Warning("PndAnaPidCombiner::ReadTCA()","No \"%s\" array found.",tcaname.Data());
  return tca;
}
