// -------------------------------------------------------------------------
// -----                PndMuoDigiProducer source file                 -----
// -----                  Created 10/01/06  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndMuoDigiProducer.h"

#include "PndMuoDigi.h"
#include "PndMuoPoint.h"

#include "CbmRootManager.h"
#include "CbmDetector.h"
#include "CbmRun.h"
#include "CbmRuntimeDb.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TVector3.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMuoDigiProducer::PndMuoDigiProducer() :
  CbmTask("Ideal MUO Digi Producer") { 
  //nSec = -1;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMuoDigiProducer::~PndMuoDigiProducer() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndMuoDigiProducer::Init() {
 
  cout << " INITIALIZATION *********************" << endl;
  
  //CbmDetector::Initialize();
  CbmRun* sim = CbmRun::Instance();
  CbmRuntimeDb* rtdb=sim->GetRuntimeDb();
  // Load of geometry params
  par=(PndGeoMuoPar*)(rtdb->getContainer("PndGeoMuoPar"));
  
  // Get RootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndMuoDigiProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("MuoPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndMuoDigiProducer::Init: "
	 << "No MuoPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fDigiArray = new TClonesArray("PndMuoDigi");
 
  ioman->Register("MuoDigi","Muo",fDigiArray,kTRUE);
  
  // Geometry loading
  //TGeoManager *geoMan = (TGeoManager*) ioman->GetObject("CBMGeom");
  // TFile tstfile("$VMCWORKDIR/macro/run/testandrea.root");
  // TGeoManager *geoMan = (TGeoManager*) tstfile.Get("CBMGeom");
  // fVolumeArray = geoMan->GetListOfVolumes();

  cout << "-I- PndMuoDigiProducer: Intialization successfull" << endl;
  
  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndMuoDigiProducer::Exec(Option_t* opt) {
 
  //cout << "Event Number "<<endl;
  /*
  Int_t evtn=0;	
  PndMuoPoint *ptemp =(PndMuoPoint*) fPointArray->At(0);
   
if(ptemp !=NULL) {
    evtn=ptemp->GetEventID();
    if(evtn%50==0)cout << "Event Number "<<evtn<<endl;
  }
  */
  //cout << " DIGI EXECUTION *********************" << endl;

  // Reset output array
  if ( ! fDigiArray ) Fatal("Exec", "No DigiArray");
  
  fDigiArray->Clear();
  
  // Declare some variables
  PndMuoPoint* point  = NULL;
  TVector3 pmean;
  Bool_t muon_hit[8][3][20][8][400];
  for (Int_t mm=0;mm<8;mm++)  for (Int_t pp=0;pp<3;pp++) for (Int_t bb=0;bb<20;bb++) for (Int_t tt=0;tt<8;tt++) for (Int_t ss=0;ss<400;ss++) muon_hit[mm][pp][bb][tt][ss] = kFALSE;
  // Loop over MuoPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  
  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {
    point  = (PndMuoPoint*) fPointArray->At(iPoint);
    if (point->GetFlag()==1) // stores only all the outgoing particles
     // if (muon_hit[point->GetSector()][point->GetPlane()][point->GetBox()][point->GetTube()][point->GetStrip()] == kFALSE) { 
	AddDigi(point->GetTrackID(),point->GetEventID(), point->GetSector(), point->GetPlane(),  point->GetStrip(),  point->GetBox(),  point->GetTube());
//	muon_hit[point->GetSector()][point->GetPlane()][point->GetBox()][point->GetTube()][point->GetStrip()]= kTRUE;
 //     }
  } // Loop over MCPoints
  
}
// -------------------------------------------------------------------------


// -----   Private method AddDigi   --------------------------------------------
PndMuoDigi* PndMuoDigiProducer::AddDigi(Int_t trackID,Int_t eventID, Short_t sec, Short_t pla, Short_t strip, Short_t box, Short_t tub){
  // It fills the PndMuoDigi category
  // cout << "PndMuoDigiProducer: track " << trackID << " evt " << eventID << " sec " << sec << " plane " << pla << " strip " << strip << "box " << box << " tube " << tub << endl;
  TClonesArray& clref = *fDigiArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndMuoDigi(trackID, eventID, sec, pla, strip, box, tub);
}
// ----


ClassImp(PndMuoDigiProducer)
