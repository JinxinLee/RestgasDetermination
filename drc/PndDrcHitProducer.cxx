// -------------------------------------------------------------------------
// -----                   PndDrcHitProducer source file               -----
// -----               Created 11/10/06  by Annalisa Cecchi            -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <fstream>
#include <iostream>
//#include "stdio.h"



#include "PndDrcHitProducer.h"
#include "CbmRootManager.h"
#include "CbmMCTrack.h"
#include "PndDrcPoint.h"
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



// -----   Default constructor   -------------------------------------------
//PndDrcHitProducer::PndDrcHitProducer() 
PndDrcHitProducer::PndDrcHitProducer() 
:CbmTask("DrcHitProducer")
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor with verbosity level  -------------------------------------------

PndDrcHitProducer::PndDrcHitProducer(Int_t verbose) 
  :CbmTask("DrcHitProducer")
{
  
  fVerbose = verbose;  
  c = 2.998E8;                // speed of light
  h = 6.626E-34;              // Plancks constant
  e = 1.6022E-19;             // Eulers constant
  //  nrefrac = 1.0002804;   // refraction index of 40%He+60%CH4
  nrefrac = 1.472;   // refraction index of quartz
  nevents = 0;
  fNHits  = 0;
  fNDoubleHits = 0;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
/** Constructor with name and title */
//PndDrcHitProducer::PndDrcHitProducer(const char *name, const char *title)
//  : CbmTask(name) {}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndDrcHitProducer::~PndDrcHitProducer()
{
  //  CbmRootManager *ioman =CbmRootManager::Instance();
  //  ioman->Write();
}
// -------------------------------------------------------------------------



// -----   Initialization of parameter Containers  ------------------------------------------------
void PndDrcHitProducer::SetParContainers() {

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
InitStatus PndDrcHitProducer::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;

  // Get RootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndDrcHitProducer::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("PndDrcPoint");
  //fPointArray = (TClonesArray*) ioman->ActivateBranch("DrcPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndDrcDigiProducer::Init: "
         << "No PndDrcPoint array!" << endl;
    return kERROR;
  }

  fListStack = (TClonesArray *)ioman->GetObject("MCTrack"); 
   if ( ! fListStack ) {
    cout << "-W- PndDrcDigiProducer::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }
 
   // Create and register output array
   fHitArray = new TClonesArray("PndDrcHit");
   ioman->Register("PndDrcHit","Drc",fHitArray, kTRUE);
    
  // Geometry loading
   TFile *drcfile = ioman->GetInFile();
   TGeoManager *geoMan = (TGeoManager*) drcfile->Get("CBMGeom");
   fVolumeArray = geoMan->GetListOfVolumes();
   //fVolumeArray = gGeoManager->GetListOfVolumes();
   cout << "-I- PndDrcHitProducer: Intialization successfull" << endl;

   return kSUCCESS;
   
}

/*
  //------------- example for getting more parameters from the data base: -------------------

  // 1) get and print medium
     CbmGeoMedium* med = det->getMedium();
     med->Dump();

  // 2) retrieve relevant parameter
  // Shape
     Int_t npoints = det->getNumPoints();
     Double_t para[npoints][3];
     TString shapeName = det->getShape();
     for (Int_t i=0;i<npoints;i++) {
         for (Int_t j=0;j<3; j++){
              para[i][j] = det->getVolParameter(i,j);
              cout << "i: " << i << "j: " << j << "par: " << para[i][j] << endl;
         }
     }

  // 3) Lab Transform
     CbmGeoTransform* transf =  det->getLabTransform();
     CbmGeoRotation rot = transf->getRotMatrix();
     CbmGeoVector trans = transf->getTransVector();

     Double_t rotp[9];
     cout << " Lab rotation : " << endl;
     for (Int_t i=0; i<9; i++) {
                                  rotp[i] = rot(i);
                                  cout << " i: " << i << " val: " << rotp[i] ;
     }
     cout << endl;
     cout << " Lab translation : " << endl;
     cout << " tX: " << trans.X() << " tY: " << trans.Y() << " tZ: " << trans.Z() << endl;
*/
  //------------------------------------------------------------------------------------------





// -----   Execution of Task   ---------------------------------------------
// -------------------------------------------------------------------------
void PndDrcHitProducer::Exec(Option_t* option)
{
  if ( ! fHitArray ) Fatal("Exec", "No HitArray");

  PndDrcPoint* pt=NULL;
  CbmMCTrack* track=NULL; // Stefano
  nevents++;
  
 
  if (fVerbose > 0) {
    cout << " ----------------- DRC Hit Producer --------------------" << endl;
  }
  if (fVerbose > 0) {
    cout <<" Number of input MC points: "<<fPointArray->GetEntries()<<endl;
  }

  fNHits = 0;
  fNDoubleHits = 0;


  /* // Stefano
  for(Int_t jj=0; jj<fListStack->GetEntriesFast(); jj++) {
    track = (CbmMCTrack*)fListStack->At(jj);
    cout << "MC->At("<<jj<< ") PID: " << track->GetPdgCode() << "\t MotherID: "  <<  track->GetMotherID()<<endl;
  }
  // end Stefano
  */


  Double_t fELoss  = 0.;
  
  // Loop over PndDrcPoints
  for(Int_t j=0; j<fPointArray->GetEntriesFast(); j++) {
    if (fVerbose > 0) printf("\n\n=====> Event no. %d\n", nevents); 
    if (fVerbose > 0) {
      cout <<" Number of input MC points: "<<fPointArray->GetEntries()<<endl;
    } 
    pt = (PndDrcPoint*)fPointArray->At(j);
    
    TVector3 fPosPoint;
    pt->Position(fPosPoint);   
    Double_t fXHit = fPosPoint.X();
    Double_t fYHit = fPosPoint.Y();
    Double_t fZHit = fPosPoint.Z();
    TVector3 fPosHit(fXHit,fYHit,fZHit);

    TVector3 fMomPoint;
    pt->Momentum(fMomPoint);   
    Double_t fMomXHit = fMomPoint.X();
    Double_t fMomYHit = fMomPoint.Y();
    Double_t fMomZHit = fMomPoint.Z();
    TVector3 fMomHit(fMomXHit,fMomYHit,fMomZHit);
    
    Int_t fCopyNo =1;
    Double_t fTime = 1.;
    Double_t fMass    = pt->GetMass();
    Int_t fNPhotons = 1;
    Int_t fEventID = 11;
    Int_t fTrackID = pt->GetTrackID();
    fELoss =0.;
    cout << "Event no: " <<  fEventID << endl;
    cout << "1.--->" << fELoss << "\n" << endl;
    fELoss += pt->GetEnergyLoss();
    cout << "2.--->" << fELoss << "\n" << endl;
    CbmMCTrack* tr = NULL;
    tr = (CbmMCTrack*)fListStack->At(pt->GetTrackID());
    Int_t fPdgCode = tr->GetPdgCode();
    //cout <<fListStack->GetEntriesFast() << endl;
    //cout << pt->GetTrackID() << endl;
    cout << fPdgCode << endl;
    Double_t fThetaC = 0.;

    AddHit(fTrackID, 
	   fCopyNo, 
	   fPosHit, 
	   fMomHit,
	   fTime,
	   fELoss,
	   fMass,
	   fPdgCode,
	   fNPhotons,
	   fEventID);
    

    // photon detected?
    // pt->Print();
    
  }
}

 // loop over input points
  
//for (Int_t iHit=0; iHit<fNHits; iHit++)
//((PndDrcHit*)fHitCollection->At(iHit))->Print();






// -----   Add Hit to HitCollection   --------------------------------------
PndDrcHit* PndDrcHitProducer::AddHit(Int_t trackID, 
				     Int_t copyNo, 
				     TVector3 posHit, 
				     TVector3 momHit, 
				     Double_t time, 
				     Double_t eLoss,
				     Double_t mass, 
				     Int_t pdgCode,
				     Int_t nPhotons,
				     Int_t eventID){
  TClonesArray& clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndDrcHit(trackID, 
				    copyNo, 
				    posHit, 
				    momHit, 
				    time, 
				    eLoss,
				    mass,
				    pdgCode,
				    nPhotons,
				    eventID);
}

  /* //  Bool_t hitMerged = kFALSE;
  PndDrcHit *hit;
  // Check if there was any hit in the same PMT
  for (Int_t iHit=0; iHit<fNHits; iHit++) {
    hit = (PndDrcHit*)fhitArray->At(iHit);
    // if (pmtID == hit->GetPmtID() && detID==hit->GetDetectorID()) {
    //   hit->SetNPhotons(hit->GetNPhotons()+1);
    //    hit->SetAmplitude(GetAmplitude()+ampl);
    hitMerged = kTRUE;
    fNDoubleHits++;
    break;
    //}
  }

  
  // If no hits found in this PMT, add a new one
  if (!hitMerged) {
    new((*fhitArray)[fNHits]) PndDrcHit();
    hit = (PndDrcHit*)fhitArray->At(fNHits);
    hit->SetPosition(posHit);
    // hit->SetPositionError(posHitErr);
    // hit->SetDetectorID(detID);
    //    hit->SetPmtID(pmtID);
    //    hit->SetNPhotons(1);
    // hit->SetAmplitude(GetAmplitude());
    //hit->SetRefIndex(index);
    fNHits++;
  }
  */

// -------------------------------------------------------------------------


// // -----   Finish Task   ---------------------------------------------------
// void PndDrcHitProducer::Finish()
// {
//   fHitArray->Clear();
// }
// // -------------------------------------------------------------------------


ClassImp(PndDrcHitProducer)
