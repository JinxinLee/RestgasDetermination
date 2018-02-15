/*
 * PndEventCombinerTask.cxx
 *
 *  Created on: july 13, 2017
 *      Author: Steinschaden
 *
 */

#include "PndBranchBurstBuilder.h"

#include <iostream>

// Root includes
#include "TROOT.h"
#include "TString.h"

// framework includes
#include "FairRootManager.h"



PndBranchBurstBuilder::PndBranchBurstBuilder() :
  FairTask("PndBranchBurstBuilder")
{
  fPersistence = kTRUE;
  fOutputPrefix = "Burst";
}

PndBranchBurstBuilder::~PndBranchBurstBuilder()
{
}


// -----   Public method Init   --------------------------------------------
InitStatus PndBranchBurstBuilder::Init()
{



  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndBranchBurstBuilder::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  if(fInBranchNames.size()==0) {
    std::cout << "kERROR ------  no inputBranch for PndBranchBurstBuilder -------" << std::endl;
    return kERROR;
  }

  TString inputName = "none";
  TString outputFolder = "Burst";
  TString outputName;

  for (size_t i = 0; i < fInBranchNames.size(); i++){
    inputName = fInBranchNames[i];
    outputName  = fOutputPrefix + "_" + inputName;
    fInArrays.push_back((TClonesArray*)ioman->GetObject(inputName.Data()));
    if ( ! fInArrays[i]){
      std::cout << " Input Array not found for PndBranchBurstBuilder:  "<< inputName << std::endl;
      return kERROR;
    }

    fOutArrays.push_back(ioman->Register(outputName.Data(), fInArrays[i]->GetClass()->GetName(), outputFolder, fPersistence));
  }

  return kSUCCESS;
}


void PndBranchBurstBuilder::Exec(Option_t*)
{

  for (size_t i = 0; i < fInBranchNames.size(); i++) {

	  fOutArrays[i]->Delete(); // make sure data written out from old events are deleted
	  fOutArrays[i]->AbsorbObjects(GetBurstData(i));

  }

}



ClassImp(PndBranchBurstBuilder);

