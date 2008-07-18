// -------------------------------------------------------------------------
// -----             PndDchCylinderHitProducer source file             -----
// -----                Created 26/03/08  by A. Wronska                -----
// -------------------------------------------------------------------------


// Pnd includes
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmRun.h"
#include "CbmDetector.h"
#include "PndDchStructure.h"
#include "PndDchMapper.h"
#include "PndDchDrifter.h"
#include "PndDchCylinderHitProducer.h"
#include "PndDchCylinderHit.h"
#include "PndDchDigi.h"

// ROOT includes
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TMath.h"
#include "TRandom.h"

// C++ includes
#include <iostream>
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndDchCylinderHitProducer::PndDchCylinderHitProducer() :
  CbmTask("CylinderHit Producer for DCH") {
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndDchCylinderHitProducer::~PndDchCylinderHitProducer() {
  if(fCylHitArray){
    fCylHitArray->Delete();
    delete fCylHitArray;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndDchCylinderHitProducer::Init() {

  std::cout << " INITIALIZATION OF PndDchCylinderHitProducer***************" << std::endl;

  // Get RootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if( !ioman ) {
    std::cout << "-E- PndDchCylinderHitProducer::Init: "
	      << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  fDchMapper     = PndDchMapper::Instance();
  fDchDrifter    = PndDchDrifter::Instance("d2t_rtdb.dat",1.);

  // Get input array
  fDigiArray = (TClonesArray*) ioman->GetObject("PndDchDigi");
  if( !fDigiArray ) {
    std::cout << "-W- PndDchCylinderHitProducer::Init: "
	      << "Array of PndDchDigis not found!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  fCylHitArray = new TClonesArray("PndDchCylinderHit");

  ioman->Register("PndDchCylinderHit","Dch",fCylHitArray,kTRUE);

  // Geometry loading
  TFile *infile = ioman->GetInFile();
  TGeoManager *geoMan = (TGeoManager*) infile->Get("CBMGeom");
  fDchStructure = PndDchStructure::Instance(geoMan);

  std::cout << "-I- PndDchCylinderHitProducer: Intialization successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndDchCylinderHitProducer::Exec(Option_t* opt) {

	// Reset output array
	if( !fCylHitArray ) Fatal("Exec", "No PndDchCylinderHit array");
	fCylHitArray->Clear();

	// Loop over Digis
	PndDchDigi* digi  = 0;
	Double_t xLoc, zGlo, dist, distErr, alpha, drTime, cellSize;
	Int_t nDigis = fDigiArray->GetEntriesFast();
	Int_t wi, ch, pl, detID;

	for(Int_t iDigi = 0; iDigi < nDigis; iDigi++){
		digi  = (PndDchDigi*) fDigiArray->At(iDigi);
		xLoc = zGlo = dist = distErr = alpha = drTime = cellSize = 0.0;
		wi = ch = pl = detID = 0;
		wi = digi->GetWire();
		ch = digi->GetChamber();
		pl = digi->GetPlane();
		drTime = digi->GetDriftTime();

		detID = fDchMapper->CalculateDetectorID(ch,pl);
		cellSize = fDchStructure->GetCellSize(detID);
		fDchDrifter->CalculateDistance(drTime,dist);
		dist *= cellSize;
		distErr = (ch==1? 0.005 : 0.02); //hard-coded exp. values
		dist += gRandom->Gaus(0.0, distErr); //smear distance

		alpha = fDchStructure->GetTransMatrix(detID)->GetRotation()->GetPhiRotation();
		alpha *= TMath::DegToRad();

		const Double_t* translation = fDchStructure->GetTransMatrix(detID)->GetTranslation();
		zGlo = translation[2];

		xLoc = fDchMapper->WirePosXlocal(wi,detID);
		if(fVerbose>1){
		  digi->Print("");
		  cout<<"alpha = "<<alpha*TMath::RadToDeg()<<
		    " deg, zGlo = "<<zGlo<<
		    " cm, xLoc = "<<xLoc<<
		    " cm, dist = "<<dist<<
		    " plus/minus "<<distErr<<
		    " cm"<<endl;
		}

		AddCylinderHit(iDigi, xLoc, zGlo, dist, distErr, alpha);
	}  // end of loop over Digis
}
// -------------------------------------------------------------------------


// -----   Private method AddHit   --------------------------------------------
PndDchCylinderHit* PndDchCylinderHitProducer::AddCylinderHit(Int_t digiidx, Double_t xLoc, Double_t zGlo,
							     Double_t dist, Double_t distErr,
							     Double_t alpha) const {
  TClonesArray& clref = *fCylHitArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndDchCylinderHit(digiidx, xLoc, zGlo,
					    dist, distErr, alpha);
}
// -------------------------------------------------------------------------


ClassImp(PndDchCylinderHitProducer)
