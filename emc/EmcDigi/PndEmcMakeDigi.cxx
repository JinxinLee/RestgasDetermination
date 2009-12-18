//----------------------------------------------------------------------
// File and Version Information:
//      $Id: //
// Description:
//      Class PndEmcMakeDigi. This class takes array of EmcHit's and produce
// an array of EmcDigis.
// It is convenient to study reconstruction algoritms without
// disturbance from digitization
//----------------------------------------------------------------------

#include "PndEmcMakeDigi.h"		

#include "PndEmcDigi.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"				
#include "PndEmcMapper.h"
#include "PndEmcStructure.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TRandom.h"
#include "TROOT.h"

#include <iostream>
//#include <map>

using std::cout;
using std::endl;
using std::fstream;

PndEmcMakeDigi::PndEmcMakeDigi(Bool_t storedigis)
{
	fDigiPosMethod="depth";// "surface" or "depth"
	fEmcDigiRescaleFactor=1.08;
	fStoreDigis=storedigis;
}

//--------------
// Destructor --
//--------------

PndEmcMakeDigi::~PndEmcMakeDigi()
{
}


InitStatus PndEmcMakeDigi::Init()
{
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman )
	{
		cout << "-E- PndEmcMakeDigi::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}
	
	// Get input array
	fHitArray = (TClonesArray*) ioman->GetObject("EmcHit");
	if ( ! fHitArray ) {
		cout << "-W- PndEmcMakeDigi::Init: "
		<< "No EmcHit array!" << endl;
		return kERROR;
	}
	
	// Create and register output array
	fDigiArray = new TClonesArray("PndEmcDigi");

	ioman->Register("EmcDigi","Emc",fDigiArray,fStoreDigis);
	
	fEmcDigiPositionDepth=fRecoPar->GetEmcDigiPositionDepth();
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
		cout << "-W- PndEmcMakeDigi::Init: "
		<< "Unknown digi position method!" << endl;
		return kERROR;
	}
	
	// The following parameters define if energy of hit will be copied to digi or it will be smeared
	fUseDigiEffectiveSmearing=fDigiPar->GetUseDigiEffectiveSmearing();
	fExcessNoiseFactor=fDigiPar->GetExcessNoiseFactor();
	fIncoherent_elec_noise_width_GeV=fDigiPar->GetIncoherent_elec_noise_width_GeV(); //GeV
	fDetectedPhotonsPerMeV=fDigiPar->GetDetectedPhotonsPerMeV();
	
	fThreshold=fDigiPar->GetEnergyDigiThreshold();
	fMapVersion=fDigiPar->GetMapperVersion();  
	PndEmcMapper::Instance(fMapVersion);
	PndEmcStructure::Instance();

	cout << "-I- PndEmcMakeDigi: Intialization successfull" << endl;
	
	return kSUCCESS;
}

void PndEmcMakeDigi::Exec(Option_t* opt)
{
	// Reset output array
	if ( ! fDigiArray ) Fatal("Exec", "No Digi Array");
	fDigiArray->Delete();
  
	// Variable declaration
	PndEmcHit* theHit = NULL;

	// Loop over PndEmcHits to add them to correspondent wavefoorms
	Int_t nHits = fHitArray->GetEntriesFast();
	
	cout<<"Hit array contains "<<nHits<< " hits"<<endl;
	for (Int_t iHit=0; iHit<nHits; iHit++) {
		theHit = (PndEmcHit*) fHitArray->At(iHit);
		int detId=theHit->GetDetectorID();
		Double_t energy=theHit->GetEnergy();
		
		//int module = detId/100000000;
		
		if (energy>fThreshold)
		{
			Int_t trackId=theHit->GetRefIndex();
			Double_t time=theHit->GetTime();
			
			// Smear hit energy as sigma/E=sqrt((a/sqrt(E))^2+(E_noise/E)^2)
			// i.e stochastic and noise term of energy resolution are taken into account
			if (fUseDigiEffectiveSmearing){
				Double_t a=sqrt(fExcessNoiseFactor/(fDetectedPhotonsPerMeV*1e3)); // 1e3 is conversion from MeV to GeV
				Double_t sigma_E=sqrt(pow(a/sqrt(energy),2)+pow(fIncoherent_elec_noise_width_GeV/energy,2));
				energy= gRandom->Gaus(energy,sigma_E*energy);
			}
			
			AddDigi(trackId,detId, energy, time,iHit); 
		}
	}

}

PndEmcDigi* PndEmcMakeDigi::AddDigi(Int_t trackID,Int_t detID, Float_t energy, Float_t time, Int_t hitIndex){
  TClonesArray& clref = *fDigiArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndEmcDigi(trackID, detID, energy, time, hitIndex);
}

void PndEmcMakeDigi::SetParContainers() {

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

void PndEmcMakeDigi::SetStorageOfData(Bool_t val)
{
  fStoreDigis=val;
  return;
}
  
ClassImp(PndEmcMakeDigi)
