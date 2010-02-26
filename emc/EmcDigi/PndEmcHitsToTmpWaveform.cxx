//----------------------------------------------------------------------
// File and Version Information:
//      $Id: //
// Description:
//      Class PndEmcHitsToTmpWaveform. Module to take the hit list for the 
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

#include "PndEmcHitsToTmpWaveform.h"

#include "PndEmcHit.h"
#include "PndEmcWaveform.h"
#include "PndEmcMapper.h"
#include "PndEmcStructure.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDigiPar.h"		
#include "PndEmcDataTypes.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TStopwatch.h"
#include "TROOT.h"
#include "TClonesArray.h"

#include "IfdProxyDict.h"
#include "IfdStdVectorProxy.h"
#include "Ifd.h"

#include "AbsEnv.h"
#include <iostream>
//#include <map>
//#include <string>

using std::cout;
using std::endl;
using std::fstream;

PndEmcHitsToTmpWaveform::PndEmcHitsToTmpWaveform(Int_t verbose, Bool_t storewaves)
{
	fVerbose = verbose;
	fStoreWaves = storewaves;
}

//--------------
// Destructor --
//--------------

PndEmcHitsToTmpWaveform::~PndEmcHitsToTmpWaveform()
{
}


InitStatus PndEmcHitsToTmpWaveform::Init()
{
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman )
	{
		cout << "-E- PndEmcHitsToTmpWaveform::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}
	
	// Get input array
	fHitArray = (TClonesArray*) ioman->GetObject("EmcHit");
	if ( ! fHitArray ) {
		cout << "-W- PndEmcHitsToTmpWaveform::Init: "
		<< "No EmcHit array!" << endl;
		return kERROR;
	}


	fNBits=fDigiPar->GetNBits();
	fDetectedPhotonsPerMeV=fDigiPar->GetDetectedPhotonsPerMeV();
	fEnergyRange=fDigiPar->GetEnergyRange(); //GeV
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
	
	// Test how parameters were read from DB.
	cout<<"nBits "<<fNBits<<endl;
	cout<<"detectedPhotonsPerMeV "<<fDetectedPhotonsPerMeV<<endl;
	cout<<"energyRange "<<fEnergyRange<<endl;
	cout<<"excessNoiseFactor "<<fExcessNoiseFactor<<endl;
	cout<<"firstSamplePhase "<<fFirstSamplePhase<<endl;
	cout<<"number_of_samples_in_waveform "<<fNumber_of_samples_in_waveform<<endl;
	cout<<"Shaping_diff_time "<<fShaping_diff_time<<endl;
	cout<<"Shaping_int_time "<<fShaping_int_time<<endl;
	cout<<"crystal_time_constant "<<fCrystal_time_constant<<endl;
	cout<<"incoherent_elec_noise_width_GeV "<<fIncoherent_elec_noise_width_GeV<<endl;
	cout<<"sampleRate "<<fSampleRate<<endl;
	cout<<"use_shaped_noise "<<fUse_shaped_noise<<endl;
	cout<<"use_photon_statistic "<<fUse_photon_statistic<<endl;

	fGeoPar->InitEmcMapper();
	PndEmcStructure::Instance();

	// Calculate 1 bit resolution (in units of FADC amplitude)
	PndEmcWaveform *tmpwaveform;
	tmpwaveform=new PndEmcWaveform(0,101010001, fShaping_diff_time, fShaping_int_time, fSampleRate, 0.0,0.0,fCrystal_time_constant,fNumber_of_samples_in_waveform,fExcessNoiseFactor,false);

	fGevPeakAnalogue = tmpwaveform->get_scale();
	
	fOneBitResolution=fEnergyRange/((double) (1<<fNBits))*fGevPeakAnalogue;
	
	fFirstADCBinTime=fFirstSamplePhase/fSampleRate;
	
	delete tmpwaveform;
	
	return kSUCCESS;
}

void PndEmcHitsToTmpWaveform::Exec(Option_t* opt)
{


  vector<PndEmcWaveform*> *theWaveformList       = new vector<PndEmcWaveform*>;
  
  IfdStdVectorProxy<PndEmcWaveform*>  *stdProxyWaveform  = new IfdStdVectorProxy<PndEmcWaveform*> ( theWaveformList  );
  
  Ifd< vector<PndEmcWaveform*> >::put( gblEvtDict, stdProxyWaveform );


	TStopwatch timer;
	if (fVerbose>0){
		timer.Start();
	}

	// Variable declaration
	PndEmcHit* theHit = NULL;
	PndEmcWaveform* theWaveform = NULL;
	
	// Loop over PndEmcHits to add them to correspondent waveforms
	// <set> fWaveformInd contains indexes of detectors for which Waveforms are created
	Int_t nHits = fHitArray->GetEntriesFast();
	cout<<"Hit array contains "<<nHits<< " hits"<<endl;
	for (Int_t iHit=0; iHit<nHits; iHit++) {
		theHit = (PndEmcHit*) fHitArray->At(iHit);
		Int_t detId=theHit->GetDetectorID();
		fWaveformInd.insert(detId);
		theWaveform = AddWaveform(detId,iHit);
		theWaveform->update_waveform(theHit);
		theWaveformList->push_back(theWaveform);                
	}
	
	// The following code finds the neigbouring elements for existing waveforms and if the waveform for this element does not exist it is created.
	// I.e. the PndEmcHitsToTmpWaveform task produces the waveform not only for the cristals where hit took place but also fo their neighbourhood
// 	Int_t nWf = fWaveformArray->GetEntriesFast();

	Int_t nWf = theWaveformList->size();	
	if (fVerbose>0){
		cout << "Initial number of waveforms = "<<nWf<<endl;
	}

	Int_t i_skipped=0;
	Int_t detId_tmp;
        
	for (int i=0; i<nWf;i++)
	  {
	    PndEmcTwoCoordIndex* theIndex=(*theWaveformList)[i]->GetTCI();
            if (0!=theIndex)
	      {
		PndEmcCoordIndexSet theNeighbourStore=theIndex->GetNeighbours();
		PndEmcCoordIndexSet::iterator theNeighbourIterator;
		PndEmcTwoCoordIndex *theNeighbourIndex;
		for (theNeighbourIterator = theNeighbourStore.begin(); theNeighbourIterator != theNeighbourStore.end(); ++theNeighbourIterator )  {
		  theNeighbourIndex = (PndEmcTwoCoordIndex*)(*theNeighbourIterator); 
		  detId_tmp =theNeighbourIndex->Index();
		  //		  check if waveform with such index exists
		  if (fWaveformInd.insert(detId_tmp).second){
    		    PndEmcWaveform* tmpWaveform= AddWaveform(detId_tmp,-1); // -1 correponds to Waveform produced not from EmcHit but from Noise
 		    theWaveformList->push_back(tmpWaveform); 
		    
		  }
		}
	      }	
	  }
	// Add electronic noise
	// There are two options how to add noise (before and after shaping) 

	nWf = theWaveformList->size();
	if (fVerbose>0){
		cout << "Number of waveforms after adding neighboring elements= "<<nWf<<endl;
	}
	

	vector<PndEmcWaveform*>::iterator it2=theWaveformList->begin();
	for (it2; it2!=theWaveformList->end();it2++)
	  {
	    if (fUse_shaped_noise==0)
		{
		  (*it2)->add_elec_noise_and_digitise(fIncoherent_elec_noise_width_GeV*fGevPeakAnalogue,fOneBitResolution);
		} else {
		        (*it2)->add_shaped_elec_noise_and_digitise(fIncoherent_elec_noise_width_GeV*fGevPeakAnalogue,fOneBitResolution);
		}
	  }

	if (fVerbose>0){
		timer.Stop();
		Double_t rtime = timer.RealTime();
		Double_t ctime = timer.CpuTime();
		cout << "PndEmcHitsToTmpWaveform, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	}

}

void PndEmcHitsToTmpWaveform::SetParContainers() {

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
PndEmcWaveform* PndEmcHitsToTmpWaveform::AddWaveform(Int_t detID, Int_t iHit){

	return new PndEmcWaveform(0,detID,
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

void PndEmcHitsToTmpWaveform::SetStorageOfData(Bool_t val)
{
  fStoreWaves = val;
  return;
}

ClassImp(PndEmcHitsToTmpWaveform)
