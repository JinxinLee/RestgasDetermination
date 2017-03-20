#include "PndSolCorrTask.h"

#include <iostream>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TVector3.h"
#include "TMath.h"

// framework includes
#include "FairRootManager.h"



PndSolCorrTask::PndSolCorrTask() :
  FairTask("Pnd SolCorr")
{
  fPersistence = kTRUE;
  fRunContinuous = kTRUE;
  fOutBranchName = "SolCorr";
  //fInBranches
}

PndSolCorrTask::~PndSolCorrTask()
{
}


// -----   Public method Init   --------------------------------------------
InitStatus PndSolCorrTask::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndSolCorrTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  if(fInBranches.size()==0) {
    std::cout << "kERROR ------  no inputBranch for PndSolCorrTask -------" << std::endl;
    return kERROR;   
}

  fOutArray = ioman->Register(fOutBranchName.Data(), "FairTimeStamp", "EventDetermination", fPersistence);
 

  TString inputName = "none";
  for (size_t i = 0; i < fInBranches.size(); i++){
    inputName = fInBranches[i];
    fInArrays.push_back((TClonesArray*)ioman->GetObject(inputName.Data()));
    if ( ! fInArrays[i]){
      std::cout << " Input Array not found for PndSolCorrTask:  "<< inputName << std::endl;
      return kERROR;
    } 
  } 

  fStopFunctor= new StopTime();
  fReadOutStop = 0;



  return kSUCCESS;
}


// -----   Public method Exec   --------------------------------------------
void PndSolCorrTask::Exec(Option_t*)
{
  // Declare some variables

  FairHit *inHit;
  FairTimeStamp *outStamp;

  // Reset output array

  fOutArray->Clear();

  if(fRunContinuous==kFALSE){  // ---  for Event based Simulation 
  // Loop over Input FairHits 
    for (size_t i = 0; i < fInArrays.size(); i++){
      for (int j = 0; j < fInArrays[i]->GetEntriesFast(); j++){
	inHit = (FairHit*) fInArrays[i]->At(j);
 	outStamp = SolCorr(inHit);
	fDataCorr_map.insert(std::pair<double,FairTimeStamp*>(outStamp->GetTimeStamp(),outStamp)); 
      }    
    }
  
    // --- write out all the Data for the Event ------
    typedef std::multimap<double, FairTimeStamp*>::iterator DataIter;
    for (DataIter it = fDataCorr_map.begin(); it != fDataCorr_map.end();it++){
      new ((*fOutArray)[fOutArray->GetEntries()]) FairTimeStamp(*(it->second));
    }
    fDataCorr_map.clear();    
  }
  else{  // for time based simulation-----------------------------------------------------------
    
    double time;

    for (size_t i = 0; i < fInArrays.size(); i++){
      if (fInArrays[i]->GetEntriesFast()>0){
	inHit = (FairHit*) fInArrays[i]->At(fInArrays[i]->GetEntriesFast()-1);
	time = inHit->GetTimeStamp();
	if ( fReadOutStop < time) fReadOutStop = time;
      }
    }


    for (size_t i = 0; i < fInBranches.size(); i++){
     
      TClonesArray* tempArray = FairRootManager::Instance()->GetData(fInBranches[i],fStopFunctor,fReadOutStop);

      for (int j = 0; j < tempArray->GetEntriesFast(); j++){
	inHit = (FairHit*) tempArray->At(j);
	outStamp = SolCorr(inHit);
	fDataCorr_map.insert(std::pair<double,FairTimeStamp*>(outStamp->GetTimeStamp(),outStamp)); 
      }  
      tempArray->Clear(); 
    }   

    // --- write out the Data which can't be affected anymore (1000 ns safty margin) ------

    typedef std::multimap<double, FairTimeStamp*>::iterator DataIter;
    for (DataIter it = fDataCorr_map.begin(); it != fDataCorr_map.lower_bound(fReadOutStop - 1000);it++){
      new ((*fOutArray)[fOutArray->GetEntries()]) FairTimeStamp(*(it->second));
    }
    fDataCorr_map.erase(fDataCorr_map.begin(),fDataCorr_map.lower_bound(fReadOutStop - 1000));  

  }


}
  
void PndSolCorrTask::FinishTask(){

  // write out the last data in the DataCorr_map ---------

  if (IsActive()==kFALSE) return;

  if(fRunContinuous==kTRUE){
    fOutArray->Clear();
    typedef std::multimap<double, FairTimeStamp*>::iterator DataIter;
    for (DataIter it = fDataCorr_map.begin(); it != fDataCorr_map.end();it++){
      new ((*fOutArray)[fOutArray->GetEntries()]) FairTimeStamp(*(it->second));
    }
    fDataCorr_map.clear(); 
    FairRootManager::Instance()->SetLastFill();
  }

}

FairTimeStamp* PndSolCorrTask::SolCorr(FairHit* inHit){

  double time, time_corr, trackLength;
  TVector3 hitPos;
  FairMultiLinkedData* links = NULL;
  FairTimeStamp* outStamp;

  inHit->Position(hitPos);
  trackLength = hitPos.Mag();
  time = inHit->GetTimeStamp();
  time_corr = time - (trackLength/100/TMath::C()*1000000000 ); // trackLength in cm, timeStamps in ns!
  outStamp = new FairTimeStamp(time_corr); 
  outStamp->SetLink(inHit->GetEntryNr());
  links = inHit->GetPointerToLinks();
  if(links != NULL) outStamp->AddLinks(*links);

  return outStamp;

}

ClassImp(PndSolCorrTask);

