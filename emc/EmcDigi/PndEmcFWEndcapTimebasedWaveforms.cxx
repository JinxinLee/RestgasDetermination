//----------------------------------------------------------------------
// Author List:
// 	Philippp Mahlberg
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
#include "PndEmcFWEndcapDigiPar.h"
#include "PndEmcGeoPar.h"		
#include "PndEmcDataTypes.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLink.h"

#include "TStopwatch.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TClonesArray.h"

#include <iostream>
#include <cassert>

using std::cout;
using std::cerr;
using std::endl;
using std::fstream;


PndEmcFWEndcapTimebasedWaveforms::PndEmcFWEndcapTimebasedWaveforms(Int_t verbose, Bool_t storewaves) :
	FairTask("PndEmcFWEndcapTimebasedWaveforms", verbose),	
	fHitArray(NULL), fWaveformBuffer(NULL), fStoreWaves(storewaves), fStoreDataClass(kFALSE), fActivateBuffering(kFALSE), fDigiPar(NULL), fGeoPar(NULL), fUse_photon_statistic(kFALSE), fNPhotoElectronsPerMeV(0), fExcessNoiseFactor(0.), fExternalSimulator(NULL), fAPD_LOWHIGH(NULL) 
{
}

//--------------
// Destructor --
//--------------
PndEmcFWEndcapTimebasedWaveforms::~PndEmcFWEndcapTimebasedWaveforms() 
{
}


/**
 * @brief Init Task
 * 
 * Prepares the TClonesArray of PndEmcHit for reading and PndEmcMultiWaveform for writing.
 * Also reads the EMC parameters and prepares the waveform simulator (PndEmcMultiWaveformSimulator).
 * 
 * @return InitStatus
 * @retval kSUCCESS success
 */
InitStatus PndEmcFWEndcapTimebasedWaveforms::Init()
{
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

	// Create and activiate output Buffer....choose between PndEmcWaveform and PndEmcMultiWaveform
  #ifndef MULTI
	fWaveformBuffer = new PndEmcWaveformBuffer("EmcWaveform", "PndEmcWaveform", "Emc", fStoreWaves);	
  #else
	fWaveformBuffer = new PndEmcWaveformBuffer("EmcWaveform", "PndEmcMultiWaveform", "Emc", fStoreWaves);	
  #endif

	fWaveformBuffer = (PndEmcWaveformBuffer*) ioman ->RegisterWriteoutBuffer("EmcTimebasedWaveform", fWaveformBuffer);
	fWaveformBuffer->ActivateBuffering(fActivateBuffering);

	if (fStoreDataClass) {
		fWaveformBuffer->StoreWaveformData("EmcWaveformData", "Emc", fStoreWaves);
	}

	
	if(!fDigiPar) {
		cout << "-E- PndEmcFWEndcapTimebasedWaveforms::Init: "
			<< "no DigiPar containter found" << endl;
		return kERROR;
	}

	fUse_photon_statistic = fDigiPar->GetUse_photon_statistic();

	if(fUse_photon_statistic) {
		//rear surface of FWEndcap crystal: 676 mm^2
		fNPhotoElectronsPerMeV = fDigiPar->GetDetectedPhotonsPerMeV() * fDigiPar->GetSensitiveAreaAPD()/ 676. * fDigiPar->GetQuantumEfficiencyAPD();
		fExcessNoiseFactor = fDigiPar->GetExcessNoiseFactorAPD();
	} else {
		fNPhotoElectronsPerMeV = 0;
		fExcessNoiseFactor = 1;
	}

	fGeoPar->InitEmcMapper();
	PndEmcStructure::Instance();
		
	const Double_t tBefore = fDigiPar->GetTimeBeforeHit();
	const Double_t tAfter = fDigiPar->GetTimeAfterHit();
	const Double_t cutoff=fDigiPar->GetWfCutOffEnergy(); 			//GeV 		//0.001
	const Double_t activeTimeIncrement=10; 					//ns		//50
	const Double_t sampleRate = fDigiPar->GetSampleRate();			//ns^-1 <-> GHz	//0.08
	const Int_t bits = fDigiPar->GetNBits(); 				//1		//14

	const Double_t tau = fDigiPar->GetPulseshapeTau();  			//ns		//68.7
	const Double_t N = fDigiPar->GetPulseshapeN();				//N		//1.667

	const Double_t energyRange_HIGH = fDigiPar->GetEnergyRangeHigh(); 	//GeV		//1
	const Double_t energyRange_LOW = fDigiPar->GetEnergyRangeLow(); 	//GeV		//15
	const Double_t noiseWidth_HIGH = fDigiPar->GetNoiseWidthLow();		//GeV		//0.0023 				
	const Double_t noiseWidth_LOW = fDigiPar->GetNoiseWidthHigh();		//GeV		//0.0035 				

	PndEmcAbsPulseshape* fPulseshape = new PndEmcFittedPulseshape(tau, N);
	fAPD_LOWHIGH = new PndEmcMultiWaveformSimulator(sampleRate, fPulseshape, 2);

	fAPD_LOWHIGH->Init(tBefore, tAfter, cutoff, activeTimeIncrement);

	fAPD_LOWHIGH->AddModifier(new PndEmcShapingNoiseAdder(fAPD_LOWHIGH->GetPulseRiseTime(), sampleRate, noiseWidth_HIGH, fAPD_LOWHIGH->GetTotalScale(0)), 0);
	fAPD_LOWHIGH->AddModifier(new PndEmcShapingNoiseAdder(fAPD_LOWHIGH->GetPulseRiseTime(), sampleRate, noiseWidth_LOW, fAPD_LOWHIGH->GetTotalScale(1)), 1);
	fAPD_LOWHIGH->AddModifier(new PndEmcWaveformDigitizer(bits, energyRange_HIGH, fAPD_LOWHIGH->GetTotalScale(0)), 0);
	fAPD_LOWHIGH->AddModifier(new PndEmcWaveformDigitizer(bits, energyRange_LOW, fAPD_LOWHIGH->GetTotalScale(1)), 1);


	return kSUCCESS;
}


/**
 * @brief Runs the task.
 * 
 * Uses the waveform simulator (PndEmcAbsWaveformSimulator) to generate a waveform
 * from the hit and fill the buffer (PndEmcWaveformBuffer).
 * 
 * @param opt unused
 * @return void
 */
void PndEmcFWEndcapTimebasedWaveforms::Exec(Option_t* opt) 
{
	FairRootManager* ioman = FairRootManager::Instance();

	TStopwatch timer;
	if (fVerbose>0){
		timer.Start();
	}
		
	// Variable declaration
	PndEmcHit* theHit = NULL;
	
	// Loop over PndEmcHits to add them to correspondent waveforms
	Int_t nHits = fHitArray->GetEntriesFast();
	if (fVerbose>2) {
		cout<< "PndEmcFWEndcapTimebasedWaveforms:: Hit array contains " << nHits << " hits" <<endl;
	}

	for (Int_t iHit=0; iHit<nHits; iHit++) {

		theHit = dynamic_cast<PndEmcHit*>(fHitArray->At(iHit));

		if(theHit->GetModule() > 5 ) continue;	//tackles invalid PndEmcHit inforamtion (valid module number = seq 1 5
		
		// select wf Simulator..
		// TODO Add realistic description for other emc modules and make simulator choice module dependent
		// switch(theHit->GetModule() {
		// 	case 1: wfSimulator = ...
		// 	case 2: wfSimulator = ...
		// 	case 3: wfSimulator = ...
		// 	case 4: wfSimulator = ...
		// 	case 5: wfSimulator = ...
		// 	default: continue;
		// }
		PndEmcAbsWaveformSimulator* wfSimulator = NULL;

		if(fExternalSimulator) {
			wfSimulator = fExternalSimulator; 
		} else {
			wfSimulator = fAPD_LOWHIGH;
		}; 

		Double_t energy = theHit->GetEnergy();

		if(fUse_photon_statistic) {
			Double_t crystalPhotonsMeV = 1.0e3 * energy * fNPhotoElectronsPerMeV;
			Double_t photonStatFactor = gRandom->Gaus(1,sqrt(fExcessNoiseFactor/crystalPhotonsMeV));
			energy *=photonStatFactor;
		}	

		//Double_t eventTime = ioman->GetEventTime(); //[R.K. 01/2017] unused variable?

		// construct corresponding waveform data Object
		PndEmcWaveformData wfData(theHit->GetDetectorID(),  wfSimulator);

		//register hit...timebased framework uses ns, whereas emc deals with seconds as time unit
		FairLink linkToHit(-1, ioman->GetEntryNr(), "EmcHit", iHit, 1.0);
	//	std::cout << "-I- PndEmcFWEndcapTimebasedWaveforms::Exec eventTIme: " << ioman->GetEventTime() << " hit Time: " << theHit->GetTime()*1.0e9 << std::endl;
		wfData.AddHit(linkToHit, ioman->GetEventTime() + theHit->GetTime()*1.0e9, theHit->GetEnergy());

		fWaveformBuffer->FillNewData(&wfData);	
		
		if (fVerbose>1){
			timer.Stop();
			Double_t rtime = timer.RealTime();
			Double_t ctime = timer.CpuTime();
				cout << "PndEmcFWEndcapTimebasedWaveforms, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
		}
	}
}

void PndEmcFWEndcapTimebasedWaveforms::SetParContainers() 
{
	// Get run and runtime database
	FairRun* run = FairRun::Instance();
	if ( ! run ) Fatal("SetParContainers", "No analysis run");

	FairRuntimeDb* db = run->GetRuntimeDb();
	if ( ! db ) Fatal("SetParContainers", "No runtime database");

	// Get Emc geometry parameter container
	fGeoPar = (PndEmcGeoPar*) db->getContainer("PndEmcGeoPar");

	// Get Emc digitisation parameter container
	fDigiPar = dynamic_cast<PndEmcFWEndcapDigiPar*>(db->getContainer("PndEmcFWEndcapDigiPar"));
}

ClassImp(PndEmcFWEndcapTimebasedWaveforms)
