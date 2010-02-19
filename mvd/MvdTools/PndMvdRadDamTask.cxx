/*
 * PndMvdRadDamTask.cxx
 *
 *  Created on: Dec 16, 2008
 *      Author: stockman
 */

#include "PndMvdRadDamTask.h"

#include "FairRootManager.h"
#include "PndMvdMCPoint.h"
#include "PndMCTrack.h"
#include "PndMvdRadDamHit.h"
#include "PndStringVector.h"


#include <iostream>

PndMvdRadDamTask::PndMvdRadDamTask()
{
}

PndMvdRadDamTask::~PndMvdRadDamTask()
{
	for (std::map<std::string, TH2*>::iterator it = fMapDetHistos.begin(); it != fMapDetHistos.end(); it++)
			delete(it->second);
	fMapDetHistos.clear();
	delete (fRadDamHisto);
}


void PndMvdRadDamTask::SetParContainers()
{
}

InitStatus PndMvdRadDamTask::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

InitStatus PndMvdRadDamTask::Init()
{
	FairRootManager* ioman = FairRootManager::Instance();

	if (!ioman)
	{
		std::cout << "-E- PndMvdRadDamTask::Init: "
				<< "RootManager not instantiated!" << std::endl;
		return kFATAL;
	}

	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if (!fMCTracks)
	{
		std::cout << "-W- PndMvdRadDamTask::Init: " << "No MCTrack array!"
				<< std::endl;
		return kERROR;
	}

	fMCHits = (TClonesArray*) ioman->GetObject("MVDPoint");
	if (!fMCHits)
	{
		std::cout << "-W- PndMvdRadDamTask::Init: " << "No MVDPoint array!"
				<< std::endl;
		return kERROR;
	}

	// Create and register output array
	fRadDamHits = new TClonesArray("PndMvdRadDamHit");
	ioman->Register("MVDRadDamHit", "MVD", fRadDamHits, fPersistance);

	InitWeightLists();
	fGeoH = new PndMvdGeoHandling(gGeoManager);
	fRadDamHisto = new TH1D("radDamH","Weight Factors", 1000,0,100);

	std::cout << "-I- PndMvdRadDamTask: Initialization successful" << std::endl;

	return kSUCCESS;
}

void PndMvdRadDamTask::InitWeightLists()
{
	fElectronList = new PndMvdRadDamList("$VMCWORKDIR/pandaroot/mvd/MvdTools/MvdRadDamage/electronsWeight.root");
	fProtonList = new PndMvdRadDamList("$VMCWORKDIR/pandaroot/mvd/MvdTools/MvdRadDamage/protonsWeight.root");
	fNeutronList = new PndMvdRadDamList("$VMCWORKDIR/pandaroot/mvd/MvdTools/MvdRadDamage/neutronsWeight.root");
	fPionList = new PndMvdRadDamList("$VMCWORKDIR/pandaroot/mvd/MvdTools/MvdRadDamage/pionsWeight.root");

	fWeightListsMap[11] = fElectronList;		//e-
	fWeightListsMap[-11] = fElectronList;		//e+
	fWeightListsMap[2212] = fProtonList;		//p+
	fWeightListsMap[-2212] = fProtonList;		//p-
	fWeightListsMap[2112] = fNeutronList;		//n
	fWeightListsMap[-2112] = fNeutronList;		//n_bar
	fWeightListsMap[111] = fPionList;			//pi0
	fWeightListsMap[211] = fPionList;			//pi+
	fWeightListsMap[-211] = fPionList;			//pi-
	fWeightListsMap[1000010030] = fProtonList;	//Tritium
	fWeightListsMap[1000010020] = fProtonList;	//Deuterium


}

void PndMvdRadDamTask::Exec(Option_t* opt)
{
  // Reset output array
  if ( ! fRadDamHits )
    Fatal("Exec", "No RadDamArray");
  fRadDamHits->Delete();

  PndMvdMCPoint* mcPoint;
  PndMCTrack* mcTrack;

  for (int i = 0; i < fMCHits->GetEntriesFast(); i++){
	  mcPoint = (PndMvdMCPoint*)fMCHits->At(i);
	  mcTrack = (PndMCTrack*)(fMCTracks->At(mcPoint->GetTrackID()));
	  TVector3 mom;
	  mcPoint->Momentum(mom);
	  Double_t Ekin = mom.Mag();
	  Int_t pid = mcTrack->GetPdgCode();
	  Double_t weight = 0;
	  if (fWeightListsMap[pid] > 0){
		  weight = fWeightListsMap[pid]->GetWeight(Ekin);
	  }
	  //std::cout << "WeightCalc: pid: " << pid << " Energy: " << Ekin << " Weight: " << weight << std::endl;
	  new ((*fRadDamHits)[i]) PndMvdRadDamHit(mcPoint->GetTrackID(), i, mcPoint->GetDetName(), pid, Ekin,
			  mcPoint->GetPosition(), mom, weight);
	  if (fMapDetHistos[mcPoint->GetDetName().Data()] == 0){
		  PndStringVector svec(mcPoint->GetDetName().Data(),"/");
		  TVector3 sensDim = fGeoH->GetSensorDimensionsId(mcPoint->GetDetName().Data());
		  fMapDetHistos[mcPoint->GetDetName().Data()] = new TH2D(svec.Replace("/","o").c_str(),
																 fGeoH->GetPath(mcPoint->GetDetName()),
																 (Int_t)(2*sensDim.X()*10),-sensDim.X(),sensDim.X(),	// point resolution mm^2
																 (Int_t)(2*sensDim.Y()*10),-sensDim.Y(),sensDim.Y());
	  }
	  TVector3 localHit = fGeoH->MasterToLocalId(mcPoint->GetPosition(), mcPoint->GetDetName());
	  (TH2D*)(fMapDetHistos[mcPoint->GetDetName().Data()])->Fill(localHit.X(), localHit.Y(), weight);
	  fRadDamHisto->Fill(weight);
  }
}

void PndMvdRadDamTask::Finish()
{
	for (std::map<std::string, TH2*>::iterator it = fMapDetHistos.begin(); it != fMapDetHistos.end(); it++)
		it->second->Write();
	fRadDamHisto->Write();
}


ClassImp(PndMvdRadDamTask);
