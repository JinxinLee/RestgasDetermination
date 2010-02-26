//----------------------------------------------------------------------
// File and Version Information:
//      $Id: //
// Description:
//      Class PndEmcHitsToWaveform. Module to take the hit list for the 
//      calorimeter and make ADC waveforms from them.
// 
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//
// Author List:
//      Phil Strother                  Original author
// 		 Dima Melnichuk - adaption for PANDA
// Copyright Information:
//      Copyright (C) 1996             Imperial College
//
//----------------------------------------------------------------------

#include "PndEmcHitsToWaveform.h"

#include "PndEmcHit.h"
#include "PndEmcWaveform.h"
#include "PndEmcMapper.h"
#include "PndEmcStructure.h"
#include "PndEmcDigiPar.h"
#include "PndEmcGeoPar.h"		
#include "PndEmcDataTypes.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TStopwatch.h"
#include "TROOT.h"
#include "TClonesArray.h"

#include <iostream>
//#include <map>
//#include <string>

using std::cout;
using std::endl;
using std::fstream;

PndEmcHitsToWaveform::PndEmcHitsToWaveform(Int_t verbose, Bool_t storewaves)
{
	fVerbose = verbose;
	fStoreWaves = storewaves;
}

//--------------
// Destructor --
//--------------

PndEmcHitsToWaveform::~PndEmcHitsToWaveform()
{
}


InitStatus PndEmcHitsToWaveform::Init()
{
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman )
	{
		cout << "-E- PndEmcHitsToWaveform::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}
	
	// Get input array
	fHitArray = (TClonesArray*) ioman->GetObject("EmcHit");
	if ( ! fHitArray ) {
		cout << "-W- PndEmcHitsToWaveform::Init: "
		<< "No EmcHit array!" << endl;
		return kERROR;
	}
	
	// Create and register output array
	fWaveformArray = new TClonesArray("PndEmcWaveform");
	
	ioman->Register("EmcWaveform","Emc",fWaveformArray,fStoreWaves);
	
	cout << "-I- PndEmcHitsToWaveform: Intialization successfull" << endl;

	fNBits=fDigiPar->GetNBits();
	fDetectedPhotonsPerMeV=fDigiPar->GetDetectedPhotonsPerMeV();
	fEnergyRange=fDigiPar->GetEnergyRange(); //GeV
	fEnergyRangeBW=fDigiPar->GetEnergyRangeBW(); //GeV	
	fExcessNoiseFactor=fDigiPar->GetExcessNoiseFactor();
	fFirstSamplePhase=fDigiPar->GetFirstSamplePhase();
	fNumber_of_samples_in_waveform=fDigiPar->GetNumber_of_samples_in_waveform();
	fShaping_diff_time=fDigiPar->GetShaping_diff_time();     //s
	fShaping_int_time=fDigiPar->GetShaping_int_time();      //s
	fCrystal_time_constant=fDigiPar->GetCrystal_time_constant();  //s
	fIncoherent_elec_noise_width_GeV=fDigiPar->GetIncoherent_elec_noise_width_GeV(); //GeV
	fSampleRate=fDigiPar->GetSampleRate();
	fUse_shaped_noise=fDigiPar->GetUse_shaped_noise();
	fUse_photon_statistic=fDigiPar->GetUse_photon_statistic();
	fNoiseAllChannels=fDigiPar->GetNoiseAllChannels();
	
	// Test how parameters were read from DB.
	cout<<"EMC digitisation parameters "<<endl;
	cout<<"  nBits "<<fNBits<<endl;
	cout<<"  detectedPhotonsPerMeV "<<fDetectedPhotonsPerMeV<<endl;
	cout<<"  energyRange "<<fEnergyRange<<endl;
	cout<<"  energyRangeBW "<<fEnergyRangeBW<<endl;	
	cout<<"  excessNoiseFactor "<<fExcessNoiseFactor<<endl;
	cout<<"  firstSamplePhase "<<fFirstSamplePhase<<endl;
	cout<<"  number_of_samples_in_waveform "<<fNumber_of_samples_in_waveform<<endl;
	cout<<"  Shaping_diff_time "<<fShaping_diff_time<<endl;
	cout<<"  Shaping_int_time "<<fShaping_int_time<<endl;
	cout<<"  crystal_time_constant "<<fCrystal_time_constant<<endl;
	cout<<"  incoherent_elec_noise_width_GeV "<<fIncoherent_elec_noise_width_GeV<<endl;
	cout<<"  sampleRate "<<fSampleRate<<endl;
	cout<<"  use_shaped_noise "<<fUse_shaped_noise<<endl;
	cout<<"  use_photon_statistic "<<fUse_photon_statistic<<endl;
	cout<<"  EMC mapper "<<fGeoPar->GetMapperVersion()<<endl;

	fGeoPar->InitEmcMapper();
	PndEmcStructure::Instance();

	// Calculate 1 bit resolution (in units of FADC amplitude)
	PndEmcWaveform *tmpwaveform;
	tmpwaveform=new PndEmcWaveform(0,101010001, fShaping_diff_time, fShaping_int_time, fSampleRate, 0.0,0.0,fCrystal_time_constant,fNumber_of_samples_in_waveform,fExcessNoiseFactor,false);

	fGevPeakAnalogue = tmpwaveform->get_scale();
	
	fOneBitResolution=fEnergyRange/((double) (1<<fNBits))*fGevPeakAnalogue;
	fOneBitResolutionBW=fEnergyRangeBW/((double) (1<<fNBits))*fGevPeakAnalogue;
	
	fFirstADCBinTime=fFirstSamplePhase/fSampleRate;
	
	delete tmpwaveform;
	
	return kSUCCESS;
}

void PndEmcHitsToWaveform::Exec(Option_t* opt)
{
	TStopwatch timer;
	if (fVerbose>0){
		timer.Start();
	}
	// Reset output array
	if ( ! fWaveformArray ) Fatal("Exec", "No Waveform Array");
	fWaveformArray->Delete();
		
	// Variable declaration
	PndEmcHit* theHit = NULL;
	PndEmcWaveform* theWaveform = NULL;
	std::set<Int_t> waveformInd;
	
	// Loop over PndEmcHits to add them to correspondent waveforms
	// <set> fWaveformInd contains indexes of detectors for which Waveforms are created
	Int_t nHits = fHitArray->GetEntriesFast();
	cout<<"Hit array contains "<<nHits<< " hits"<<endl;
	for (Int_t iHit=0; iHit<nHits; iHit++) {
		theHit = (PndEmcHit*) fHitArray->At(iHit);
		Int_t detId=theHit->GetDetectorID();
		waveformInd.insert(detId);
		theWaveform = AddWaveform(detId,iHit);
		theWaveform->update_waveform(theHit);
	}
	
	// Produce waveforms in all the crystals, not only where hits took place 
	// Since it is time consuming, by default it is off
	if (fNoiseAllChannels)
	{
		Int_t detId_tmp;
		std::map<Int_t,PndEmcTwoCoordIndex*>  intTwoCoordMap =  PndEmcMapper::Instance()->GetTciMap();
		for(std::map<Int_t,PndEmcTwoCoordIndex* >::iterator iter = intTwoCoordMap.begin();
		iter != intTwoCoordMap.end(); ++iter){
			detId_tmp=(*iter).first;
			if (waveformInd.insert(detId_tmp).second){
				AddWaveform(detId_tmp,-1); // -1 correponds to Waveform produced not from EmcHit but from Noise
			}
		}
	}

	// Add electronic noise
	// There are two options how to add noise (before and after shaping) 

	Int_t nWf = fWaveformArray->GetEntriesFast();
	if (fVerbose>0){
		cout << "Number of waveforms processed= "<<nWf<<endl;
	}
	
	for (Int_t iWf=0; iWf<nWf; iWf++) {
		theWaveform = (PndEmcWaveform*) fWaveformArray->At(iWf);
		Int_t detId = theWaveform->GetDetectorId();
		Int_t module = detId / 100000000;
		if (fUse_shaped_noise==0)
		{
			if(module == 4){
				theWaveform->add_elec_noise_and_digitise(fIncoherent_elec_noise_width_GeV*fGevPeakAnalogue,fOneBitResolutionBW);
			}
			else{
				theWaveform->add_elec_noise_and_digitise(fIncoherent_elec_noise_width_GeV*fGevPeakAnalogue,fOneBitResolution);
			}
		} else {
			if(module == 4){
				theWaveform->add_shaped_elec_noise_and_digitise(fIncoherent_elec_noise_width_GeV*fGevPeakAnalogue,fOneBitResolutionBW);
			}
			else{
				theWaveform->add_shaped_elec_noise_and_digitise(fIncoherent_elec_noise_width_GeV*fGevPeakAnalogue,fOneBitResolution);
			}
		}
	}

	if (fVerbose>0){
		timer.Stop();
		Double_t rtime = timer.RealTime();
		Double_t ctime = timer.CpuTime();
		cout << "PndEmcHitsToWaveform, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	}

}

void PndEmcHitsToWaveform::SetParContainers() {

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get Emc geometry parameter container
  fGeoPar = (PndEmcGeoPar*) db->getContainer("PndEmcGeoPar");
  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar*) db->getContainer("PndEmcDigiPar");
 
}

// -----   Private method AddWaveform   --------------------------------------------
PndEmcWaveform* PndEmcHitsToWaveform::AddWaveform(Int_t detID, Int_t iHit){
	TClonesArray& clref = *fWaveformArray;
	Int_t size = clref.GetEntriesFast();
	return new(clref[size]) PndEmcWaveform(0,detID,
				       fShaping_diff_time,fShaping_int_time,
				       fSampleRate,
				       fFirstADCBinTime,
				       fDetectedPhotonsPerMeV,
				       fCrystal_time_constant,
				       fNumber_of_samples_in_waveform,
						 fExcessNoiseFactor,
						 fUse_photon_statistic,
						 iHit);
}

void PndEmcHitsToWaveform::SetStorageOfData(Bool_t val)
{
  fStoreWaves = val;
  return;
}

ClassImp(PndEmcHitsToWaveform)
