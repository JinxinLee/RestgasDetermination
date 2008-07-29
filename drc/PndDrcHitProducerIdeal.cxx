// -------------------------------------------------------------------------
// -----                   PndDrcHitProducerIdeal source file               -----
// -----               Created 11/10/06  by Annalisa Cecchi            -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include "stdio.h"

#include "PndDrcHitProducerIdeal.h"
#include "CbmRootManager.h"
#include "CbmMCTrack.h"
#include "PndDrcBarPoint.h"
#include "PndDrcHit.h"
#include "TVector3.h"
#include "TRandom.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmBaseParSet.h"
#include "CbmGeoVolume.h"
#include "TString.h"
#include "CbmGeoTransform.h"
#include "CbmGeoVector.h"
#include "CbmGeoMedium.h"
#include "CbmGeoNode.h"
#include "PndGeoDrcPar.h"
#include "TFormula.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TPDGCode.h"
#include "TGeoManager.h"

using std::endl;
using std::cout;

// -----   Default constructor   -------------------------------------------
//PndDrcHitProducerIdeal::PndDrcHitProducerIdeal() 
PndDrcHitProducerIdeal::PndDrcHitProducerIdeal() 
:CbmTask("PndDrcHitProducerIdeal")
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor with verbosity level  -------------------------------------------

PndDrcHitProducerIdeal::PndDrcHitProducerIdeal(Int_t verbose) 
  :CbmTask("PndDrcHitProducerIdeal")
{
   fVerbose = verbose;  
 }
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndDrcHitProducerIdeal::~PndDrcHitProducerIdeal()
{
}
// -------------------------------------------------------------------------



// -----   Initialization of parameter Containers  ------------------------------------------------
void PndDrcHitProducerIdeal::SetParContainers() {

  // Get run and runtime database
  CbmRunAna* run = CbmRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  CbmRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get Drc digitisation parameter container
  fPar = (PndGeoDrcPar*) db->getContainer("PndGeoDrcPar");  
}
// -------------------------------------------------------------------------


// -----   Initialization   -----------------------------------------------
// -------------------------------------------------------------------------
InitStatus PndDrcHitProducerIdeal::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;

  // Get RootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndDrcHitProducerIdeal::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fBarPointArray = (TClonesArray*) ioman->GetObject("DrcBarPoint");
   if ( ! fBarPointArray ) {
    cout << "-W- PndDrcHitProducerIdeal::Init: "
         << "No DrcBarPoint array!" << endl;
    return kERROR;
  }

  fListStack = (TClonesArray *)ioman->GetObject("MCTrack"); 
   if ( ! fListStack ) {
    cout << "-W- PndDrcHitProducerIdeal::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }
 
   // Create and register output array
   fHitArray = new TClonesArray("PndDrcHit");
   ioman->Register("DrcHit","Drc",fHitArray, kTRUE);
    
  // Geometry loading
   TFile *drcfile = ioman->GetInFile();
   TGeoManager *geoMan = (TGeoManager*) drcfile->Get("CBMGeom");
   fVolumeArray = geoMan->GetListOfVolumes();
   //fVolumeArray = gGeoManager->GetListOfVolumes();
   cout << "-I- PndDrcHitProducerIdeal: Intialization successfull" << endl;

   return kSUCCESS;
   
}


// -----   Execution of Task   ---------------------------------------------
// -------------------------------------------------------------------------
void PndDrcHitProducerIdeal::Exec(Option_t* option)
{
  if ( ! fHitArray ) Fatal("Exec", "No HitArray");
  //  fHitArray->Clear();

  PndDrcBarPoint* pt=NULL;
  CbmMCTrack* track=NULL; // Stefano
  nevents++;
   
  if (fVerbose > 0) {
    cout << " ----------------- DRC Hit Producer --------------------" << endl;
  }
  if (fVerbose > 0) {
    cout <<" Number of input MC points in the bar: "<<fBarPointArray->GetEntries()<<endl;
  }

  // fNHits = 0;

  // Loop over PndDrcPoints
  for(Int_t j=0; j<fBarPointArray->GetEntriesFast(); j++) {
    if (fVerbose > 0) printf("\n\n=====> Event No. %d\n", nevents); 
    
    pt = (PndDrcBarPoint*)fBarPointArray->At(j);
    
    fDetectorID = pt->GetNBar();
   
    TVector3 fPosPoint;
    pt->Position(fPosPoint);   
    Double_t fXHit = fPosPoint.X();
    Double_t fYHit = fPosPoint.Y();
    Double_t fZHit = fPosPoint.Z();
    TVector3 fPosHit(fXHit,fYHit,fZHit);

    Double_t fDPosXHit = 0.5; //mm
    Double_t fDPosYHit = 0.5;
    Double_t fDPosZHit = 0.5;
    TVector3 fDPosHit(fDPosXHit,fDPosYHit,fDPosZHit);
    
    Double_t fThetaC = pt->GetThetaC();
    Double_t fErrThetaC = 0.008; //rad

    Int_t fRefIndex = j;

    CbmMCTrack* tr = NULL;
    tr = (CbmMCTrack*)fListStack->At(pt->GetTrackID());
  
    AddHit(fDetectorID, 
	   fPosHit, 
	   fDPosHit,
	   fThetaC,
	   fErrThetaC,
	   fRefIndex);
    
  }
}




// -----   Add Hit to HitCollection   --------------------------------------
PndDrcHit* PndDrcHitProducerIdeal::AddHit(Int_t detID, 
				     TVector3 posHit, 
				     TVector3 dPosHit, 
				     Double_t thetaC,
				     Double_t errThetaC,
				     Int_t index){
  TClonesArray& clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndDrcHit(detID, 
				    posHit, 
				    dPosHit, 
				    thetaC,
				    errThetaC,
				    index);
}

// -------------------------------------------------------------------------


// -----   Finish Task   ---------------------------------------------------
void PndDrcHitProducerIdeal::Finish()
{
   fHitArray->Clear();
 }
// -------------------------------------------------------------------------


ClassImp(PndDrcHitProducerIdeal)
