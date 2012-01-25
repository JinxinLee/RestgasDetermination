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
TNamed(name,"Panda PID Combiner") 
{
  if(tcanames=="") SetDefaults();
  else SetTcaNames(tcanames);
  fRootManager=FairRootManager::Instance();
}

void PndAnaPidCombiner::Init()
{
  // Initialize the TClonesArray lists
  
  for(std::map<TString,TClonesArray*>::iterator iter=fPidArrays.begin();
      iter!=fPidArrays.end();iter++)
  {
    iter->second = ReadTCA(iter->first);
  }
}

Bool_t PndAnaPidCombiner::Apply(TCandList &tcl)
{
  Bool_t check;
  for (int j=0;j<tcl.GetLength();++j){
    check = check && Apply(tcl[j]);
  }
  return check;
}

Bool_t PndAnaPidCombiner::Apply(TCandidate &tc)
{
  //TODO: Merge PID info now.
  fPidResult->Reset();
  // combine algorithms
  TClonesArray* aTca=0;
  PndPidProbability* aProb=0;
  Int_t trackIndex = tc.GetTrackNumber();
  for(std::map<TString,TClonesArray*>::iterator iter=fPidArrays.begin();
      iter!=fPidArrays.end();iter++)
  {
    aTca=iter->second;
    if(0==aTca){
      Error("Apply", "PID Probability array not found, skip setting pid for candidate %i.",trackIndex);
      return kFALSE;
    }
    if(trackIndex>=aTca->GetEntriesFast()){
      Error("Apply", "Index tout of array bounds, skip setting pid for candidate %i.",trackIndex);
      return kFALSE;
    }
    aProb=(PndPidProbability*)aTca->At(trackIndex);
    if(aProb == 0) {
      Error("Apply", "PID Probability object not found, skip setting pid for candidate %i.",trackIndex);
      return kFALSE;
    }
    if(trackIndex!=aProb->GetIndex()) { 
      Error("Apply", "PID Probability object index (%i) is not the track index (%i). Is that bad?",aProb->GetIndex(),trackIndex);
      return kFALSE;// should we check the numbers?
    }
    
    //now multiply
    
    *fPidResult *= *aProb;
    
    // renormalizing is done in the Pid object upon request
  }
  
  // numbering see PndPidListMaker 
  // No flux implemented! To come for each Detector!
  tc.SetPidInfo(0,fPidResult->GetElectronPidProb());
  tc.SetPidInfo(1,fPidResult->GetMuonPidProb());
  tc.SetPidInfo(2,fPidResult->GetPionPidProb());
  tc.SetPidInfo(3,fPidResult->GetKaonPidProb());
  tc.SetPidInfo(4,fPidResult->GetProtonPidProb());
  return kTRUE;
}

void PndAnaPidCombiner::SetDefaults()
{
  // Set list of names and weights to the default PANDA
  //TString names = "PidAlgoMvd;PidAlgoStt;PidAlgoEmcBayes;PidAlgoDrc;PidAlgoDisc;PidAlgoMdtHardCuts;";
  TString names = "PidAlgoIdealCharged";
  //TString names = "PidMvaChargedProbability";
  SetTcaNames(names);
  return;
}

void PndAnaPidCombiner::SetTcaNames(TString &names)
{
  // Tokenizer, cool thingy!
  TStringToken list(names,";"); 
  //use TString class part (inherited, Tokenizer stores data there 
  while(list.NextToken()) AddTcaName( (TString)list ); 
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
