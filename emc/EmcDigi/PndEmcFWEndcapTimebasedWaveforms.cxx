//----------------------------------------------------------------------
// Author List:
//      Phil Strother                  	Original author
// 	Dima Melnichuk 			- adaption for PANDA
// 	Philippp Mahlberg		- integrtion in timebased simulation concept
//----------------------------------------------------------------------
//
//
#include "PndEmcFWEndcapTimebasedWaveforms.h"

#include "FairRun.h"
#include "FairTask.h"

#include "PndEmcWaveform.h"
#include "PndEmcWaveformData.h"
#include "PndEmcWaveformBuffer.h"
#include "PndEmcAbsWaveformModifier.h"
#include "PndEmcFullStackedWaveformSimulator.h"
#include "PndEmcMultiWaveformSimulator.h"
#include "PndEmcShapingNoiseAdder.h"
#include "PndEmcWaveformDigitizer.h"
#include "PndEmcHit.h"
#include "PndEmcFittedPulseshape.h"
#include "PndEmcMapper.h"
#include "PndEmcStructure.h"
#include "PndEmcDigiPar.h"
#include "PndEmcGeoPar.h"		
#include "PndEmcDataTypes.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLink.h"

#include "TStopwatch.h"
#include "TROOT.h"
#include "TClonesArray.h"

#include <iostream>
#include <cassert>

using std::cout;
using std::cerr;
using std::endl;
using std::fstream;



PndEmcFWEndcapTimebasedWaveforms::PndEmcFWEndcapTimebasedWaveforms(Int_t verbose, Bool_t storewaves):
	fHitArray(new TClonesArray), fDigiPar(new PndEmcDigiPar()), fGeoPar(new PndEmcGeoPar()), fVerbose(verbose), fStoreWaves(storewaves) {
#ifdef USEBUFFER
		fActivateBuffering = kFALSE;
		fStoreDataClass = kFALSE;
#endif
	}


//--------------
// Destructor --
//--------------

PndEmcFWEndcapTimebasedWaveforms::~PndEmcFWEndcapTimebasedWaveforms() {
}


InitStatus PndEmcFWEndcapTimebasedWaveforms::Init() {
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		cout << "-E- PndEmcFWEndcapTimebasedWaveforms::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}
	
	// Get input array
	fHitArray = (TClonesArray*) ioman->GetObject("EmcHit");
	if (!fHitArray) {
		cout << "-W- PndEmcFWEndcapTimebasedWaveforms::Init: "
		<< "No EmcHit array!" << endl;
		return kERROR;
	}


#ifdef USEBUFFER
	// Create and activiate output Buffer....choose between PndEmcWaveform and PndEmcMultiWaveform
	//fWaveformBuffer = new PndEmcWaveformBuffer("EmcWaveform", "PndEmcWaveform", "Emc", fStoreWaves);	
	fWaveformBuffer = new PndEmcWaveformBuffer("EmcWaveform", "PndEmcMultiWaveform", "Emc", fStoreWaves);	
	fWaveformBuffer = (PndEmcWaveformBuffer*) ioman ->RegisterWriteoutBuffer("EmcTimebasedWaveform", fWaveformBuffer);

	fWaveformBuffer->ActivateBuffering(fActivateBuffering);

	if (fStoreDataClass) {
		fWaveformBuffer->StoreWaveformData("EmcWaveformData", "Emc", fStoreWaves);
	}
#else
	fWaveformArray = new TClonesArray(PndEmcMultiWaveform::Class());
	ioman->Register("EmcWaveform", "Emc", fWaveformArray, fStoreWaves);
#endif 
	

	fGeoPar->InitEmcMapper();
	PndEmcStructure::Instance();
	
	cout << "-I- PndEmcFWEndcapTimebasedWaveforms: Intialization successfull" << endl;


	{	//FWD_VPTT_LOWGAIN
		Double_t sampleRate = 0.12;
		Int_t nBits = 15;

		Double_t tau = 68.7; //ns
		Double_t N = 1.667;

		Double_t energyRange = 15.; //GeV	
		Double_t noiseWidth = 0.002;//0.65; //GeV, 0.65 represents expected loss of gain inside magnetic field

		Double_t samplingBeforeFirstPulse = 250;	//ns
		Double_t samplingAfterLastPulse = 250; 		//ns
		Double_t cutoff = 0.001; 			//GeV
		Double_t activeTimeIncrement = 50.; 		//ns

		fVPTT_LOW_Pulseshape = new PndEmcFittedPulseshape(tau, N);

		PndEmcFullStackedWaveformSimulator* wfSim = new PndEmcFullStackedWaveformSimulator(sampleRate, fVPTT_LOW_Pulseshape);
		wfSim->Init(samplingBeforeFirstPulse, samplingAfterLastPulse, cutoff, activeTimeIncrement);

		fVPTT_LOW_Shaping = new PndEmcShapingNoiseAdder(wfSim->GetPulseRaiseTime(), sampleRate, noiseWidth, wfSim->GetScale());
		wfSim->AddModifier(fVPTT_LOW_Shaping);

		fVPTT_LOW_Digitize = new PndEmcWaveformDigitizer(nBits, energyRange, wfSim->GetScale());
		wfSim->AddModifier(fVPTT_LOW_Digitize);

		fVPTT_LOW = wfSim;
	}

	{	//FWD_VPTT_HIGHGAIN
		Double_t sampleRate = 0.12;
		Int_t nBits = 15;

		Double_t tau = 68.7; //ns
		Double_t N = 1.667;

		Double_t energyRange = 1.; //GeV
		Double_t noiseWidth = 0.001;//0.65; //GeV, 0.65 represents expected loss of gain inside magnetic fieldGeV

		Double_t samplingBeforeFirstPulse = 250;
		Double_t samplingAfterLastPulse = 250;
		Double_t cutoff = 0.001; //GeV
		Double_t activeTimeIncrement = 50.; //in ns

		fVPTT_HIGH_Pulseshape = new PndEmcFittedPulseshape(tau, N);

		PndEmcFullStackedWaveformSimulator* wfSim = new PndEmcFullStackedWaveformSimulator(sampleRate, fVPTT_HIGH_Pulseshape);
		wfSim->Init(samplingBeforeFirstPulse, samplingAfterLastPulse, cutoff, activeTimeIncrement);

		fVPTT_HIGH_Shaping = new PndEmcShapingNoiseAdder(wfSim->GetPulseRaiseTime(), sampleRate, noiseWidth, wfSim->GetScale());
		wfSim->AddModifier(fVPTT_HIGH_Shaping);

		fVPTT_HIGH_Digitize = new PndEmcWaveformDigitizer(nBits, energyRange, wfSim->GetScale());
		wfSim->AddModifier(fVPTT_HIGH_Digitize);

		fVPTT_HIGH = wfSim;
	}

	{	//FWD_APD_LOWGAIN
		Double_t sampleRate = 0.08;
		Int_t nBits = 14;

		Double_t tau = 68.7; //ns
		Double_t N = 1.667;

		Double_t energyRange = 15.; //GeV
		Double_t noiseWidth = 0.0035; //GeV

		Double_t samplingBeforeFirstPulse = 250;
		Double_t samplingAfterLastPulse = 250;
		Double_t cutoff = 0.001; //GeV
		Double_t activeTimeIncrement = 50.; //in ns

		fAPD_LOW_Pulseshape = new PndEmcFittedPulseshape(tau, N);

		PndEmcFullStackedWaveformSimulator* wfSim = new PndEmcFullStackedWaveformSimulator(sampleRate, fAPD_LOW_Pulseshape);
		wfSim->Init(samplingBeforeFirstPulse, samplingAfterLastPulse, cutoff, activeTimeIncrement);

		fAPD_LOW_Shaping = new PndEmcShapingNoiseAdder(wfSim->GetPulseRaiseTime(), sampleRate, noiseWidth, wfSim->GetScale());
		wfSim->AddModifier(fAPD_LOW_Shaping);

		fAPD_LOW_Digitize = new PndEmcWaveformDigitizer(nBits, energyRange, wfSim->GetScale());
		wfSim->AddModifier(fAPD_LOW_Digitize);

		fAPD_LOW = wfSim;
	}

	{	//FWD_APD_HIGHGAIN
		Double_t sampleRate = 0.08;
		Int_t nBits = 14;

		Double_t tau = 68.7; //ns
		Double_t N = 1.667;

		Double_t energyRange = 1.; //GeV	
		Double_t noiseWidth = 0.0023; //GeV

		Double_t samplingBeforeFirstPulse = 250;
		Double_t samplingAfterLastPulse = 250;
		Double_t cutoff = 0.001; //GeV
		Double_t activeTimeIncrement = 50.; //in ns

		fAPD_HIGH_Pulseshape = new PndEmcFittedPulseshape(tau, N);
		
		PndEmcFullStackedWaveformSimulator* wfSim = new PndEmcFullStackedWaveformSimulator(sampleRate, fAPD_HIGH_Pulseshape);
		wfSim->Init(samplingBeforeFirstPulse, samplingAfterLastPulse, cutoff, activeTimeIncrement);

		fAPD_HIGH_Shaping = new PndEmcShapingNoiseAdder(wfSim->GetPulseRaiseTime(), sampleRate, noiseWidth, wfSim->GetScale());
		wfSim->AddModifier(fAPD_HIGH_Shaping);

		fAPD_HIGH_Digitize = new PndEmcWaveformDigitizer(nBits, energyRange, wfSim->GetScale());
		wfSim->AddModifier(fAPD_HIGH_Digitize);

		fAPD_HIGH = wfSim;
	}

	{ //VPTT_MULT
		Double_t sampleRate = 0.12;

		Double_t samplingBeforeFirstPulse = 250;
		Double_t samplingAfterLastPulse = 250;
		Double_t cutoff = 0.001; //GeV
		Double_t activeTimeIncrement = 50.; //in ns

		PndEmcMultiWaveformSimulator* wfSim = new PndEmcMultiWaveformSimulator(sampleRate, fVPTT_LOW_Pulseshape, 2);
		wfSim->Init(samplingBeforeFirstPulse, samplingAfterLastPulse, cutoff, activeTimeIncrement);

		wfSim->AddModifier(fVPTT_HIGH_Shaping, 0);
		wfSim->AddModifier(fVPTT_LOW_Shaping, 1);

		wfSim->AddModifier(fVPTT_HIGH_Digitize, 0);
		wfSim->AddModifier(fVPTT_LOW_Digitize, 1);

		fVPTT_MULT = wfSim;
	}
        return kSUCCESS;
}




void PndEmcFWEndcapTimebasedWaveforms::Exec(Option_t* opt) {
	FairRootManager* ioman = FairRootManager::Instance();

	TStopwatch timer;
	if (fVerbose>2){
		timer.Start();
	}
		
	// Variable declaration
	PndEmcHit* theHit = NULL;
	
	// Loop over PndEmcHits to add them to correspondent waveforms
	Int_t nHits = fHitArray->GetEntriesFast();
	if (fVerbose>0) {
		cout<< "PndEmcFWEndcapTimebasedWaveforms:: Hit array contains " << nHits << " hits" <<endl;
	}

	for (Int_t iHit=0; iHit<nHits; iHit++) {

		theHit = dynamic_cast<PndEmcHit*>(fHitArray->At(iHit));
		
		if(theHit->GetEnergy() < 0.001) { //wf simulation threshold
			continue;
		}

		// select wf Simulator
		PndEmcAbsWaveformSimulator* wfSimulator = fVPTT_MULT;

#ifdef USEBUFFER
		Double_t eventTime = ioman->GetEventTime();


		// construct corresponding waveform data Object
		PndEmcWaveformData* wfData = new PndEmcWaveformData(theHit->GetDetectorID(),  wfSimulator);

		//register hit...timebased framewark uses ns, whereas emc deals with seconds as unit of time
		FairLink linkToHit(-1, ioman->GetEntryNr(), "EmcHit", iHit, 1.0);
		wfData->AddHit(linkToHit, ioman->GetEventTime() + theHit->GetTime()*1.0e9, theHit->GetEnergy());


		fWaveformBuffer->FillNewData(wfData);	
		delete wfData; 
#else 
		wfSimulator->SimulateSinglePulse(theHit->GetEnergy(), theHit->GetTime()*1.0e9, fWaveformArray, theHit->GetDetectorID(), -1, iHit);  
#endif
		




		if (fVerbose>2){
			timer.Stop();
			Double_t rtime = timer.RealTime();
			Double_t ctime = timer.CpuTime();
				cout << "PndEmcFWEndcapTimebasedWaveforms, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
		}
	}
}


void PndEmcFWEndcapTimebasedWaveforms::SetParContainers() {

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


void PndEmcFWEndcapTimebasedWaveforms::SetStorageOfData(Bool_t val) {
	fStoreWaves = val;
	return;
}

void PndEmcFWEndcapTimebasedWaveforms::Finish() {

	delete fVPTT_LOW;
	delete fVPTT_HIGH;
	delete fVPTT_MULT;

	delete fAPD_LOW;
	delete fAPD_HIGH;

	delete fVPTT_LOW_Pulseshape;
	delete fVPTT_HIGH_Pulseshape;
	delete fVPTT_LOW_Shaping;
	delete fVPTT_HIGH_Shaping;
	delete fVPTT_LOW_Digitize;
	delete fVPTT_HIGH_Digitize;

	delete fAPD_LOW_Pulseshape;
	delete fAPD_HIGH_Pulseshape;
	delete fAPD_LOW_Shaping;
	delete fAPD_HIGH_Shaping;
	delete fAPD_LOW_Digitize;
	delete fAPD_HIGH_Digitize;

}

ClassImp(PndEmcFWEndcapTimebasedWaveforms)
