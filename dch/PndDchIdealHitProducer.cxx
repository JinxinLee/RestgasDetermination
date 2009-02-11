/////////////////////////////////////////////////////////////
//  PndDchIdealHitProducer
//  Filler of PndDchHit
///////////////////////////////////////////////////////////////// 


#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"

#include "FairDetector.h"
#include "PndDchIdealHitProducer.h"
#include "PndDchHit.h"
#include "PndDchPoint.h"
#include "PndDchStructure.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
//#include "TVector3.h"
#include "TRandom.h"


// -----   Default constructor   -------------------------------------------
PndDchIdealHitProducer::PndDchIdealHitProducer() :
  FairTask("Ideal DCH hit Producer") { 
}

// -----   Destructor   ----------------------------------------------------
PndDchIdealHitProducer::~PndDchIdealHitProducer() {
  if(fHitArray){
    fHitArray->Delete();
    delete fHitArray;
  }
}


// -----   Public method Init   --------------------------------------------
InitStatus PndDchIdealHitProducer::Init() {
 
  std::cout << " INITIALIZATION OF Ideal Dch Hit Producer***************" << std::endl;
  
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if( !ioman ) {
    std::cout << "-E- PndDchIdealHitProducer::Init: "
	      << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  
  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("PndDchPoint");
  if( !fPointArray ) {
    std::cout << "-W- PndDchIdealHitProducer::Init: "
	      << "Array of PndDchPoints not found!" << std::endl;
    return kERROR;
  }
  
  // Create and register output array
  fHitArray = new TClonesArray("PndDchHit");
  
  ioman->Register("PndDchHit","Dch",fHitArray,kTRUE);
  
  // Geometry loading
  TFile *infile = ioman->GetInFile();
  TGeoManager *geoMan = (TGeoManager*) infile->Get("FAIRGeom");
  fDchStructure = PndDchStructure::Instance(geoMan);
  
  // Create histograms
  CreateHistos();
  Reset();
  
  std::cout << "-I- PndDchIdealHitProducer: Intialization successfull" << std::endl;
  return kSUCCESS;

}

// -----   Public method Exec   --------------------------------------------
void PndDchIdealHitProducer::Exec(Option_t* opt) {

	// Reset output array
	if( !fHitArray ) Fatal("Exec", "No hit array");

	fHitArray->Clear();

	// Loop over Points
	PndDchPoint* point  = 0;
	Int_t nPoints = fPointArray->GetEntriesFast();
	for(Int_t iPoint = 0; iPoint < nPoints; iPoint++){
		point  = (PndDchPoint*) fPointArray->At(iPoint);		
		AddHit(point, iPoint);
	}  // end of loop over Points
}

// -----   Private method AddHit   --------------------------------------------
PndDchHit* PndDchIdealHitProducer::AddHit(const PndDchPoint* point , Int_t idx){
  // Fills the PndDchHit array
  TClonesArray& clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();
  Int_t trackID = point->GetTrackID();
  Int_t detID = point->GetDetectorID();
  Int_t chamber = point->GetChamber();
  TVector3 pos(point->GetX(),point->GetY(),point->GetZ());
  Double_t res = 0.02; // cm, default for drift chambers
  if(fDchStructure->GetCellSize(detID)<0.8) //dummy GEM
    res = 0.005; //cm, defalut for GEMs
  TVector3 dpos(res,res,res);
  Smear(pos, dpos); //smear here  
  
  // Filling histograms
  fhDx->Fill(10.*(point->GetX()-pos.X()));
  fhDy->Fill(10.*(point->GetY()-pos.Y()));
  fhDz->Fill(10.*(point->GetZ()-pos.Z()));
  
  return new(clref[size]) PndDchHit(trackID, detID, pos, dpos , idx);
}

// -----   Private Method Smear  ----------------------------------------------
void PndDchIdealHitProducer::Smear(TVector3& pos, const TVector3 dpos)const {
  Double_t dX, dY;
  gRandom->Rannor(dX, dY);
  dX *= dpos.X();
  dY *= dpos.Y();
  pos.SetX(pos.X() + dX);
  pos.SetY(pos.Y() + dY);
  pos.SetZ( gRandom->Gaus(pos.Z(), dpos.Z()));
}


// -----   Private method CreateHistos   -----------------------------------
void PndDchIdealHitProducer::CreateHistos() {
	
	// Histogram list
	fHistoList = new TList();
	
	fhDx = new TH1F("fhDx","x_{point}-x_{hit} / mm", 40,-2.,2.);	
	fhDy = new TH1F("fhDy","y_{point}-y_{hit} / mm", 40,-2.,2.);
	fhDz = new TH1F("fhDz","z_{point}-z_{hit} / mm", 40,-2.,2.);
	
	fHistoList->Add(fhDx);
	fHistoList->Add(fhDy);
	fHistoList->Add(fhDz);
}


// -----   Private method Reset   ------------------------------------------
void PndDchIdealHitProducer::Reset() {
	
	TIter next(fHistoList);
		while(TH1* histo = ((TH1*)next())) histo->Reset();	
}


// -----   Private method Finish   -----------------------------------------
void PndDchIdealHitProducer::Finish() {

	TDirectory *current = gDirectory;
	TDirectory *hdir = current->mkdir("DchIdealHitProducer");
	hdir->cd();
	TIter next(fHistoList);
	while ( TH1* histo = ((TH1*)next()) ) histo->Write();
	current->cd();
}

ClassImp(PndDchIdealHitProducer)
