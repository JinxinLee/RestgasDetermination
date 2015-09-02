#include "PndEmcFWEndcapDigi.h"

#include "PndEmcWaveformData.h"
#include "PndEmcMultiWaveform.h"
#include "PndEmcPSAFPGA/PndEmcPSAFPGAPileupAnalyser.h"
#include "PndEmcPSAFPGA/PndEmcHighLowPSA.h"
#include "PndEmcDigi.h"
#include "PndEmcFWEndcapDigiPar.h"		
#include "PndEmcGeoPar.h"		
#include "PndEmcRecoPar.h"				
#include "PndEmcMapper.h"
#include "PndEmcStructure.h"
#include "PndEmcSimCrystalCalibrator.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TStopwatch.h"
#include "TF1.h"
		
#include <iostream>
#include <vector>
#include <utility>

using std::cout;
using std::endl;

PndEmcFWEndcapDigi::PndEmcFWEndcapDigi(Int_t verbose, Bool_t storedigis):
  fWaveformArray(NULL), fDigiArray(NULL), fEnergyDigiThreshold(0), fDigiPosMethod(""), fEmcDigiRescaleFactor(0.), fEmcDigiPositionDepthPWO(0), fEmcDigiPositionDepthShashlyk(0), fHighgainPSA(NULL), fLowgainPSA(NULL), fHighLowPSA(verbose), fCalibrator(NULL), fDigiPar(NULL), fRecoPar(NULL), fGeoPar(NULL), fVerbose(verbose), fStoreDigis(storedigis), fTimeOrderedDigi(kFALSE)
{
}

//--------------
// Destructor --
//--------------

PndEmcFWEndcapDigi::~PndEmcFWEndcapDigi()
{
	if (fDigiArray!= 0) delete fDigiArray;
}


//#define DEBUG2020

#ifdef DEBUG2020
#include "PndEmcWaveformData.h"
static TClonesArray* fWfDataArray;
#endif

InitStatus PndEmcFWEndcapDigi::Init()
{
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman )
	{
		cout << "-E- PndEmcFWEndcapDigi::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}
	
	// Get input array
	fWaveformArray = (TClonesArray*) ioman->GetObject("EmcMultiWaveform");
	if (!fWaveformArray) {
		//check if EmcWaveform contains MultiWaveforms
		fWaveformArray = (TClonesArray*) ioman->GetObject("EmcWaveform");
		if((!fWaveformArray) || (!fWaveformArray->GetClass()->InheritsFrom("PndEmcMultiWaveform"))){
		cout << "-W- PndEmcFWEndcapDigi::Init: "
		<< "No PndEmcWaveform array containing multi waveforms!" << endl;
		return kERROR;
		}

	}
	
#ifdef DEBUG2020
	fWfDataArray = (TClonesArray*) ioman->GetObject("EmcWaveformData");
#endif

	if(!fDigiPar) {
		cout << "-E- PndEmcFWEndcapTimebasedWaveforms::Init: "
			<< "no DigiPar containter found" << endl;
		return kFATAL;
	}
	
	// Create and register output array
	fDigiArray = ioman->Register("EmcDigi","PndEmcDigi", "Emc", fStoreDigis);

	//position methods
	fEmcDigiPositionDepthPWO=fRecoPar->GetEmcDigiPositionDepthPWO();
	fEmcDigiPositionDepthShashlyk=fRecoPar->GetEmcDigiPositionDepthShashlyk();
	
	cout<<"fEmcDigiPositionDepthPWO: "<<fEmcDigiPositionDepthPWO<<endl;
	cout<<"fEmcDigiPositionDepthShashlyk: "<<fEmcDigiPositionDepthShashlyk<<endl;

	//digi pos method				
	fDigiPosMethod = "depth";	//ATTN: hardcoded...do we need this at all?
	fEmcDigiRescaleFactor = 1.08;	
	if (!fDigiPosMethod.CompareTo("surface")) {
		PndEmcDigi::selectDigiPositionMethod( PndEmcDigi::surface, 0., 0., 1.0 );
	} else if (!fDigiPosMethod.CompareTo("depth")) {
	   PndEmcDigi::selectDigiPositionMethod(PndEmcDigi::depth, fEmcDigiPositionDepthPWO, fEmcDigiPositionDepthShashlyk, fEmcDigiRescaleFactor);
	} else {
		cout << "-W- PndEmcFWEndcapDigi::Init: " << "Unknown digi position method!" << endl;
		return kERROR;
	}

	// psa
	if(!fDigiPar->GetPsaTypeLow().IsNull()) {

		if(fLowgainPSA) {
			std::cout << "-W in PndEmcFWEndcapDigi::Init: Lowgain PSA already set. Skipping default initialization" << std::endl;
		} else if(!fDigiPar->GetPsaTypeLow().CompareTo("PSAFPGAPileupAnalyser")) {
			PndEmcPSAFPGAPileupAnalyser* psa = new PndEmcPSAFPGAPileupAnalyser();
			psa->SetVerbose(fVerbose);

			TObjArray* rparas = fDigiPar->GetRValueParLow().Tokenize(";");
			TF1 R_thres("R_thres_Low", rparas->GetEntriesFast()>0 ? rparas->UncheckedAt(0)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));				
			TF1 R_mean("R_mean_Low", rparas->GetEntriesFast()>1 ? rparas->UncheckedAt(1)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));				

			delete rparas;

			psa->Init(std::vector<Double_t>(fDigiPar->GetPsaParLow().GetArray(), fDigiPar->GetPsaParLow().GetArray()+fDigiPar->GetPsaParLow().GetSize()), R_thres.IsZombie() ? NULL : &R_thres, R_mean.IsZombie() ? NULL : & R_mean);

			fLowgainPSA = psa;

		} else {
			std::cerr << "-E in PndEmcFWEndcapDigi::Init could not find PSA of type: " << fDigiPar->GetPsaTypeLow() << std::endl;
		}
	}

	if(!fDigiPar->GetPsaTypeHigh().IsNull()) {

		if(fHighgainPSA) {
			std::cout << "-W in PndEmcFWEndcapDigi::Init: Highgain PSA already set. Skipping default initialization" << std::endl;
		} else if(!fDigiPar->GetPsaTypeHigh().CompareTo("PSAFPGAPileupAnalyser")) {
			PndEmcPSAFPGAPileupAnalyser* psa = new PndEmcPSAFPGAPileupAnalyser();
			psa->SetVerbose(fVerbose);

			TObjArray* rparas = fDigiPar->GetRValueParHigh().Tokenize(";");
			TF1 R_thres("R_thres_High", rparas->GetEntriesFast()>0 ? rparas->UncheckedAt(0)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));				
			TF1 R_mean("R_mean_High", rparas->GetEntriesFast()>1 ? rparas->UncheckedAt(1)->GetName() : "", 1, TMath::Power(2, fDigiPar->GetNBits()));				

			delete rparas;

			psa->Init(std::vector<Double_t>(fDigiPar->GetPsaParHigh().GetArray(), fDigiPar->GetPsaParHigh().GetArray()+fDigiPar->GetPsaParHigh().GetSize()), R_thres.IsZombie() ? NULL : &R_thres, R_mean.IsZombie() ? NULL : & R_mean);

			fHighgainPSA = psa;
		} else {
			std::cerr << "-E in PndEmcFWEndcapDigi::Init could not find PSA of type: " << fDigiPar->GetPsaTypeHigh() << std::endl;
		}
	}


	if(fHighgainPSA && fLowgainPSA) {
		fHighLowPSA.Init(fHighgainPSA, fLowgainPSA, fDigiPar->GetSignalOverflowHigh(), 0, 1);
	} else {
		std::cerr << "-E- PndEmcFWEndcapDigi::Init " << "No highgain and/or lowgain psa" << std::endl;
		return kFATAL;
	}

	//calibration
	if(fCalibrator != NULL) {
			std::cout << "-W in PndEmcFWEndcapDigi::Init: Calibrator already set. Skipping default initialization"  << std::endl;
	} else {
		fCalibrator = new PndEmcSimCrystalCalibrator();
		for(Int_t imod=1; imod<=5; ++imod) { //TODO: different calibration constants for different modules
			fCalibrator->SetCalibration(imod, fDigiPar->GetCalibHigh(), 0, fDigiPar->GetSignalOverflowHigh()); 	//idx:0 <-> highgain
			fCalibrator->SetCalibration(imod, fDigiPar->GetCalibLow(), 1, 0); 					//idx:1 <-> lowgain
		}
		fCalibrator->Init();
	}

	fEnergyDigiThreshold=fDigiPar->GetEnergyDigiThreshold();

	fGeoPar->InitEmcMapper();
	PndEmcStructure::Instance();
	
	cout << "-I- PndEmcFWEndcapDigi: Intialization successfull" << endl;
	return kSUCCESS;
}

void PndEmcFWEndcapDigi::Exec(Option_t* opt)
{
	TStopwatch timer;
	if (fVerbose>2){
		timer.Start();
	}

	fDigiArray->Delete();

	Double_t energy;
	Double_t digi_time;
	Int_t hitIndex;
	Int_t nHits;
	Int_t detId;
	Int_t trackId;
	Int_t nWaveforms = fWaveformArray->GetEntriesFast();
	PndEmcWaveform* theWaveform;

	for (Int_t iWaveform=0; iWaveform<nWaveforms; iWaveform++) {

		theWaveform = (PndEmcWaveform*) fWaveformArray->At(iWaveform);
		hitIndex=theWaveform->GetHitIndex();
		detId=theWaveform->GetDetectorId();
		trackId=theWaveform->GetTrackId();

		Double_t timeshift; // how maximum is shifted

		#ifdef DEBUG2020

		if(theWaveform->GetDetectorId() != 320000020) continue;

		//(dynamic_cast<PndEmcPSAFPGAPileupAnalyser*>(fLowgainPSA))->SetVerbose(5);
		//(dynamic_cast<PndEmcPSAFPGAPileupAnalyser*>(fHighgainPSA))->SetVerbose(5);
		fHighLowPSA.SetVerbose(5);

		nHits = fHighLowPSA.Process(theWaveform);	

		std::cout << "\nhits in waveform data\n" << *((PndEmcWaveformData*) fWfDataArray->UncheckedAt(iWaveform)) << std::endl;
		cout << "--------------------------------\nfeature extraction found " << nHits << " hits" << endl;

		#else 
		nHits = fHighLowPSA.Process(theWaveform);	
		#endif


		for(Int_t iHit=0; iHit<nHits; ++iHit) {
			fHighLowPSA.GetHit(iHit, energy, digi_time);	
			PndEmcAbsCrystalCalibrator::CalibrationStatus_t CalStat;

			//cout << "#" << iHit << ".\tDetId: " << detId << "\tsample time: " << digi_time << "\traw energy: " << energy << "\t";
			CalStat = fCalibrator->Calibrate(energy, detId, fHighLowPSA.GetWaveformIdx(iHit));

			Double_t sampleRate = theWaveform->GetSampleRate();

			digi_time/=sampleRate;
			digi_time*=1e9;		//ns

			#ifdef DEBUG2020
			std::cout << "\t#" << iHit <<"\ttime: " << digi_time+theWaveform->GetTimeStamp() << "\tenergy: " << energy << "\t\t digi time: " << digi_time << std::endl;
			#endif
			
			if (energy>fEnergyDigiThreshold) {
				Double_t timestamp=theWaveform->GetTimeStamp() + digi_time;
				//std::cout << "PndEmcFWEndcapDigi::Exec Waveform TS: " << theWaveform->GetTimeStamp() << " digiTime: " << digi_time << std::endl;

				PndEmcDigi* myDigi = new((*fDigiArray)[fDigiArray->GetEntriesFast()]) PndEmcDigi(trackId,detId, energy, timestamp, hitIndex);
				myDigi->ResetLinks();
				myDigi->AddLinks(theWaveform->GetLinksWithType(FairRootManager::Instance()->GetBranchId("EmcHit")));

				//std::cout << "-I- PndEmcFWEndcapDigi Links MyDigi: " << *myDigi->GetPointerToLinks() << std::endl;
				//std::cout << "Waveform: " << *theWaveform->GetPointerToLinks() << std::endl;
				//myDigi->AddLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), "EmcWaveform", iWaveform));
				//std::cout << "\t accepted" << std::endl;
				//std::cout << "digi at:" << fDigiArray->GetEntriesFast()-1 << " links to: " << myDigi->GetLink(0).GetIndex() << " iwaveform: " << iWaveform << " number of links: " << myDigi->GetNLinks() << std::endl;
				//std::cout << "#0: " << myDigi->GetLink(0) << "\n#1:" << myDigi->GetLink(1) << std::endl;
				//std::cout << "\t rejected" << std::endl;
			}
				
		}

		#ifdef DEBUG2020
		getchar();
		(dynamic_cast<PndEmcPSAFPGAPileupAnalyser*>(fLowgainPSA))->SetVerbose(0);
		(dynamic_cast<PndEmcPSAFPGAPileupAnalyser*>(fHighgainPSA))->SetVerbose(0);
		fHighLowPSA.SetVerbose(0);
		#endif
	}

	if (fVerbose>2) {
		timer.Stop();
		Double_t rtime = timer.RealTime();
		Double_t ctime = timer.CpuTime();
		cout << "PndEmcFWEndcapDigi, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	}
}

void PndEmcFWEndcapDigi::SetParContainers() {

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get Emc digitisation parameter container
  fDigiPar = dynamic_cast<PndEmcFWEndcapDigiPar*>(db->getContainer("PndEmcFWEndcapDigiPar"));
  
  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");

  // Get Emc geometry parameter container
  fGeoPar = (PndEmcGeoPar*) db->getContainer("PndEmcGeoPar");
 
}

void PndEmcFWEndcapDigi::SetStorageOfData(Bool_t val)
{
  fStoreDigis = val;
  return;
}

ClassImp(PndEmcFWEndcapDigi);

