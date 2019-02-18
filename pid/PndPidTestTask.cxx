// ************************************************************************
//
// psi(2S) -> J/psi (-> mu+ mu-) pi+ pi- Analysis Example Task
// 
// for the Rho Tutorial, see
// http://panda-wiki.gsi.de/cgi-bin/viewauth/Computing/PandaRootRhoTutorial
//
// K.Goetzen 7/2013
// ************************************************************************


// The header file
#include "PndPidTestTask.h"

// C++ headers
#include <string>
#include <iostream>
#include <iomanip>
#include <numeric>

// FAIR headers
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

// ROOT headers
#include "TClonesArray.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"

// RHO headers
#include "RhoCandidate.h"
#include "RhoCandList.h"
#include "RhoHistogram/RhoTuple.h"
#include "RhoFactory.h"

// Analysis headers
#include "PndAnalysis.h"
		
		
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndPidTestTask::PndPidTestTask() :
  FairTask("Panda Pid Test Task") {

    fPdgIndex[11] = 0;
    fPdgIndex[13] = 1;
    fPdgIndex[211] = 2;
    fPdgIndex[321] = 3;
    fPdgIndex[2212] = 4;
    fPdgIndex[-11] = 5;
    fPdgIndex[-13] = 6;
    fPdgIndex[-211] = 7;
    fPdgIndex[-321] = 8;
    fPdgIndex[-2212] = 9;

    fParticleNames.push_back("Electron");
    fParticleNames.push_back("Muon");
    fParticleNames.push_back("Pion");
    fParticleNames.push_back("Kaon");
    fParticleNames.push_back("Proton");


    std::cout << "fPdgIndex.size()1 = " << fPdgIndex.size() << std::endl;

    for (int i = 0; i < fPdgIndex.size(); i++){
        std::vector<int> vec(fPdgIndex.size(), 0);
        fConfusionMatrix.push_back(vec);
    }

    // *** RhoCandLists for the analysis
    for (int i = 0; i < fPdgIndex.size(); i++){
        fCandLists.push_back(new RhoCandList());
    }
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndPidTestTask::~PndPidTestTask() { }
// -------------------------------------------------------------------------




// -----   Public method Init   --------------------------------------------
InitStatus PndPidTestTask::Init()
{		
	// initialize analysis object
	fAnalysis = new PndAnalysis();
	
	// reset the event counter
	fEvtCount = 0;
	
	return kSUCCESS;
}

// -------------------------------------------------------------------------
	
void PndPidTestTask::SetParContainers()
{
  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
}

// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndPidTestTask::Exec(Option_t*)
{
	// *** some variables
	int j=0;
	
	// necessary to read the next event
	fAnalysis->GetEventInTask();
	
	if (!(++fEvtCount%100)) cout << "evt "<<fEvtCount<<endl;
	

	

	fAnalysis->FillList(*fCandLists[0], fParticleNames[0] + fSelector + "Plus", fClassifier);
	fAnalysis->FillList(*fCandLists[1], fParticleNames[1] + fSelector + "Plus", fClassifier);
	fAnalysis->FillList(*fCandLists[2], fParticleNames[2] + fSelector + "Plus", fClassifier);
	fAnalysis->FillList(*fCandLists[3], fParticleNames[3] + fSelector + "Plus", fClassifier);
    fAnalysis->FillList(*fCandLists[4], fParticleNames[4] + fSelector + "Plus", fClassifier);
    fAnalysis->FillList(*fCandLists[5], fParticleNames[0] + fSelector + "Minus", fClassifier);
    fAnalysis->FillList(*fCandLists[6], fParticleNames[1] + fSelector + "Minus", fClassifier);
    fAnalysis->FillList(*fCandLists[7], fParticleNames[2] + fSelector + "Minus", fClassifier);
    fAnalysis->FillList(*fCandLists[8], fParticleNames[3] + fSelector + "Minus", fClassifier);
    fAnalysis->FillList(*fCandLists[9], fParticleNames[4] + fSelector + "Minus", fClassifier);

    int candListIndex = 0;
    for (auto candidates : fCandLists){
        if (fVerbose > 1) std::cout << "CandList " << candListIndex++ << " Entries: " << candidates->GetLength() << std::endl;
        for (int candInd = 0; candInd < candidates->GetLength(); candInd++){
            RhoCandidate* cand = (*candidates)[candInd];
            if (fVerbose > 1) cand->PrintOn(std::cout);
            if (fVerbose > 1) std::cout << std::endl;
            RhoCandidate* mccand = cand->GetMcTruth();
            if (fVerbose > 1) std::cout << candInd << " Cand: " << cand->PdgCode() << " MCCand " << mccand->PdgCode() << std::endl;
            if (fVerbose > 1) std::cout << *(FairMultiLinkedData_Interface*)mccand << std::endl;
            if (fPdgIndex.count(cand->PdgCode()) > 0 && fPdgIndex.count(mccand->PdgCode()) > 0){
                fConfusionMatrix[fPdgIndex[mccand->PdgCode()]][fPdgIndex[cand->PdgCode()]]++;
            } else {
                std::cout << "-W- PndPidTestTask::Exec Wrong PDG Codes: Cand: " << cand->PdgCode() << " MCCand: " << mccand->PdgCode() << std::endl;
            }
        }
    }
	
}

void PndPidTestTask::PrintConfusionMatrix(bool relative){
    for (int i = 0; i < fPdgIndex.size(); i++){
        std::cout << i << " : " << std::setw(15) << fParticleNames[i%fParticleNames.size()] + ((i < fParticleNames.size()) ? "Plus" : "Minus") << " : ";
        for (int j = 0; j < fPdgIndex.size(); j++){
            if (relative == false)
                std::cout << std::setw(5) << fConfusionMatrix[i][j] << " ";
            else {
                std::cout << std::setw(5);
                int entryCount = std::accumulate(fConfusionMatrix[i].begin(), fConfusionMatrix[i].end(), 0);
                if (entryCount > 0)
                    std::cout << std::fixed << std::setprecision(2) << fConfusionMatrix[i][j] / (double)entryCount << " ";
                else
                    std::cout << std::fixed << std::setprecision(2) << 0.0 << " ";
            }
        }
        std::cout << std::endl;
    }
}


void PndPidTestTask::Finish()
{
    PrintConfusionMatrix(false);
    PrintConfusionMatrix(true);

}

ClassImp(PndPidTestTask)
