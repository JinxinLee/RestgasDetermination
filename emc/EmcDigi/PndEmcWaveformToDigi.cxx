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
#include "PndEmcAsicPulseshape.h"
#include "PndEmcPSAParabolic.h"
#include "PndEmcPSAMatchedDigiFilter.h"

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

PndEmcWaveformToDigi::PndEmcWaveformToDigi(Int_t verbose, Bool_t storedigis):
  fWaveformArray(new TClonesArray()), fDigiArray(new TClonesArray()), fSampleRate(0), fSampleRate_PMT(0), fEnergyDigiThreshold(0), fASIC_Shaping_int_time(0), fPMT_Shaping_int_time(0), fPMT_Shaping_diff_time(0), fCrystal_time_constant(0), fShashlyk_time_constant(0), fNumber_of_samples_in_waveform(0), fNumber_of_samples_in_waveform_pmt(0), fDigiPosMethod(0), fEmcDigiRescaleFactor(0), fEmcDigiPositionDepthPWO(0), fEmcDigiPositionDepthShashlyk(0), fPulseshape(0), fPulseshape_pmt(0), psaAlgorithm(0), psaAlgorithm_pmt(0), fDigiPar(new PndEmcDigiPar()), fRecoPar(new PndEmcRecoPar()), fVerbose(verbose), fStoreDigis(storedigis), fWfNormalisation(0), fWfNormalisation_pmt(0), fTimeOrderedDigi(kFALSE)
{
  fDigiPosMethod="depth";// "surface" or "depth"
  fEmcDigiRescaleFactor=1.08;
  //fPndEmcDigiPositionDepth=6.2;
}

//--------------
// Destructor --
//--------------

PndEmcWaveformToDigi::~PndEmcWaveformToDigi()
{
	if (fDataBuffer!= 0) delete fDataBuffer;
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
//	fDigiArray = new TClonesArray("PndEmcDigi");
//	ioman->Register("EmcDigi","Emc",fDigiArray,fStoreDigis);
	
	fDataBuffer = new PndEmcDigiWriteoutBuffer("EmcDigi", "Emc", fStoreDigis);
	fDataBuffer = (PndEmcDigiWriteoutBuffer*)ioman->RegisterWriteoutBuffer("EmcDigi", fDataBuffer);
	fDataBuffer->ActivateBuffering(fTimeOrderedDigi);


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
		cout << "-W- PndEmcWaveformToDigi::Init: "
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
	
	// Determine normalisation constant for PndEmcWaveform
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

	cout << "-I- PndEmcWaveformToDigi: Intialization successfull" << endl;
	
	return kSUCCESS;
}

void PndEmcWaveformToDigi::Exec(Option_t* opt)
{
	TStopwatch timer;
	if (fVerbose>2){
		timer.Start();
	}
	
	Double_t EventTime = FairRootManager::Instance()->GetEventTime();

	Double_t peakPosition;
	Double_t energy;
	Double_t digi_time;
	Int_t hitIndex;
	Int_t detId;
	Int_t trackId;
	Int_t module;
	Int_t nWaveforms = fWaveformArray->GetEntriesFast();
	//cout<<"PndEmcWaveformToDigi: "<<nWaveforms<<" waveforms to convert"<<endl;
	for (Int_t iWaveform=0; iWaveform<nWaveforms; iWaveform++) {
		PndEmcWaveform* theWaveform = (PndEmcWaveform*) fWaveformArray->At(iWaveform);
		hitIndex=theWaveform->GetHitIndex();
		detId=theWaveform->GetDetectorId();
		trackId=theWaveform->GetTrackId();
		module=theWaveform->GetModule();

		Double_t timeshift; // how maximum is shifted
		if(module==5){
			psaAlgorithm_pmt->Process(theWaveform,energy,peakPosition);
			energy/=fWfNormalisation_pmt;
			digi_time=peakPosition/fSampleRate_PMT*1e9;//ns

		}
		else{
			psaAlgorithm->Process(theWaveform,energy,peakPosition);
			energy/=fWfNormalisation;
			digi_time=peakPosition/fSampleRate*1e9;//ns
		}
		if (energy>fEnergyDigiThreshold)
		{

			Double_t timestamp=EventTime+digi_time;	
			PndEmcDigi* myDigi = new PndEmcDigi(trackId,detId, energy, timestamp, hitIndex);
			myDigi->AddLink(FairLink("EmcWaveform", iWaveform));
			fDataBuffer->FillNewData(myDigi, 300, 300); // 300 ns
			if (fVerbose>2)
				cout<<"timestamp="<<timestamp<<" EventTime="<<EventTime<<" digi_time="<<digi_time<<endl;
		}
	}
	if (fVerbose>2){
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
