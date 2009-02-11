/////////////////////////////////////////////////////////////
//
//  PndEmcHitProducer
//
//  Filler of PndEmcHit
//
//  Created 14/08/06  by S.Spataro
//
/////////////////////////////////////////////////////////////// 

#include "PndEmcHitProducer.h"

#include "PndEmcStructure.h"
#include "PndEmcMapper.h"		
#include "PndEmcHit.h"
#include "PndEmcPoint.h"
#include "PndEmcDigiPar.h"		

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TROOT.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TVector3.h"

//#include <string>

using std::cout;
using std::endl;
using std::map;

		
// -----   Default constructor   -------------------------------------------
PndEmcHitProducer::PndEmcHitProducer() :
  FairTask("Ideal EMC hit Producer") { 
  fStoreHits=kTRUE;
  //eneThr = 0.001; // Energy threshold for emc pad (Now taken from DB)
}
// -------------------------------------------------------------------------

PndEmcHitProducer::PndEmcHitProducer(Bool_t val) :
  FairTask("Ideal EMC hit Producer") { 
  fStoreHits=val;
  //eneThr = 0.001; // Energy threshold for emc pad (Now taken from DB)
}

// -----   Destructor   ----------------------------------------------------
PndEmcHitProducer::~PndEmcHitProducer() { delete fEmcStr;}
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus PndEmcHitProducer::Init() {
 
  cout << " INITIALIZATION *********************" << endl;
  
  //FairDetector::Initialize();
  //FairRun* sim = FairRun::Instance();
  //FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndEmcHitProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("EmcPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndEmcHitProducer::Init: "
	 << "No EmcPoint array!" << endl;
    return kERROR;
  }
  
  // Create and register output array
  fDigiArray = new TClonesArray("PndEmcHit");
  
  ioman->Register("EmcHit","Emc",fDigiArray,fStoreHits);
  
	// Geometry loading
// 	TFile *infile = ioman->GetInFile();
// 	TGeoManager *geoMan = (TGeoManager*) infile->Get("FAIRGeom");
//	TGeoManager *geoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
	
	fMapVersion=fDigiPar->GetMapperVersion();
	PndEmcMapper *map=PndEmcMapper::Instance(fMapVersion);
	fEmcStr=PndEmcStructure::Instance();
  
	emcX=fEmcStr->GetEmcX();
	emcY=fEmcStr->GetEmcY();
	emcZ=fEmcStr->GetEmcZ();;

  fEnergyThreshold =fDigiPar->GetEnergyHitThreshold();
  
  cout << "-I- PndEmcHitProducer: Intialization successfull" << endl;
  
  return kSUCCESS;

}

void PndEmcHitProducer::SetParContainers() {

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar*) db->getContainer("PndEmcDigiPar");
 
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndEmcHitProducer::Exec(Option_t* opt) {

  cout << " DIGI EXECUTION *********************" << endl;
  // Reset output array
  if ( ! fDigiArray ) Fatal("Exec", "No DigiArray");
  
  fDigiArray->Delete();
  
  // Declare some variables
  PndEmcPoint* point  = NULL;
  map<Int_t, Float_t> fTrackEnergy;
  map<Int_t, Float_t> fTrackTime;  //time of first point
  fTrackEnergy.clear();
  fTrackTime.clear();
  map<Int_t, Float_t>::const_iterator p;
  
  std::vector<PndEmcPoint*> fPointList;// to pass to EmcHit
  
  // Loop over EmcPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  
  Double_t point_time;
  for (Int_t iPoint=0; iPoint<nPoints; iPoint++)
  {
    point  = (PndEmcPoint*) fPointArray->At(iPoint);
    fTrackEnergy[point->GetDetectorID()] += point->GetEnergyLoss();
	 point_time=point ->GetTime();
	 if (point_time<fTrackTime[point->GetDetectorID()]) fTrackTime[point->GetDetectorID()] =point_time;
  }
  // Loop over EmcPoint
  
  // Loop to register EmcHit
  for(p=fTrackEnergy.begin(); p!=fTrackEnergy.end(); ++p) {
    if ((*p).second>fEnergyThreshold)
      AddHit(1, (*p).first, (*p).second, fTrackTime[(*p).first]); 
  }
  
}
// -------------------------------------------------------------------------

// -----   Private method AddDigi   --------------------------------------------
PndEmcHit* PndEmcHitProducer::AddHit(Int_t trackID,Int_t detID, Float_t energy, Float_t time){
  // It fills the PndEmcHit category
  //cout << "PndEmcHitProducer: track " << trackID << " evt " << eventID << " sec " << sec << " plane " << pla << " strip " << strip << "box " << box << " tube " << tub << endl;
  TClonesArray& clref = *fDigiArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndEmcHit(trackID, detID, energy, time, emcX[detID], emcY[detID], emcZ[detID]);
}
// ----

void PndEmcHitProducer::SetStorageOfData(Bool_t val)
{
  fStoreHits=val;
  return;
}

ClassImp(PndEmcHitProducer)
