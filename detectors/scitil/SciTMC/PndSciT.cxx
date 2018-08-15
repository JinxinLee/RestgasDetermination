/////////////////////////////////////////////////////////////
//
//  PndSciT
//
//  created by A. Sanchez
//  modified by D. Steinschaden
//  last update  06.2015
///////////////////////////////////////////////////////////////

#include "PndSciT.h"
#include "PndSciTPoint.h"
//#include "PndGeoSciT.h"
#include "PndStack.h"
#include "PndDetectorList.h"

#include "FairGeoTransform.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoMedium.h"
#include "FairGeoInterface.h"
#include "FairGeoMedia.h"
//#include "FairGeoG3Builder.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TString.h"
#include "TList.h"
#include "TGeoBBox.h"
#include "TGeoMCGeometry.h"
#include "TObjArray.h"
#include "TGeoVoxelFinder.h"
#include "TGeoMatrix.h"

#include <string>
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;
using std::ostringstream;

// -----   Default constructor   -------------------------------------------
PndSciT::PndSciT()
 : FairDetector(), fSciTCollection(0)
 {
  fSciTCollection        = new TClonesArray("PndSciTPoint");
  fVerboseLevel = 0;
  fThreshold = 0.0001; // Standard Threshold set to 100 keV
  fGeoH = NULL;
  fGeoH = PndGeoHandling::Instance(); 

  // Volumes containing "SENSOR" in the name will be processed in the process hit funktion 
  fListOfSensitives.push_back("SENSOR");

}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndSciT::PndSciT(const char* name, Bool_t active)
  : FairDetector(name, active), fSciTCollection(0)
{
    fSciTCollection        = new TClonesArray("PndSciTPoint");
    fVerboseLevel = 0;
    fThreshold = 0.0001; // Standard Threshold set to 100 keV

    fGeoH = NULL;
    std::cout << "ScitTil fGeoH is loading" << std::endl;
    fGeoH = PndGeoHandling::Instance(); 
  if ( fGeoH == NULL ){
    std::cout << "ScitTil fGeoH was loaded but is still NULL" << std::endl;
  }

    // Volumes containing "SENSOR" in the name will be processed in the process hit funktion
    fListOfSensitives.push_back("SENSOR");
    
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndSciT::~PndSciT() {
  if (fSciTCollection) {
    fSciTCollection->Delete();
    delete fSciTCollection;
  }

}
// -------------------------------------------------------------------------



// -----   Public method Intialize   ---------------------------------------
void PndSciT::Initialize() {

  std::cout<<" -I- Initializing PndSciT()"<<std::endl;
  FairDetector::Initialize();
  std::cout<<" Fairdetector geht"<<std::endl;
  // not mandatory ,but may someone can make use out of the stored parameters
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  par=(PndGeoSciTPar*)(rtdb->getContainer("PndGeoSciTPar"));
  par->setChanged();
  par->setInputVersion(sim->GetRunId(),1);
  //-----------------------------------------------------------------

  if(0==gGeoManager) {
    std::cout<<" -E- No gGeoManager in PndSciT::Initialize()!"<<std::endl;
    abort();
  }
  
   fGeoH->CreateUniqueSensorId("", fListOfSensitives);
  if(fVerboseLevel>0) fGeoH->PrintSensorNames();
 
  std::cout<<" -I- Initialized PndSciT()"<<std::endl;
}
// -------------------------------------------------------------------------
void PndSciT::BeginEvent(){
  // Begin of the event
  
}

// -----   Public method ProcessHits  --------------------------------------

Bool_t PndSciT::ProcessHits(FairVolume* )  // vol //[R.K.03/2017] unused variable(s)
{

    if(0==fGeoH) {
      std::cout<<" -E- No PndGeoHandling loaded."<<std::endl;
      abort();
    }

  // Set parameters at entrance of volume. Reset ELoss.
    if ( gMC->IsTrackEntering() ) 
      {
	fELoss  = 0.;
	fEventID = gMC->CurrentEvent();
	fTime   = gMC->TrackTime() * 1.0e09;
	fLength = gMC->TrackLength();	
	gMC->TrackPosition(fPosIn);
	gMC->TrackMomentum(fMomIn);
      }

    // Sum energy loss for all steps in the active volume 
    fELoss += gMC->Edep();

   // Set additional parameters at exit of active volume. 
   // And create the PndSciTPoint.	 
    if ( (gMC->IsTrackExiting()    ||
	  gMC->IsTrackStop()       ||
	  gMC->IsTrackDisappeared() ))
      {
	fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();

	fdetPath = gMC->CurrentVolPath();
	fSensorID = fGeoH->GetShortID(gMC->CurrentVolPath());

	//fSensorID = vol->getCopyNo();
	
	gMC->TrackPosition(fPosOut);
	gMC->TrackMomentum(fMomOut);
	 
	//Cut on energy loss to reduce stored data Elos < 100 keV
	if (fELoss < fThreshold) return kFALSE;
	 
	AddHit(fEventID, fTrackID, fSensorID, fdetPath,
		TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
		TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
		TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
		TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
		fTime, fLength,fELoss);
	 
	 PndStack* stack = (PndStack*) gMC->GetStack();
	 stack->AddPoint(kTOF);
	 
	 ResetParameters();
      }
 
  return kTRUE;
   
}//ProcessHits

// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void PndSciT::EndOfEvent() {
  if (fVerboseLevel)  Print();
  
  Reset();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void PndSciT::Register() {
  FairRootManager::Instance()->Register("SciTPoint","PndSciT", fSciTCollection, kTRUE);

  
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* PndSciT::GetCollection(Int_t iColl) const {
   if (iColl == 0) 
     return fSciTCollection;
   else
     return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndSciT::Print() const {
    Int_t nHits = fSciTCollection->GetEntriesFast();
    cout << "-I- PndSciT: " << nHits << " points registered in this event."
 	<< endl;

    if (fVerboseLevel>1)
      for (Int_t i=0; i<nHits; i++) (*fSciTCollection)[i]->Print();
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void PndSciT::Reset() {
   fSciTCollection->Clear();

 
  fPosIndex = 0;
}
// ----------------------------------------------------------------------------



// -----   Public method CopyClones   -----------------------------------------
void PndSciT::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
  Int_t nEntries = cl1->GetEntriesFast();
  //cout << "-I- PndSciT: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  PndSciTPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (PndSciTPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndSciTPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- PndSciT: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}
// ----------------------------------------------------------------------------
 // -----   Public method ConstructGeometry   ----------------------------------
void PndSciT::ConstructGeometry() {

 TString fileName=GetGeometryFileName();

  if(fileName.EndsWith(".root")){
    ConstructRootGeometry();
  }else {
	cout<<" geometry format not supported"<<endl;}	
 
}

  
// -------------------------------------------------------------------------
bool PndSciT::CheckIfSensitive(std::string name)
{
  for (size_t i = 0; i < fListOfSensitives.size(); i++){
    if (name.find(fListOfSensitives[i]) != std::string::npos)
    return true;
  }
  return false;
}


// -----   Private method AddHit   --------------------------------------------

PndSciTPoint* PndSciT::AddHit(Int_t eventID, Int_t trackID, Int_t sensorID, TString detName,
			    TVector3 pos, TVector3 mom,
			    TVector3 posout, 
			    TVector3 momout,
			   Double_t time,
			    Double_t length, 
			    Double_t eLoss) {
  TClonesArray& clref = *fSciTCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndSciTPoint( eventID, trackID, sensorID, detName,pos, mom, 
				      posout, momout,
				      time, length, eLoss);
 }

// --------


ClassImp(PndSciT)
