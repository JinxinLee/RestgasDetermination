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
#include "PndEmcGeoPar.h"
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

PndEmcMakeDigi::PndEmcMakeDigi(Bool_t storedigis):
  fHitArray(new TClonesArray()), fDigiArray(new TClonesArray()), fThreshold(0), fDigiPosMethod(""), fEmcDigiRescaleFactor(0), fEmcDigiPositionDepthPWO(0), fEmcDigiPositionDepthShashlyk(0), fUseDigiEffectiveSmearing(0), fDetectedPhotonsPerMeV(0), fNPhotoElectronsPerMeVAPDBarrel(0), fNPhotoElectronsPerMeVAPDBWD(0), fNPhotoElectronsPerMeVVPT(0), fSensitiveAreaAPD(0), fSensitiveAreaVPT(0), fQuantumEfficiencyAPD(0), fQuantumEfficiencyVPT(0), fExcessNoiseFactorAPD(0), fExcessNoiseFactorVPT(0), fIncoherent_elec_noise_width_GeV_APD(0), fIncoherent_elec_noise_width_GeV_VPT(0), fMapVersion(0), fGeoPar(new PndEmcGeoPar()), fDigiPar(new PndEmcDigiPar()), fRecoPar(new PndEmcRecoPar()), fStoreDigis(storedigis)
{
	fDigiPosMethod="depth";// "surface" or "depth"
	fEmcDigiRescaleFactor=1.08;
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
	
	fEmcDigiPositionDepthPWO=fRecoPar->GetEmcDigiPositionDepthPWO();
	fEmcDigiPositionDepthShashlyk=fRecoPar->GetEmcDigiPositionDepthShashlyk();
	if (!fDigiPosMethod.compare("surface"))
	{
		PndEmcDigi::selectDigiPositionMethod( PndEmcDigi::surface, 1., 0. );
	}
	else if (!fDigiPosMethod.compare("depth"))
	{
	   PndEmcDigi::selectDigiPositionMethod( PndEmcDigi::depth, 
			 fEmcDigiPositionDepthPWO, fEmcDigiPositionDepthShashlyk, fEmcDigiRescaleFactor);
	}
	else 
	{
		cout << "-W- PndEmcMakeDigi::Init: "
		<< "Unknown digi position method!" << endl;
		return kERROR;
	}
	
	// The following parameters define if energy of hit will be copied to digi or it will be smeared
	fUseDigiEffectiveSmearing=fDigiPar->GetUseDigiEffectiveSmearing();
	fDetectedPhotonsPerMeV=fDigiPar->GetDetectedPhotonsPerMeV();
	fSensitiveAreaAPD=fDigiPar->GetSensitiveAreaAPD();
	fSensitiveAreaVPT=fDigiPar->GetSensitiveAreaVPT();
	fQuantumEfficiencyAPD=fDigiPar->GetQuantumEfficiencyAPD();
	fQuantumEfficiencyVPT=fDigiPar->GetQuantumEfficiencyVPT();
	fExcessNoiseFactorAPD=fDigiPar->GetExcessNoiseFactorAPD();
	fExcessNoiseFactorVPT=fDigiPar->GetExcessNoiseFactorVPT();
	fIncoherent_elec_noise_width_GeV_APD=fDigiPar->GetIncoherent_elec_noise_width_GeV_APD(); //GeV
	fIncoherent_elec_noise_width_GeV_VPT=fDigiPar->GetIncoherent_elec_noise_width_GeV_VPT(); //GeV
	
	fThreshold=fDigiPar->GetEnergyDigiThreshold();
	fGeoPar->InitEmcMapper();  
	PndEmcStructure::Instance();


	// Calculate number of photoelectrons for APD and VPT
	// The number fDetectedPhotonsPerMeV is the measured number of photoelectrons with PM covering the whole rear surface divided by quantum efficiency of PM (18%)
	// To estimate Number of photoelectrons in barrel the rare surface is taken equal for all the crystals 745 mm^2, which is average surface, hovewer it varies depending on the type of the crystal
	// For forward and backward endcap rear surface is equal 26x26=676 mm^2
	// Therefore the different number of photoelectrons are used with APD for barrel and backward endcap
	fNPhotoElectronsPerMeVAPDBarrel=fDetectedPhotonsPerMeV*fSensitiveAreaAPD/745.*fQuantumEfficiencyAPD;
	fNPhotoElectronsPerMeVAPDBWD=fDetectedPhotonsPerMeV*fSensitiveAreaAPD/676.*fQuantumEfficiencyAPD;
	fNPhotoElectronsPerMeVVPT=fDetectedPhotonsPerMeV*fSensitiveAreaVPT/676.*fQuantumEfficiencyVPT;

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
		
			Int_t trackId=theHit->GetRefIndex();
			Double_t time=theHit->GetTime();
			
			// Smear hit energy as sigma/E=sqrt((a/sqrt(E))^2+(E_noise/E)^2)
			// i.e stochastic and noise term of energy resolution are taken into account
			if (fUseDigiEffectiveSmearing==1){
				int module = theHit->GetModule();
				Double_t a,sigma_E;
				switch (module){
					case 1: // Barrel 
						a=sqrt(fExcessNoiseFactorAPD/(fNPhotoElectronsPerMeVAPDBarrel*1e3)); // 1e3 is conversion from MeV to GeV
						sigma_E=sqrt(pow(a/sqrt(energy),2)+pow(fIncoherent_elec_noise_width_GeV_APD/energy,2));
						break;
					case 2: // Barrel 
						a=sqrt(fExcessNoiseFactorAPD/(fNPhotoElectronsPerMeVAPDBarrel*1e3)); // 1e3 is conversion from MeV to GeV
						sigma_E=sqrt(pow(a/sqrt(energy),2)+pow(fIncoherent_elec_noise_width_GeV_APD/energy,2));
						break;
					case 7: // Proto60
						a=sqrt(fExcessNoiseFactorAPD/(fNPhotoElectronsPerMeVAPDBarrel*1e3)); // 1e3 is conversion from MeV to GeV
						sigma_E=sqrt(pow(a/sqrt(energy),2)+pow(fIncoherent_elec_noise_width_GeV_APD/energy,2));
						break;
					case 3: // FWD endcap 
						a=sqrt(fExcessNoiseFactorVPT/(fNPhotoElectronsPerMeVVPT*1e3)); // 1e3 is conversion from MeV to GeV
						sigma_E=sqrt(pow(a/sqrt(energy),2)+pow(fIncoherent_elec_noise_width_GeV_VPT/energy,2));
						break;
					case 4: // BWD endcap 
						a=sqrt(fExcessNoiseFactorAPD/(fNPhotoElectronsPerMeVAPDBWD*1e3)); // 1e3 is conversion from MeV to GeV
						sigma_E=sqrt(pow(a/sqrt(energy),2)+pow(fIncoherent_elec_noise_width_GeV_APD/energy,2));
						break;
					case 5: // shashlyk
						// sigma/E=5.6/E+2.4/sqrt(E)+1.3 (%)
						sigma_E=sqrt(pow(0.056/energy,2)+pow(0.024/sqrt(energy),2)+0.013);
						break;
					default:
						std::cout<<"PndEmcMakeDigi::Exec - Unknown module number in EMC digitization"<<std::endl;
						abort();
				}
				
				energy= gRandom->Gaus(energy,sigma_E*energy);
			}
			if(fUseDigiEffectiveSmearing ==2){
				Double_t nPhotons=(energy*1e3*fDetectedPhotonsPerMeV);
				energy= gRandom->Gaus(0,fIncoherent_elec_noise_width_GeV_APD)+(gRandom->PoissonD(nPhotons)/1.0e3)/fDetectedPhotonsPerMeV;
			}

		if (energy>fThreshold&& detId>0)
		{
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

  // Get Emc geometry parameter container
  fGeoPar = (PndEmcGeoPar*) db->getContainer("PndEmcGeoPar");
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
