//----------------------------------------------------------------------
// File and Version Information:
//      $Id: //
// Description:
//      Class PndEmcMultiWaveformToCalibratedDigi. Module to take the ADC waveforms and produces digi.
// 
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI		
//		
// Author List:
//      Phil Strother                  Original Author
// Dima Melnichuk - adaption for PANDA		
// Copyright Information:
//      Copyright (C) 1996             Imperial College
//
//----------------------------------------------------------------------

#include "PndEmcMultiWaveformToCalibratedDigi.h"

#include "PndEmcMultiWaveform.h"
#include "PndEmcWaveform.h"
#include "PndEmcDigi.h"
#include "PndEmcDigiPar.h"		
#include "PndEmcRecoPar.h"				
#include "PndEmcAsicPulseshape.h"
#include "PndEmcPSAParabolic.h"
#include "PndEmcPSAParabolicBaseline.h"
#include "PndEmcPSAMatchedDigiFilter.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TStopwatch.h"
		
#include <iostream>
#include <fstream>
//#include <map>

using std::cout;
using std::endl;
using std::fstream;

PndEmcMultiWaveformToCalibratedDigi::PndEmcMultiWaveformToCalibratedDigi(Int_t verbose, Bool_t storedigis)
{
	fVerbose=verbose;
	fDigiPosMethod="depth";// "surface" or "depth"
	fEmcDigiRescaleFactor=1.08;
	fStoreDigis=storedigis;
	fCalibrationFileName = "";
	//fPndEmcDigiPositionDepth=6.2;
}

//--------------
// Destructor --
//--------------

PndEmcMultiWaveformToCalibratedDigi::~PndEmcMultiWaveformToCalibratedDigi()
{
}


InitStatus PndEmcMultiWaveformToCalibratedDigi::Init()
{
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman )
	{
		cout << "-E- PndEmcMultiWaveformToCalibratedDigi::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}
	
	// Get input array
	fWaveformArray = (TClonesArray*) ioman->GetObject("EmcMultiWaveform");
	if ( ! fWaveformArray ) {
		cout << "-W- PndEmcMultiWaveformToCalibratedDigi::Init: "
		<< "No PndEmcMultiWaveform array!" << endl;
		return kERROR;
	}
	
	// Create and register output array
	fDigiArray = new TClonesArray("PndEmcDigi");

	ioman->Register("EmcDigi","Emc",fDigiArray,fStoreDigis);
	fSampleRate=fDigiPar->GetSampleRate();
	fSampleRate_PMT=fDigiPar->GetSampleRate_PMT();
	fASIC_Shaping_int_time=fDigiPar->GetASIC_Shaping_int_time();      //s
	fPMT_Shaping_int_time=fDigiPar->GetPMT_Shaping_int_time();      //s
	fPMT_Shaping_diff_time=fDigiPar->GetPMT_Shaping_diff_time();      //s
	fCrystal_time_constant=fDigiPar->GetCrystal_time_constant();  //s
	fShashlyk_time_constant=fDigiPar->GetShashlyk_time_constant();  //s
	fNumber_of_samples_in_waveform=fDigiPar->GetNumber_of_samples_in_waveform();
	fNumber_of_samples_in_waveform_pmt=fDigiPar->GetNumber_of_samples_in_waveform_pmt();
	fEnergyDigiThreshold=fDigiPar->GetEnergyDigiThreshold();
	fEmcDigiPositionDepthPWO=fRecoPar->GetEmcDigiPositionDepthPWO();
	fEmcDigiPositionDepthShashlyk=fRecoPar->GetEmcDigiPositionDepthShashlyk();
	
	cout<<"fEmcDigiPositionDepthPWO: "<<fEmcDigiPositionDepthPWO<<endl;
	cout<<"fEmcDigiPositionDepthShashlyk: "<<fEmcDigiPositionDepthShashlyk<<endl;
				
	if (!fDigiPosMethod.CompareTo("surface"))
	{
		PndEmcDigi::selectDigiPositionMethod( PndEmcDigi::surface, 0., 0., 1.0 );
	}
	else if (!fDigiPosMethod.CompareTo("depth"))
	{
	   PndEmcDigi::selectDigiPositionMethod( PndEmcDigi::depth, 
			 fEmcDigiPositionDepthPWO, fEmcDigiPositionDepthShashlyk, fEmcDigiRescaleFactor);
	}
	else 
	{
		cout << "-W- PndEmcMultiWaveformToCalibratedDigi::Init: "
		<< "Unknown digi position method!" << endl;
		return kERROR;
	}
	
	fPulseshape= new PndEmcAsicPulseshape(fASIC_Shaping_int_time,fCrystal_time_constant);
	fPulseshape_pmt= new PndEmcCRRCPulseshape(fPMT_Shaping_int_time,fPMT_Shaping_diff_time,fShashlyk_time_constant);


	// Pulse shape analysis algorithm.
	// Simple parabolic fit.
	//psaAlgorithm = new PndEmcPSAParabolic();

	// Matched digital filter
	// Parameters of the filter are hardcoded at the moment
	// For different pulseshape in barrel and endcaps different filters should be implemented
 	std::vector<Double_t> params;
 	params.push_back(30); // width
	params.push_back(fSampleRate); // Sample rate
	psaAlgorithm = new PndEmcPSAMatchedDigiFilter(params,fPulseshape);

//	std::vector<Double_t> params2;
// 	params2.push_back(30); // width
//	params2.push_back(fSampleRate_PMT); // Sample rate
	psaAlgorithm_pmt = new PndEmcPSAParabolic();
	
	// Determine normalisation constant for PndEmcMultiWaveform
	PndEmcWaveform *tmpwaveform=new PndEmcWaveform(0,101010001, fNumber_of_samples_in_waveform);
	PndEmcWaveform *tmpwaveform2=new PndEmcWaveform(0,101010001, fNumber_of_samples_in_waveform_pmt);
	
	PndEmcHit *gevHit=new PndEmcHit();
	gevHit->SetEnergy(1.0);
	gevHit->SetTime(0.);
	tmpwaveform->UpdateWaveform(gevHit, 0, false, 1., 0., fSampleRate, fPulseshape);
	tmpwaveform2->UpdateWaveform(gevHit, 0, false, 1., 0., fSampleRate_PMT, fPulseshape_pmt);
	Double_t tmpPeakPosition;
	Double_t tmpPeakPosition2;
	psaAlgorithm->Process(tmpwaveform,fWfNormalisation,tmpPeakPosition);
	psaAlgorithm_pmt->Process(tmpwaveform2,fWfNormalisation_pmt,tmpPeakPosition2);

	psaAlgorithm_proto192 = new PndEmcPSAParabolicBaseline(20);
	fWfNormalisation_proto192 = 1;

	if(fCalibrationFileName !="")
		ReadCalibrationFile();

	cout << "-I- PndEmcMultiWaveformToCalibratedDigi: Read "<< fCalibrationMap.size() << " Calibration Entries" << endl;

	cout << "-I- PndEmcMultiWaveformToCalibratedDigi: Intialization successfull" << endl;
	
	return kSUCCESS;
}

void PndEmcMultiWaveformToCalibratedDigi::Exec(Option_t* opt)
{
	TStopwatch timer;
	if (fVerbose>2){
		timer.Start();
	}
// Reset output array
	if ( ! fDigiArray ) Fatal("Exec", "No Digi Array");
  	fDigiArray->Delete();
	Double_t peakPosition;
	Double_t energy;
	Double_t digi_time;
	Int_t i_digi=0; //index of digi in TClonesArray
	Int_t hitIndex;
	Int_t detId;
	Int_t trackId;
	Int_t module;
	Int_t nWaveforms = fWaveformArray->GetEntriesFast();
	Int_t nSignal;
	Bool_t highgain;
	//cout<<"PndEmcMultiWaveformToCalibratedDigi: "<<nWaveforms<<" waveforms to convert"<<endl;
	for (Int_t iWaveform=0; iWaveform<nWaveforms; iWaveform++) {
		PndEmcMultiWaveform* theWaveform = (PndEmcMultiWaveform*) fWaveformArray->At(iWaveform);
		hitIndex=theWaveform->GetHitIndex();
		detId=theWaveform->GetDetectorId();
		trackId=theWaveform->GetTrackId();
		module=theWaveform->GetModule();
		// Determine waveform maximum and its position
/*		if(module==5){
			psaAlgorithm_pmt->Process(theWaveform,energy,peakPosition);
			energy/=fWfNormalisation_pmt;
			digi_time=peakPosition/fSampleRate_PMT;

		}
		else{
			psaAlgorithm->Process(theWaveform,energy,peakPosition);
			energy/=fWfNormalisation;
			digi_time=peakPosition/fSampleRate;
		}
		*/
//        nSignal = theWaveform->GetNumberOfWaveforms();
//        if(hitIndex == 45) std::cout << "################################" << std::endl;
		highgain = kTRUE;
		energy = -1;
		peakPosition = -1;
		if(nSignal ==1){
			theWaveform->SetActiveWaveform(0);
			psaAlgorithm_proto192->Process(theWaveform,energy,peakPosition);
		}else{
			theWaveform->SetActiveWaveform(0);
			psaAlgorithm_proto192->Process(theWaveform,energy,peakPosition);
//            if(hitIndex == 45) std::cout << "highgain: " << energy;
			if(energy>1500 || energy <= 0){
				theWaveform->SetActiveWaveform(1);
				psaAlgorithm_proto192->Process(theWaveform,energy,peakPosition);
//                if(hitIndex == 45) std::cout << "lowgain: " << energy;

				highgain=kFALSE;
			}
//            if(hitIndex == 45) std::cout << std::endl;
		}

		energy/=fWfNormalisation_proto192;
		digi_time = peakPosition/fSampleRate;
		std::map<Int_t,Double_t>::iterator it;
		it = fCalibrationMap.find(detId);
		if(it!=fCalibrationMap.end()){
//            if(hitIndex == 45) std::cout << "found calibration value of " << it->second << " for hitIndex " << hitIndex << std::endl;
			energy*=it->second;
		}else{
//            if(hitIndex == 45) std::cout << "no calibration value for hitIndex " << hitIndex << std::endl;
		}
		if(!highgain){
//            if(hitIndex == 45) std::cout << "highgain" << std::endl;
			it=fGainMap.find(detId);
			if(it!=fGainMap.end()){
//            if(hitIndex == 45) std::cout << "found highgain value of " << it->second << " for hitIndex " << hitIndex << std::endl;
				energy*=it->second;
			}
		}
//        if(energy>1000){
//            if(hitIndex == 45) std::cout << "energy: " << energy << " threshold: "<< fEnergyDigiThreshold << endl;
//            if(hitIndex == 45) std::cout << "creating digi for detid: " << detId << "hitIndex: " << hitIndex <<std::endl;
//        }
		if (energy>fEnergyDigiThreshold)
		{
//            if(hitIndex == 45) std::cout << "energy: " << energy << endl;
			PndEmcDigi* myDigi = new((*fDigiArray)[i_digi]) PndEmcDigi(trackId,detId, energy, digi_time, hitIndex);
			myDigi->AddLink(FairLink("EmcMultiWaveform", iWaveform));
			i_digi++;
			
		}
	}
	if (fVerbose>2){
		timer.Stop();
		Double_t rtime = timer.RealTime();
		Double_t ctime = timer.CpuTime();
		cout << "PndEmcMultiWaveformToCalibratedDigi, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	}

}

void PndEmcMultiWaveformToCalibratedDigi::SetParContainers() {

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar*) db->getContainer("PndEmcDigiPar");
  
  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");
 
}

void PndEmcMultiWaveformToCalibratedDigi::SetStorageOfData(Bool_t val)
{
  fStoreDigis = val;
  return;
}

void PndEmcMultiWaveformToCalibratedDigi::ReadCalibrationFile(){
	std::ifstream in(fCalibrationFileName,std::ifstream::in);
//    in.open(fCalibrationFileName);
	if (!in.good()) {
		std::cerr << "Cannot open calibration file!" << endl;
		return;
	}

	char buf[255];
	while (in.getline(buf, 255)) {
		TString tmp = buf;
		TObjArray *tokens = tmp.Tokenize(" \t;");
		if(tokens->GetEntries()<2){
			continue;
		}
		tmp=tokens->UncheckedAt(0)->GetName();
		TString tmp2 = tokens->UncheckedAt(1)->GetName();
		if(tmp.IsDigit() && tmp2.IsFloat()){
			fCalibrationMap.insert(std::pair<Int_t,Double_t>(tmp.Atoi(),tmp2.Atof()));

		}
		if(tokens->GetEntries()<3){
			continue;
		}
		tmp2 = tokens->UncheckedAt(2)->GetName();
		if(tmp.IsDigit() && tmp2.IsFloat()){
			fGainMap.insert(std::pair<Int_t,Double_t>(tmp.Atoi(),tmp2.Atof()));

		}
		delete tokens;
		
		
	}
	if(fVerbose >1){
		std::cout << "calibration values:" << std::endl;
		std::map<Int_t,Double_t>::iterator it;
		std::map<Int_t,Double_t>::iterator it2;
		Int_t detid;
		for ( it=fCalibrationMap.begin() ; it != fCalibrationMap.end(); it++ ){
			detid = (*it).first;
			std::cout << detid << "\t" << (*it).second << "\t";
			it2=fGainMap.find(detid);
			if(it2!=fGainMap.end()){
				std::cout << (*it2).second;
			}
			std::cout << endl;
		}
	}
}


ClassImp(PndEmcMultiWaveformToCalibratedDigi)
