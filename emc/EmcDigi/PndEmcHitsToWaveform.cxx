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
#include <cassert>
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
	fSensitiveAreaAPD=fDigiPar->GetSensitiveAreaAPD();
	fSensitiveAreaVPT=fDigiPar->GetSensitiveAreaVPT();
	fQuantumEfficiencyAPD=fDigiPar->GetQuantumEfficiencyAPD();
	fQuantumEfficiencyVPT=fDigiPar->GetQuantumEfficiencyVPT();
	fExcessNoiseFactorAPD=fDigiPar->GetExcessNoiseFactorAPD();
	fExcessNoiseFactorVPT=fDigiPar->GetExcessNoiseFactorVPT();
	fIncoherent_elec_noise_width_GeV_APD=fDigiPar->GetIncoherent_elec_noise_width_GeV_APD(); //GeV
	fIncoherent_elec_noise_width_GeV_VPT=fDigiPar->GetIncoherent_elec_noise_width_GeV_VPT(); //GeV
	fEnergyRange=fDigiPar->GetEnergyRange(); //GeV
	fEnergyRangeBW=fDigiPar->GetEnergyRangeBW(); //GeV	
	fFirstSamplePhase=fDigiPar->GetFirstSamplePhase();
	fNumber_of_samples_in_waveform=fDigiPar->GetNumber_of_samples_in_waveform();
	fShaping_diff_time=fDigiPar->GetShaping_diff_time();     //s
	fShaping_int_time=fDigiPar->GetShaping_int_time();      //s
	fCrystal_time_constant=fDigiPar->GetCrystal_time_constant();  //s
	fSampleRate=fDigiPar->GetSampleRate();
	fUse_shaped_noise=fDigiPar->GetUse_shaped_noise();
	fUse_photon_statistic=fDigiPar->GetUse_photon_statistic();
	fNoiseAllChannels=fDigiPar->GetNoiseAllChannels();
	
	// Test how parameters were read from DB.
	cout<<"EMC digitisation parameters "<<endl;
	cout<<"  nBits "<<fNBits<<endl;
	cout<<"  detectedPhotonsPerMeV "<<fDetectedPhotonsPerMeV<<endl;
	cout<<"  excessNoiseFactor APD"<<fExcessNoiseFactorAPD<<endl;
	cout<<"  excessNoiseFactor VPT"<<fExcessNoiseFactorVPT<<endl;
	cout<<"  incoherent_elec_noise_width_GeV_APD "<<fIncoherent_elec_noise_width_GeV_APD<<endl;
	cout<<"  incoherent_elec_noise_width_GeV_VPT "<<fIncoherent_elec_noise_width_GeV_VPT<<endl;
	cout<<"  energyRange "<<fEnergyRange<<endl;
	cout<<"  energyRangeBW "<<fEnergyRangeBW<<endl;	
	cout<<"  firstSamplePhase "<<fFirstSamplePhase<<endl;
	cout<<"  number_of_samples_in_waveform "<<fNumber_of_samples_in_waveform<<endl;
	cout<<"  Shaping_diff_time "<<fShaping_diff_time<<endl;
	cout<<"  Shaping_int_time "<<fShaping_int_time<<endl;
	cout<<"  crystal_time_constant "<<fCrystal_time_constant<<endl;
	cout<<"  sampleRate "<<fSampleRate<<endl;
	cout<<"  use_shaped_noise "<<fUse_shaped_noise<<endl;
	cout<<"  use_photon_statistic "<<fUse_photon_statistic<<endl;
	cout<<"  EMC mapper "<<fGeoPar->GetMapperVersion()<<endl;

	fGeoPar->InitEmcMapper();
	PndEmcStructure::Instance();

	// Calculate 1 bit resolution (in units of FADC amplitude)
	PndEmcWaveform *tmpwaveform=new PndEmcWaveform(0,101010001, fNumber_of_samples_in_waveform);

	PndEmcAbsPulseshape *pulseshape=new PndEmcCRRCPulseshape(fShaping_diff_time,fShaping_int_time,fCrystal_time_constant);
	
	fGevPeakAnalogue = tmpwaveform->GetScale(fSampleRate, pulseshape);
	
	fOneBitResolution=fEnergyRange/((double) (1<<fNBits))*fGevPeakAnalogue;
	fOneBitResolutionBW=fEnergyRangeBW/((double) (1<<fNBits))*fGevPeakAnalogue;
	
	fFirstADCBinTime=fFirstSamplePhase/fSampleRate;
	
	// Calculate number of photoelectrons for APD and VPT
	// The number fDetectedPhotonsPerMeV is the measured number of photoelectrons with PM covering the whole rear surface divided by quantum efficiency of PM (18%)
	// To estimate Number of photoelectrons in barrel the rare surface is taken equal for all the crystals 745 mm^2, which is average surface, hovewer it varies depending on the type of the crystal
	// For forward and backward endcap rear surface is equal 26x26=676 mm^2
	// Therefore the different number of photoelectrons are used with APD for barrel and backward endcap
	fNPhotoElectronsPerMeVAPDBarrel=fDetectedPhotonsPerMeV*fSensitiveAreaAPD/745.*fQuantumEfficiencyAPD;
	fNPhotoElectronsPerMeVAPDBWD=fDetectedPhotonsPerMeV*fSensitiveAreaAPD/676.*fQuantumEfficiencyAPD;
	fNPhotoElectronsPerMeVVPT=fDetectedPhotonsPerMeV*fSensitiveAreaVPT/676.*fQuantumEfficiencyVPT;
	delete pulseshape;
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
	
	PndEmcCRRCPulseshape *pulseshape= new PndEmcCRRCPulseshape(fShaping_diff_time,fShaping_int_time,fCrystal_time_constant);
	
	for (Int_t iHit=0; iHit<nHits; iHit++) {
		theHit = (PndEmcHit*) fHitArray->At(iHit);
		Int_t detId=theHit->GetDetectorID();
		waveformInd.insert(detId);
		theWaveform = AddWaveform(detId,iHit);
		Int_t module = theWaveform->GetModule();
		switch (module){
			case 1: // Barrel 
					theWaveform->UpdateWaveform(theHit, fNPhotoElectronsPerMeVAPDBarrel, fUse_photon_statistic, fExcessNoiseFactorAPD, fFirstSamplePhase, fSampleRate, pulseshape);
					break;
			case 2: // Barrel
					theWaveform->UpdateWaveform(theHit, fNPhotoElectronsPerMeVAPDBarrel, fUse_photon_statistic, fExcessNoiseFactorAPD, fFirstSamplePhase, fSampleRate, pulseshape);
					break;
			case 3: // Fwd endcap
					theWaveform->UpdateWaveform(theHit, fNPhotoElectronsPerMeVVPT, fUse_photon_statistic, fExcessNoiseFactorVPT, fFirstSamplePhase, fSampleRate, pulseshape);
					break;
			case 4: // Bwd endcap
					theWaveform->UpdateWaveform(theHit, fNPhotoElectronsPerMeVVPT, fUse_photon_statistic, fExcessNoiseFactorVPT, fFirstSamplePhase, fSampleRate, pulseshape);
					break;
			case 5: // Shashlyk calorimetr (At the moment parameters from barrel are used)
					theWaveform->UpdateWaveform(theHit, fNPhotoElectronsPerMeVAPDBarrel, fUse_photon_statistic, fExcessNoiseFactorAPD, fFirstSamplePhase, fSampleRate, pulseshape);
					break;
			default:
				std::cout<<"Unknown module number in EMC digitization"<<std::endl;
				abort();
		}
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
	
		// "0" corresponds to timing constant of signal
		// for noise can be assumed 0 (it is taken tauInt*1e-5 to avoid nan)
	PndEmcAbsPulseshape *pulseshape2 = new PndEmcCRRCPulseshape(fShaping_diff_time,fShaping_int_time,fShaping_diff_time*1e-5);
	
	for (Int_t iWf=0; iWf<nWf; iWf++) {
		theWaveform = (PndEmcWaveform*) fWaveformArray->At(iWf);
		Int_t module = theWaveform->GetModule();
		switch (module){
			case 1: // Barrel 
				if (fUse_shaped_noise==0)
				{
					theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolution);
				}
				else {
					theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolution, pulseshape2, fFirstSamplePhase, fSampleRate);
				}
				break;
			case 2: // Barrel 
				if (fUse_shaped_noise==0)
				{
					theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolution);
				}
				else {
					theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolution, pulseshape2, fFirstSamplePhase, fSampleRate);
				}
				break;
			case 3: // FWD Endcap 
				if (fUse_shaped_noise==0)
				{
					theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_VPT*fGevPeakAnalogue,fOneBitResolution);
				}
				else {
					theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_VPT*fGevPeakAnalogue,fOneBitResolution, pulseshape2, fFirstSamplePhase, fSampleRate);
				}
				break;
			case 4: // BWD Endcap 
				if (fUse_shaped_noise==0)
				{
					theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolutionBW);
				}
				else {
					theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolutionBW, pulseshape2, fFirstSamplePhase, fSampleRate);
				}
				break;
			case 5: // shashlyk calorimetr 
				if (fUse_shaped_noise==0)
				{
					theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolution);
				}
				else {
					theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolution, pulseshape2, fFirstSamplePhase, fSampleRate);
				}
				break;
			default:
				std::cout<<"Unknown module number in EMC digitization"<<std::endl;
				abort();
		}
	}

	delete pulseshape;
	
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
				       fNumber_of_samples_in_waveform,
						 iHit);
}

void PndEmcHitsToWaveform::SetStorageOfData(Bool_t val)
{
  fStoreWaves = val;
  return;
}

ClassImp(PndEmcHitsToWaveform)
