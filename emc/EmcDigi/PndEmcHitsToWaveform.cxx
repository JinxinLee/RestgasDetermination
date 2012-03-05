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
#include "PndEmcAsicPulseshape.h"
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

PndEmcHitsToWaveform::PndEmcHitsToWaveform(Int_t verbose, Bool_t storewaves):
  fHitArray(new TClonesArray), fWaveformArray(new TClonesArray), fOneBitResolution(0), fOneBitResolutionBW(0), fOneBitResolutionPMT(0), fNBits(0), fDetectedPhotonsPerMeV(0), fDetectedPhotonsPerMeV_PMT(0), fNPhotoElectronsPerMeVAPDBarrel(0), fNPhotoElectronsPerMeVAPDBWD(0), fNPhotoElectronsPerMeVVPT(0), fNPhotoElectronsPerMeVPMT(0), fSensitiveAreaAPD(0), fSensitiveAreaVPT(0), fQuantumEfficiencyAPD(0), fQuantumEfficiencyVPT(0), fQuantumEfficiencyPMT(0), fExcessNoiseFactorAPD(0), fExcessNoiseFactorVPT(0), fExcessNoiseFactorPMT(0), fIncoherent_elec_noise_width_GeV_APD(0), fIncoherent_elec_noise_width_GeV_VPT(0), fEnergyRange(0), fEnergyRangeBW(0), fFirstSamplePhase(0), fNumber_of_samples_in_waveform(0), fNumber_of_samples_in_waveform_pmt(0), fASIC_Shaping_int_time(0), fPMT_Shaping_int_time(0), fPMT_Shaping_diff_time(0), fCrystal_time_constant(0), fShashlyk_time_constant(0), fShashlykSamplingFactor(0), fSampleRate(0), fSampleRate_PMT(0), fUse_shaped_noise(0), fUse_photon_statistic(0), fNoiseAllChannels(0), fMapVersion(0), fFirstADCBinTime(0), fGevPeakAnalogue(0), fGevPeakAnalogue_PMT(0), fDigiPar(new PndEmcDigiPar()), fGeoPar(new PndEmcGeoPar()), fVerbose(verbose), fStoreWaves(storewaves)
{
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
	fDetectedPhotonsPerMeV_PMT=fDigiPar->GetDetectedPhotonsPerMeV_PMT();
	fSensitiveAreaAPD=fDigiPar->GetSensitiveAreaAPD();
	fSensitiveAreaVPT=fDigiPar->GetSensitiveAreaVPT();
	fQuantumEfficiencyAPD=fDigiPar->GetQuantumEfficiencyAPD();
	fQuantumEfficiencyVPT=fDigiPar->GetQuantumEfficiencyVPT();
	fQuantumEfficiencyPMT=fDigiPar->GetQuantumEfficiencyPMT();
	fExcessNoiseFactorAPD=fDigiPar->GetExcessNoiseFactorAPD();
	fExcessNoiseFactorVPT=fDigiPar->GetExcessNoiseFactorVPT();
	fExcessNoiseFactorPMT = fDigiPar->GetExcessNoiseFactorPMT();
	fIncoherent_elec_noise_width_GeV_APD=fDigiPar->GetIncoherent_elec_noise_width_GeV_APD(); //GeV
	fIncoherent_elec_noise_width_GeV_VPT=fDigiPar->GetIncoherent_elec_noise_width_GeV_VPT(); //GeV
	fEnergyRange=fDigiPar->GetEnergyRange(); //GeV
	fEnergyRangeBW=fDigiPar->GetEnergyRangeBW(); //GeV	
	fFirstSamplePhase=fDigiPar->GetFirstSamplePhase();
	fNumber_of_samples_in_waveform=fDigiPar->GetNumber_of_samples_in_waveform();
	fNumber_of_samples_in_waveform_pmt=fDigiPar->GetNumber_of_samples_in_waveform_pmt();
	fASIC_Shaping_int_time=fDigiPar->GetASIC_Shaping_int_time();      //s
	fPMT_Shaping_int_time=fDigiPar->GetPMT_Shaping_int_time();      //s
	fPMT_Shaping_diff_time=fDigiPar->GetPMT_Shaping_diff_time();      //s
	fCrystal_time_constant=fDigiPar->GetCrystal_time_constant();  //s
	fShashlyk_time_constant=fDigiPar->GetShashlyk_time_constant();  //s
	fShashlykSamplingFactor=fDigiPar->GetShashlykSamplingFactor();
	fSampleRate=fDigiPar->GetSampleRate();
	fSampleRate_PMT=fDigiPar->GetSampleRate_PMT();
	fUse_shaped_noise=fDigiPar->GetUse_shaped_noise();
	fUse_photon_statistic=fDigiPar->GetUse_photon_statistic();
	fNoiseAllChannels=fDigiPar->GetNoiseAllChannels();
	
	// Test how parameters were read from DB.
	cout<<"EMC digitisation parameters "<<endl;
	cout<<"  nBits "<<fNBits<<endl;
	cout<<"  detectedPhotonsPerMeV "<<fDetectedPhotonsPerMeV<<endl;
	cout<<"  detectedPhotonsPerMeV_PMT "<<fDetectedPhotonsPerMeV_PMT<<endl;
	cout<<"  excessNoiseFactor APD"<<fExcessNoiseFactorAPD<<endl;
	cout<<"  excessNoiseFactor VPT"<<fExcessNoiseFactorVPT<<endl;
	cout<<"  excessNoiseFactor PMT"<<fExcessNoiseFactorPMT<<endl;
	cout<<"  incoherent_elec_noise_width_GeV_APD "<<fIncoherent_elec_noise_width_GeV_APD<<endl;
	cout<<"  incoherent_elec_noise_width_GeV_VPT "<<fIncoherent_elec_noise_width_GeV_VPT<<endl;
	cout<<"  energyRange "<<fEnergyRange<<endl;
	cout<<"  energyRangeBW "<<fEnergyRangeBW<<endl;	
	cout<<"  firstSamplePhase "<<fFirstSamplePhase<<endl;
	cout<<"  number_of_samples_in_waveform "<<fNumber_of_samples_in_waveform<<endl;
	cout<<"  number_of_samples_in_waveform_pmt "<<fNumber_of_samples_in_waveform_pmt<<endl;
	cout<<"  ASIC_Shaping_int_time "<<fASIC_Shaping_int_time<<endl;
	cout<<"  PMT_Shaping_int_time "<<fPMT_Shaping_int_time<<endl;
	cout<<"  PMT_Shaping_diff_time "<<fPMT_Shaping_diff_time<<endl;
	cout<<"  crystal_time_constant "<<fCrystal_time_constant<<endl;
	cout<<"  shashlyk_time_constant "<<fShashlyk_time_constant<<endl;
	cout<<"  ShashlykSamplingFactor "<<fShashlykSamplingFactor<<endl;
	cout<<"  sampleRate "<<fSampleRate<<endl;
	cout<<"  sampleRate_PMT "<<fSampleRate_PMT<<endl;
	cout<<"  use_shaped_noise "<<fUse_shaped_noise<<endl;
	cout<<"  use_photon_statistic "<<fUse_photon_statistic<<endl;
	cout<<"  EMC mapper "<<fGeoPar->GetMapperVersion()<<endl;

	fGeoPar->InitEmcMapper();
	PndEmcStructure::Instance();

	// Calculate 1 bit resolution (in units of FADC amplitude)
	PndEmcWaveform *tmpwaveform=new PndEmcWaveform(0,101010001, fNumber_of_samples_in_waveform);

	PndEmcAbsPulseshape *pulseshape=new PndEmcAsicPulseshape(fASIC_Shaping_int_time,fCrystal_time_constant);

	PndEmcWaveform *tmpwaveform2=new PndEmcWaveform(0,101010001, fNumber_of_samples_in_waveform_pmt);

	PndEmcAbsPulseshape *pulseshape2=new PndEmcCRRCPulseshape(fPMT_Shaping_int_time,fPMT_Shaping_diff_time,fShashlyk_time_constant);
	
	fGevPeakAnalogue = tmpwaveform->GetScale(fSampleRate, pulseshape);

	fGevPeakAnalogue_PMT = tmpwaveform2->GetScale(fSampleRate_PMT, pulseshape2);
	
//	cout<<" -I- PndEmcHitsToWaveform::Init:=========== "<<endl;
//	cout<<"  fGevPeakAnalogue= "<<fGevPeakAnalogue<<endl;
//	cout<<"  fGevPeakAnalogue_PMT= "<<fGevPeakAnalogue_PMT<<endl;
	fOneBitResolution=fEnergyRange/((double) (1<<fNBits))*fGevPeakAnalogue;
	fOneBitResolutionBW=fEnergyRangeBW/((double) (1<<fNBits))*fGevPeakAnalogue;
	fOneBitResolutionPMT=fEnergyRange/((double) (1<<fNBits))*fGevPeakAnalogue_PMT;
//	cout<<"  fOneBitResolution= "<<fOneBitResolution<<endl;
//	cout<<"  fOneBitResolutionBW= "<<fOneBitResolutionBW<<endl;
//	cout<<"  fOneBitResolutionPMT= "<<fOneBitResolutionPMT<<endl;
	
	fFirstADCBinTime=fFirstSamplePhase/fSampleRate;
//	cout<<"  fFirstADCBinTime= "<<fFirstADCBinTime<<endl;
	
	// Calculate number of photoelectrons for APD and VPT
	// The number fDetectedPhotonsPerMeV is the measured number of photoelectrons with PM covering the whole rear surface divided by quantum efficiency of PM (18%)
	// To estimate Number of photoelectrons in barrel the rare surface is taken equal for all the crystals 745 mm^2, which is average surface, hovewer it varies depending on the type of the crystal
	// For forward and backward endcap rear surface is equal 26x26=676 mm^2
	// Therefore the different number of photoelectrons are used with APD for barrel and backward endcap
	fNPhotoElectronsPerMeVAPDBarrel=fDetectedPhotonsPerMeV*fSensitiveAreaAPD/745.*fQuantumEfficiencyAPD;
	fNPhotoElectronsPerMeVAPDBWD=fDetectedPhotonsPerMeV*fSensitiveAreaAPD/676.*fQuantumEfficiencyAPD;
	fNPhotoElectronsPerMeVVPT=fDetectedPhotonsPerMeV*fSensitiveAreaVPT/676.*fQuantumEfficiencyVPT;
	fNPhotoElectronsPerMeVPMT=fDetectedPhotonsPerMeV_PMT*fQuantumEfficiencyPMT;
//	cout<<"  fNPhotoElectronsPerMeVAPDBarrel= "<<fNPhotoElectronsPerMeVAPDBarrel<<endl;
//	cout<<"  fNPhotoElectronsPerMeVAPDBWD= "<<fNPhotoElectronsPerMeVAPDBWD<<endl;
//	cout<<"  fNPhotoElectronsPerMeVVPT= "<<fNPhotoElectronsPerMeVVPT<<endl;
//	cout<<"  fNPhotoElectronsPerMeVPMT= "<<fNPhotoElectronsPerMeVPMT<<endl;
	delete pulseshape;
	delete tmpwaveform;
	delete pulseshape2;
	delete tmpwaveform2;
	
	return kSUCCESS;
}

void PndEmcHitsToWaveform::Exec(Option_t* opt)
{
	TStopwatch timer;
	if (fVerbose>2){
		timer.Start();
	}
	// Reset output array
	if ( ! fWaveformArray ) Fatal("Exec", "No Waveform Array");
	fWaveformArray->Delete();
		
	// Variable declaration
	PndEmcHit* theHit = NULL;
	PndEmcWaveform* theWaveform = NULL;
	PndEmcHit* tmpHit = NULL;
	std::set<Int_t> waveformInd;
	Int_t NumOfSamples;
	
	// Loop over PndEmcHits to add them to correspondent waveforms
	// <set> fWaveformInd contains indexes of detectors for which Waveforms are created
	Int_t nHits = fHitArray->GetEntriesFast();
	if (fVerbose>0){
		cout<<"PndEmcHitsToWaveform:: Hit array contains "<<nHits<< " hits"<<endl;
	}
	PndEmcAsicPulseshape *pulseshape= new PndEmcAsicPulseshape(fASIC_Shaping_int_time,fCrystal_time_constant);
	PndEmcAbsPulseshape *pulseshape2=new PndEmcCRRCPulseshape(fPMT_Shaping_int_time,fPMT_Shaping_diff_time,fShashlyk_time_constant);

	for (Int_t iHit=0; iHit<nHits; iHit++) {
		theHit = (PndEmcHit*) fHitArray->At(iHit);
		Int_t detId=theHit->GetDetectorID();
		Int_t module = theHit->GetModule();
		waveformInd.insert(detId);
		if(module == 5)
			NumOfSamples = fNumber_of_samples_in_waveform_pmt;
		else
			NumOfSamples = fNumber_of_samples_in_waveform;

		theWaveform = AddWaveform(detId,iHit,NumOfSamples);
//		Int_t module_wf = theWaveform->GetModule();

		switch (module){
			case 1: // Barrel
				theWaveform->UpdateWaveform(theHit, fNPhotoElectronsPerMeVAPDBarrel, fUse_photon_statistic, fExcessNoiseFactorAPD, fFirstADCBinTime, fSampleRate, pulseshape);
					break;
			case 2: // Barrel
				theWaveform->UpdateWaveform(theHit, fNPhotoElectronsPerMeVAPDBarrel, fUse_photon_statistic, fExcessNoiseFactorAPD, fFirstADCBinTime, fSampleRate, pulseshape);
					break;
			case 3: // Fwd endcap
				theWaveform->UpdateWaveform(theHit, fNPhotoElectronsPerMeVVPT, fUse_photon_statistic, fExcessNoiseFactorVPT, fFirstADCBinTime, fSampleRate, pulseshape);
					break;
			case 4: // Bwd endcap
				theWaveform->UpdateWaveform(theHit, fNPhotoElectronsPerMeVVPT, fUse_photon_statistic, fExcessNoiseFactorVPT, fFirstADCBinTime, fSampleRate, pulseshape);
					break;
			case 5: // Shashlyk calorimetr
				tmpHit = theHit;
				tmpHit->SetEnergy(tmpHit->GetEnergy()*fShashlykSamplingFactor);
				theWaveform->UpdateWaveform(theHit, fNPhotoElectronsPerMeVPMT, fUse_photon_statistic,fExcessNoiseFactorPMT, fFirstADCBinTime, fSampleRate_PMT, pulseshape2);
					break;
			default:
				std::cout<<" UpdateWaveform: Unknown module number "<<module<<" in EMC digitization. Detector ID = "<<detId<<std::endl;
				//abort();
		}
	}
	
	// Produce waveforms in all the crystals, not only where hits took place 
	// Since it is time consuming, by default it is off
	if (fNoiseAllChannels)
	{
		Int_t detId_tmp, modId_tmp;
		std::map<Int_t,PndEmcTwoCoordIndex*>  intTwoCoordMap =  PndEmcMapper::Instance()->GetTciMap();
		for(std::map<Int_t,PndEmcTwoCoordIndex* >::iterator iter = intTwoCoordMap.begin();
		iter != intTwoCoordMap.end(); ++iter){
			detId_tmp=(*iter).first;
			modId_tmp = detId_tmp/100000000;
			if(modId_tmp == 5)
				NumOfSamples = fNumber_of_samples_in_waveform_pmt;
			else
				NumOfSamples = fNumber_of_samples_in_waveform;

			if (waveformInd.insert(detId_tmp).second){
				AddWaveform(detId_tmp,-1,NumOfSamples); // -1 correponds to Waveform produced not from EmcHit but from Noise
			}
		}
	}

	// Add electronic noise
	// There are two options how to add noise (before and after shaping) 

	Int_t nWf = fWaveformArray->GetEntriesFast();
	if (fVerbose>2){
		cout << "Number of waveforms processed= "<<nWf<<endl;
	}
	
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
					theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolution, pulseshape, fFirstSamplePhase, fSampleRate);
				}
				break;
			case 2: // Barrel 
				if (fUse_shaped_noise==0)
				{
					theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolution);
				}
				else {
					theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolution, pulseshape, fFirstSamplePhase, fSampleRate);
				}
				break;
			case 3: // FWD Endcap 
				if (fUse_shaped_noise==0)
				{
					theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_VPT*fGevPeakAnalogue,fOneBitResolution);
				}
				else {
					theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_VPT*fGevPeakAnalogue,fOneBitResolution, pulseshape, fFirstSamplePhase, fSampleRate);
				}
				break;
			case 4: // BWD Endcap 
				if (fUse_shaped_noise==0)
				{
					theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolutionBW);
				}
				else {
					theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolutionBW, pulseshape, fFirstSamplePhase, fSampleRate);
				}
				break;
			case 5: // shashlyk calorimetr 
				if (fUse_shaped_noise==0)
				{
					theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue_PMT,fOneBitResolutionPMT);
				}
				else {
					theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue_PMT,fOneBitResolutionPMT, pulseshape2, fFirstSamplePhase, fSampleRate_PMT);
				}
				break;
			default:
				std::cout<<"Add Noise: Unknown module number in EMC digitization"<<std::endl;
				//abort();
		}
	}

	delete pulseshape;
	delete pulseshape2;
	
	if (fVerbose>2){
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
PndEmcWaveform* PndEmcHitsToWaveform::AddWaveform(Int_t detID, Int_t iHit,Int_t numOfSamples){
	TClonesArray& clref = *fWaveformArray;
	Int_t size = clref.GetEntriesFast();
	return new(clref[size]) PndEmcWaveform(0,detID,numOfSamples,iHit);
}

void PndEmcHitsToWaveform::SetStorageOfData(Bool_t val)
{
  fStoreWaves = val;
  return;
}

ClassImp(PndEmcHitsToWaveform)
