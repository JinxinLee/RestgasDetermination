
/////////////////////////////////////////////////////////////
// RpcBarrelTofDetector
//
// Class for RpcBarrelTofDetector
//
/////////////////////////////////////////////////////////////

#include <iostream>
#include<fstream>
using std::cout;
using std::endl;
using std::ofstream;

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "FairMCEventHeader.h"
#include "PndDetectorList.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "PndStack.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "PndRpcGeo.h"
#include "PndRpcDetector.h"	
#include "PndRpcPoint.h"
#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"
#include "PndRpcGeoPar.h"
#include "TObjArray.h"
#include "FairRun.h"


// -----   Default constructor   -------------------------------------------
PndRpcDetector::PndRpcDetector()  {
  fRpcPointCollection = new TClonesArray("PndRpcPoint");
  fPosIndex   = 0;
//  fDetectorID = 0;
  fVerboseLevel = 2;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndRpcDetector::PndRpcDetector(const char* name, Bool_t active)
  : FairDetector(name, active) {
    fRpcPointCollection  = new TClonesArray("PndRpcPoint");
    fPosIndex   = 0;
//    fDetectorID = 0;
    fVerboseLevel = 2;

}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndRpcDetector::~PndRpcDetector() {
  if (fRpcPointCollection) {
    fRpcPointCollection->Delete();
    delete fRpcPointCollection;
  }
  
}
// -------------------------------------------------------------------------

// -----   Public method Intialize   
// ---------------------------------------
void PndRpcDetector::Initialize() {

  FairDetector::Initialize();
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  PndRpcGeoPar *par=(PndRpcGeoPar*)(rtdb->getContainer("PndRpcGeoPar"));

   TObjArray *fSensNodes = par->GetGeoSensitiveNodes();

}
//--------------------------------------------------------

/*
void RpcDetector::BeginEvent(){
  //cout<<">>>>>>>>>>>>>>>>>>>>inizio evento "<<endl;
  //fevent++;
}
*/

// -----   Public method ProcessHits  --------------------------------------

Bool_t PndRpcDetector::ProcessHits(FairVolume* vol) {

/*  FairMCEventHeader EvHead; 
//  Double_t time_ev = EvHead.GetT();
//  char c;
//  cin>>c;
 cout << time_ev;   */

  Int_t gap, cell, module, region;
//aida  TString Volname;
 // Set parameters at entrance of volume. Reset ELoss.
  if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPos);
    gMC->TrackMomentum(fMom);        // GeV
    fEventID = gMC->CurrentEvent();
 //  fmass   = gMC->TrackMass();     // mass (GeV)
 //  fcharge = gMC->TrackCharge();   // charge

    Int_t fpdgCode = gMC->TrackPid();


}
 // Sum energy loss for all steps in the active volume



  fELoss += gMC->Edep();
   
// Set additional parameters at exit of active volume. 
  if  ( (gMC->TrackCharge()!=0) 
   &&
      ( gMC->IsTrackExiting() )    ||
      ( gMC->IsTrackStop() )       ||
      ( gMC->IsTrackDisappeared() )  ){
 
 fTrackID  = gMC->GetStack()->GetCurrentTrackNumber(); // trk ID
 TString Volname = gMC->CurrentVolName();
//  Volname = vol->getName();  
    region = Volname[5]-'0';
 
    gMC->CurrentVolID(gap);
    gMC->CurrentVolOffID(1, cell);
    gMC->CurrentVolOffID(2, module);

 fVolumeID = ((region-1)<<24) + ((module-1)<<14)+((cell-1)<<4)+(gap-1);

    TString cvp = gMC->CurrentVolPath();

  Double_t xlab[3]={fPos.X(),  fPos.Y(),  fPos.Z()};
  Double_t xlab_sm[3];

// xcel - local koordinates, xcel_sm - smeared  local coordinates
  Double_t xcel[3], xcel_sm[3];
  gMC->Gmtod(xlab,xcel,1);
    
   cout<<xcel[0] << " " << xcel[1] << " " << xcel[2]<<" "<<cvp<<endl;
    
  Double_t Velosity = 19.047;
  Double_t length_cell=29.8;
  Double_t thickness = 1.0;
  Double_t width = 2.9;

  Double_t ft1 = fTime + (length_cell/2.0 - xcel[0])/Velosity;
  Double_t ft2 = fTime + (length_cell/2.0 + xcel[0])/Velosity;

// Smear ft1 and ft2 at 100 ps
 Double_t ft1_sm=gRandom->Gaus(ft1,0.1);
 Double_t ft2_sm=gRandom->Gaus(ft2,0.1);

// calculation of smeared coordinates
  xcel_sm[0]=(ft2_sm-ft1_sm)/2.0 * Velosity;
  xcel_sm[1]=0.0;    // "y" is choosen in the center of local system
  xcel_sm[2]=0.0;    // "z" is chosen in the center of  local system

// transfer from local smeared coordinates to global ones	
  gMC->Gdtom(xcel_sm,xlab_sm,1);

  TVector3 fPos_sm(xlab_sm[0],xlab_sm[1],xlab_sm[2]);

 
    AddHit(fTrackID, fVolumeID, TVector3(fPos.X(),  fPos.Y(),  fPos.Z()),
           TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength,
           fELoss, fEventID, ft1, ft2, TVector3(fPos_sm.X(),  fPos_sm.Y(),  
           fPos_sm.Z()));

  
cout << "PARAMETERS ----------" << endl;
cout << "trk " << fTrackID << " vol " << fVolumeID <<endl;
cout << " pos  " << fPos.X() << " " << fPos.Y() << " " << fPos.Z() << endl;
cout << " time " << fTime << " length " << fLength <<"Eloss "<< fELoss << endl;
cout << "t1 = " << ft1 << " t2= " << ft2<<endl;  

    // Increment number of tof points for TParticle
     PndStack* stack = (PndStack*) gMC->GetStack();
     stack->AddPoint(kTOF);
     

  
  // TO BE SET AS DEBUG 

 ResetParameters();
}
 return kTRUE;
  
}
// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void PndRpcDetector::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}

// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void PndRpcDetector::Register() {
FairRootManager::Instance()->Register("PndRpcPoint","Rpc", fRpcPointCollection, kTRUE);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* PndRpcDetector::GetCollection(Int_t iColl) const {
   if (iColl == 0) return fRpcPointCollection;

  return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndRpcDetector::Print() const {
    Int_t nHits = fRpcPointCollection->GetEntriesFast();
cout << "-I- RpcDetector: " << nHits << " points registered in this event."
 	<< endl;

    if (fVerboseLevel>1)
      for (Int_t i=0; i<nHits; i++) (*fRpcPointCollection)[i]->Print();
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void PndRpcDetector::Reset() {
  fRpcPointCollection->Clear();  
  fPosIndex = 0;
}
// ----------------------------------------------------------------------------

// -----   Public method CopyClones   -----------------------------------------
 void PndRpcDetector::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset )
 {
   Int_t nEntries = cl1->GetEntriesFast();
   cout << "-I- RpcTofDetector: " << nEntries << " entries to add." << endl;
   TClonesArray& clref = *cl2;
   PndRpcPoint* oldpoint = NULL;
   for (Int_t i=0; i<nEntries; i++) {
     oldpoint = (PndRpcPoint*) cl1->At(i);
     Int_t index = oldpoint->GetTrackID() + offset;
     oldpoint->SetTrackID(index);
     new (clref[fPosIndex]) PndRpcPoint(*oldpoint);
     fPosIndex++;
   }
cout << " -I- RpcTofDetector: " << cl2->GetEntriesFast() << " merged entries."
        << endl;
 }
// ----------------------------------------------------------------------------

// -----   Public method ConstructGeometry   ----------------------------------
void PndRpcDetector::ConstructGeometry() {
 Int_t count=0;
  Int_t count_tot=0;

  std::cout<<" --- Building RpcTof Geometry ---"<<std::endl;
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoIFace = geoLoad->getGeoInterface();
  PndRpcGeo*      rpcGeo = new PndRpcGeo();
  rpcGeo->setGeomFile(GetGeometryFileName());
  std::cout<<"Geometry filename = "<<GetGeometryFileName()<<std::endl;
  geoIFace->addGeoModule(rpcGeo);
  
  Bool_t rc = geoIFace->readSet(rpcGeo);
  if (rc) rpcGeo->create(geoLoad->getGeoBuilder());
  else std::cerr<<"RpcTofDetector:: geometry could not be read!"<<std::endl;

  TList* volList = rpcGeo->getListOfVolumes();
  std::cout<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%"<<std::endl;
  std::cout<<"volList contains "<<volList->GetEntries()<<" volumes"<<std::endl;

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  PndRpcGeoPar* par=(PndRpcGeoPar*)(rtdb->getContainer("PndRpcGeoPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume *aVol=NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
    std::cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  "<< node->GetName()<<std::endl;
    aVol = dynamic_cast<FairGeoVolume*> ( node );
    if ( node->isSensitive()  ) {
      fSensNodes->AddLast( aVol );
      count++;
    }else{
      fPassNodes->AddLast( aVol );
    }
  count_tot++;
  }

  ProcessNodes( volList );
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);

  // set active/inactive
  std::cout<<"koniec"<<std::endl;

}
// ----------------------------------------------------------------------------

// -----   Private method AddHit   --------------------------------------------
// see PndRpcPoint for hit description
PndRpcPoint* PndRpcDetector::AddHit(Int_t trackID, Int_t detID,TVector3 pos, 
TVector3 mom, Double_t time, Double_t length, Double_t eLoss, 
Int_t eventID, Double_t t1, Double_t t2,TVector3 pos_sm )
			   {
  TClonesArray& clref = *fRpcPointCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1) 
  return new(clref[size]) PndRpcPoint(trackID, detID, pos,
  mom, time, length, eLoss, eventID, t1, t2, pos_sm);
}
// ----------------------------------------------------------------------------

ClassImp(PndRpcDetector)
