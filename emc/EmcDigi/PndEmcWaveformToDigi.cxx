//----------------------------------------------------------------------
// File and Version Information:
//      $Id: //
// Description:
//      Class PndEmcWaveformToDigi. Module to take the ADC waveforms and produces digi.
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

#include "PndEmcWaveformToDigi.h"

#include "PndEmcWaveform.h"
#include "PndEmcDigi.h"
#include "PndEmcDigiPar.h"		
#include "PndEmcRecoPar.h"				
#include "PndEmcPSAParabolic.h"
#include "PndEmcPSATrapDigiFilter.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TStopwatch.h"
		
#include <iostream>
//#include <map>

using std::cout;
using std::endl;
using std::fstream;

PndEmcWaveformToDigi::PndEmcWaveformToDigi(Int_t verbose, Bool_t storedigis)
{
	fVerbose=verbose;
	fDigiPosMethod="depth";// "surface" or "depth"
	fEmcDigiRescaleFactor=1.08;
	fStoreDigis=storedigis;
	//fPndEmcDigiPositionDepth=6.2;
}

//--------------
// Destructor --
//--------------

PndEmcWaveformToDigi::~PndEmcWaveformToDigi()
{
}


InitStatus PndEmcWaveformToDigi::Init()
{
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman )
	{
		cout << "-E- PndEmcWaveformToDigi::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}
	
	// Get input array
	fWaveformArray = (TClonesArray*) ioman->GetObject("EmcWaveform");
	if ( ! fWaveformArray ) {
		cout << "-W- PndEmcWaveformToDigi::Init: "
		<< "No PndEmcWaveform array!" << endl;
		return kERROR;
	}
	
	// Create and register output array
	fDigiArray = new TClonesArray("PndEmcDigi");

	ioman->Register("EmcDigi","Emc",fDigiArray,fStoreDigis);
	fSampleRate=fDigiPar->GetSampleRate();
	fShaping_diff_time=fDigiPar->GetShaping_diff_time();     //s
	fShaping_int_time=fDigiPar->GetShaping_int_time();      //s
	fCrystal_time_constant=fDigiPar->GetCrystal_time_constant();  //s
	fSampleRate=fDigiPar->GetSampleRate();
	fNumber_of_samples_in_waveform=fDigiPar->GetNumber_of_samples_in_waveform();
	fEnergyDigiThreshold=fDigiPar->GetEnergyDigiThreshold();
	fEmcDigiPositionDepth=fRecoPar->GetEmcDigiPositionDepth();
	
	cout<<"fEmcDigiPositionDepth: "<<fEmcDigiPositionDepth<<endl;
				
	if (!fDigiPosMethod.compare("surface"))
	{
		PndEmcDigi::selectDigiPositionMethod( PndEmcDigi::surface, 1., 0. );
	}
	else if (!fDigiPosMethod.compare("depth"))
	{
	   PndEmcDigi::selectDigiPositionMethod( PndEmcDigi::depth, 
			 fEmcDigiRescaleFactor, fEmcDigiPositionDepth);
	}
	else 
	{
		cout << "-W- PndEmcWaveformToDigi::Init: "
		<< "Unknown digi position method!" << endl;
		return kERROR;
	}
	
	fPulseshape= new PndEmcCRRCPulseshape(fShaping_diff_time,fShaping_int_time,fCrystal_time_constant);
	
	// Pulse shape analysis algorithm.
	// At the moment simple parabolic fit is used by default.
	psaAlgorithm = new PndEmcPSAParabolic();

	// Trapezoidal digi filter
	// Parameters of the filter are hardcoded at the moment
// 	std::vector<Double_t> params;
// 	params.push_back(20); // Rise time (in sampling period)
// 	params.push_back(20); // Flat top period (in sampling period)
// 	params.push_back(0); // Shift for energy determination
// 	psaAlgorithm = new PndEmcPSATrapDigiFilter(params);
	
	// Determine normalisation constant for PndEmcWaveform
	PndEmcWaveform *tmpwaveform=new PndEmcWaveform(0,101010001, fNumber_of_samples_in_waveform);
	
	PndEmcHit *gevHit=new PndEmcHit();
	gevHit->SetEnergy(1.0);
	gevHit->SetTime(0.);
	tmpwaveform->UpdateWaveform(gevHit, 0, false, 1., 0., fSampleRate, fPulseshape);
	Double_t tmpPeakPosition;
	psaAlgorithm->Process(tmpwaveform,fWfNormalisation,tmpPeakPosition);

	cout << "-I- PndEmcWaveformToDigi: Intialization successfull" << endl;
	
	return kSUCCESS;
}

void PndEmcWaveformToDigi::Exec(Option_t* opt)
{
	TStopwatch timer;
	if (fVerbose>0){
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
	Int_t nWaveforms = fWaveformArray->GetEntriesFast();
	//cout<<"PndEmcWaveformToDigi: "<<nWaveforms<<" waveforms to convert"<<endl;
	for (Int_t iWaveform=0; iWaveform<nWaveforms; iWaveform++) {
		PndEmcWaveform* theWaveform = (PndEmcWaveform*) fWaveformArray->At(iWaveform);
		hitIndex=theWaveform->GetHitIndex();
		Int_t detId=theWaveform->GetDetectorId();
		Int_t trackId=theWaveform->GetTrackId();
		
		// Determine waveform maximum and its position
		psaAlgorithm->Process(theWaveform,energy,peakPosition);
		energy/=fWfNormalisation;
		digi_time=peakPosition/fSampleRate;
		if (energy>fEnergyDigiThreshold)
		{
			new((*fDigiArray)[i_digi]) PndEmcDigi(trackId,detId, energy, peakPosition,hitIndex);
			i_digi++;
			
		}
	}
	if (fVerbose>0){
		timer.Stop();
		Double_t rtime = timer.RealTime();
		Double_t ctime = timer.CpuTime();
		cout << "PndEmcWaveformToDigi, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	}

}

void PndEmcWaveformToDigi::SetParContainers() {

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

void PndEmcWaveformToDigi::SetStorageOfData(Bool_t val)
{
  fStoreDigis = val;
  return;
}

ClassImp(PndEmcWaveformToDigi)
