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

#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"

#include "TClonesArray.h"
#include "TStopwatch.h"
		
#include <iostream>
#include <map>

using std::cout;
using std::endl;
using std::fstream;

PndEmcWaveformToDigi::PndEmcWaveformToDigi(Int_t verbose)
{
	fVerbose=verbose;
	fDigiPosMethod="depth";// "surface" or "depth"
	fEmcDigiRescaleFactor=1.08;
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
	CbmRootManager* ioman = CbmRootManager::Instance();
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

	ioman->Register("EmcDigi","Emc",fDigiArray,kTRUE);
	fSampleRate=fDigiPar->GetSampleRate();
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
	Double_t max_energy;
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
		theWaveform->fitPeak(max_energy,peakPosition);
		max_energy/=theWaveform->get_scale();
		digi_time=peakPosition/fSampleRate;
		if (max_energy>fEnergyDigiThreshold)
		{
			new((*fDigiArray)[i_digi]) PndEmcDigi(trackId,detId, max_energy, peakPosition,hitIndex);
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
  CbmRunAna* run = CbmRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  CbmRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar*) db->getContainer("PndEmcDigiPar");
  
  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");
 
}

ClassImp(PndEmcWaveformToDigi)
