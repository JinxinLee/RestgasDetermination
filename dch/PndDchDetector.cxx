/////////////////////////////////////////////////////////////
// PndDchDetector
//
// Class for PndDchDetector
//
/////////////////////////////////////////////////////////////

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TObjArray.h"
#include "TString.h"
#include "TGeoVoxelFinder.h"
#include "TGeoMatrix.h"

#include "CbmGeoInterface.h"
#include "CbmGeoLoader.h"
#include "CbmGeoNode.h"
#include "CbmGeoRootBuilder.h"
#include "CbmStack.h"
#include "CbmRootManager.h"
#include "CbmVolume.h"
#include "CbmGeoG3Builder.h"
#include "CbmRuntimeDb.h"
#include "CbmRun.h"
#include "CbmGeoMedia.h"

#include "PndDchGeo.h"
#include "PndDchDetector.h"	
#include "PndDchPoint.h"
#include "PndDchGeoPar.h"

// -----   Default constructor   -------------------------------------------
PndDchDetector::PndDchDetector() {
  fDchPointCollection        = new TClonesArray("PndDchPoint");
  fPosIndex   = 0;
  ResetParameters();
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndDchDetector::PndDchDetector(const char* name, Bool_t active)
  : CbmDetector(name, active) {
    fDchPointCollection        = new TClonesArray("PndDchPoint");
    fPosIndex   = 0;
    ResetParameters();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndDchDetector::~PndDchDetector() {
  if (fDchPointCollection) {
    fDchPointCollection->Delete();
    delete fDchPointCollection;
  }
  
}
// -------------------------------------------------------------------------

// -----   Public method ProcessHits  --------------------------------------
Bool_t PndDchDetector::ProcessHits(CbmVolume* vol) {

  TParticle* particle =  gMC->GetStack()->GetCurrentTrack();
  fTrackID  = gMC->GetStack()->GetCurrentTrackNumber(); 
  fEvent    = gMC->CurrentEvent(); 
  Bool_t  valid = kFALSE;

  gMC->CurrentVolID(fPlane);

  TString nam = gMC->CurrentVolName(); 
  if(!(nam.Contains("dchSplane"))) return kFALSE;

  nam = gMC->CurrentVolOffName(1);
  nam.Remove(0,nam.Length()-1);
  fChamber = nam.Atoi();
  if(fVerboseLevel>2)
    std::cout<<"III PndDchDetector::ProcessHits() : plane = "<<fPlane<<
      " chamber = "<<fChamber<<std::endl;

  //Increment number of dch points
  //if entering
  if ( gMC->IsTrackEntering() ) {	
    valid=kTRUE;
    Int_t points = gMC->GetStack()->GetCurrentTrack()->GetMother(1);
    Int_t nDchPoints = (points & (63<<10)) >> 10;
    nDchPoints++;
    if (nDchPoints <= 63) {
      points = ( points & ( ~ (63<<10) ) ) | (nDchPoints << 10);
      gMC->GetStack()->GetCurrentTrack()->SetMother(1,points);
    }
  }
   
  fELoss = gMC->Edep();
  fTime   = gMC->TrackTime() * 1.0e09; // ns
  fLength = gMC->TrackLength();
  gMC->TrackPosition(fPos);  // cm
  gMC->TrackMomentum(fMom); // GeV
  if (particle->Energy()>0.01 && fabs(particle->GetPDG()->Charge())>0.5  && valid)
  AddPoint(fTrackID, fEvent, fChamber, fPlane, 
	 TVector3(fPos.X(), fPos.Y(), fPos.Z()) , TVector3(fMom.Px(), fMom.Py(), fMom.Pz()),  
	 fTime, fLength, fELoss);

 ResetParameters();
 return kTRUE;
  
}
// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void PndDchDetector::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void PndDchDetector::Register() {
  CbmRootManager::Instance()->Register("PndDchPoint","Dch", fDchPointCollection, kTRUE);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* PndDchDetector::GetCollection(Int_t iColl) const {
   if (iColl == 0) return fDchPointCollection;

  return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndDchDetector::Print() const {
    Int_t nHits = fDchPointCollection->GetEntriesFast();
    cout << "-I- PndDchDetector: " << nHits << " points registered in this event."
 	<< endl;

    if (fVerboseLevel>1)
      for (Int_t i=0; i<nHits; i++) (*fDchPointCollection)[i]->Print();
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void PndDchDetector::Reset() {
  fDchPointCollection->Clear();  
  fPosIndex = 0;
}
// ----------------------------------------------------------------------------

// -----   Public method CopyClones   -----------------------------------------
// void PndDchDetector::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
//   Int_t nEntries = cl1->GetEntriesFast();
//   cout << "-I- PndDchDetector: " << nEntries << " entries to add." << endl;
//   TClonesArray& clref = *cl2;
//   PndDchPoint* oldpoint = NULL;
//   for (Int_t i=0; i<nEntries; i++) {
//     oldpoint = (PndDchPoint*) cl1->At(i);
//     Int_t index = oldpoint->GetTrackID() + offset;
//     oldpoint->SetTrackID(index);
//     new (clref[fPosIndex]) PndDchPoint(*oldpoint);
//     fPosIndex++;
//   }
//   cout << " -I- PndDchDetector: " << cl2->GetEntriesFast() << " merged entries."
//        << endl;
// }
// ----------------------------------------------------------------------------

// -----   Public method ConstructGeometry   ----------------------------------
void PndDchDetector::ConstructGeometry() {

   TString fileName=GetGeometryFileName();
   if (fileName.EndsWith(".geo")) {
      ConstructASCIIGeometry();
   } else if(fileName.EndsWith(".root")) {
      ConstructRootGeometry();
   } else {
      std::cout<< "Geometry format not supported " <<std::endl;
   }
}


void PndDchDetector::ConstructRootGeometry() {

   TFile *f=new TFile(GetGeometryFileName().Data());
   TGeoVolume *dchTop=(TGeoVolume *)f->Get("top");
   TGeoVolume *Cave = gGeoManager->GetTopVolume();

   //***
    TGeoNode* n = dchTop->GetNode(0);
//    TGeoVolume* v1= n->GetVolume();
//    gGeoManager->AddVolume(dchTop);
   TGeoVoxelFinder *voxels = dchTop->GetVoxels();
   if (voxels) voxels->SetNeedRebuild();
    TGeoMatrix *M = n->GetMatrix();
    M->SetDefaultName();
    gGeoManager->GetListOfMatrices()->Remove(M);
    TGeoHMatrix *global = gGeoManager->GetHMatrix();             
    gGeoManager->GetListOfMatrices()->Remove(global); //Remove the Identity matrix 
//    Cave->AddNode(v1,0, M);
   //***/

   ExpandNode(dchTop,Cave);
 
}

void PndDchDetector::ExpandNode(TGeoVolume *fVol, TGeoVolume *Cave){

   CbmGeoLoader*geoLoad = CbmGeoLoader::Instance();
   CbmGeoInterface *geoFace = geoLoad->getGeoInterface();
   CbmGeoMedia *Media =  geoFace->getMedia();
   CbmGeoBuilder *geobuild=geoLoad->getGeoBuilder();
  
   TObjArray *nodeList=fVol->GetNodes();
   if(fVerboseLevel>2)
     std::cout<< "DEBUG NodeListEntries = " << nodeList->GetEntries() << std::endl;
   
   for (Int_t nod=0; nod < nodeList->GetEntries(); nod++) {
     
     if(fVerboseLevel>2)   std::cout<< "DEBUG nod = " << nod << std::endl;
     TGeoNode *fNode =(TGeoNode *)nodeList->At(nod);
     TGeoVolume *v= fNode->GetVolume();
     if(fNode->GetNdaughters()>0) 	
       ExpandNode(v, Cave);
     
     TGeoMedium* med1=v->GetMedium();
     if(fVerboseLevel>2) std::cout<< "DEBUG NodeName = " << fNode->GetName() << std::endl;
     if (med1) {
       if(fVerboseLevel>2) std::cout<< "DEBUG medium  = " << med1->GetName() << std::endl;
       TGeoMaterial*mat1=v->GetMaterial(); 
       TGeoMaterial *newMat = gGeoManager->GetMaterial(mat1->GetName());
       if (newMat==0) {
	 std::cout<< "Material " << mat1->GetName() << " is not defined " << std::endl;
	 CbmGeoMedium *CbmMedium=Media->getMedium(mat1->GetName());
	 if (!CbmMedium) {
	   std::cout << "Material is not defined in ASCII file nor in Root file" << std::endl;
	   CbmMedium=new CbmGeoMedium(mat1->GetName());
	   Media->addMedium(CbmMedium);
	 }
	 std::cout << "Create Medium " << mat1->GetName() << std::endl;
	 Int_t nmed=geobuild->createMedium(CbmMedium);
	 v->SetMedium(gGeoManager->GetMedium(nmed));
	 gGeoManager->SetAllIndex();
       } else {
	  if(fVerboseLevel>2)  
	    std::cout<< "DEBUG material was defined  MaterialName= " << mat1->GetName() << std::endl;
	  TGeoMedium *med2= gGeoManager->GetMedium(mat1->GetName());
	  v->SetMedium(med2);
       }
     }
     if (!gGeoManager->FindVolumeFast(v->GetName())) {
       if(fVerboseLevel>2) std::cout<< "DEBUG registration form"  << std::endl;
       v->RegisterYourself();
     }
     //only dchVolume added to the Node list of Cave
     TString name = v->GetName();
     if(fVerboseLevel>2) std::cout<< "DEBUG VolumeName "  <<name<< std::endl;
     
     if (name.Contains("dchVol")) {
       if(fVerboseLevel>2) std::cout<< "DEBUG AddNode for volume "  <<name<< std::endl;
       Cave->AddNode(v,0, fNode->GetMatrix());
     }
     if (name.Contains("Splane")) {
       if (nod==0) {
	 if(fVerboseLevel>2) std::cout<< "DEBUG AddSensitive "  <<name<< std::endl;
	 AddSensitiveVolume(v);
       }
     }
   }
}




void PndDchDetector::ConstructASCIIGeometry() {
  std::cout<<" --- Building DCH Geometry from ASCI file ---"<<std::endl;

  CbmGeoLoader*    geoLoad = CbmGeoLoader::Instance();
  CbmGeoInterface* geoIFace = geoLoad->getGeoInterface();
  PndDchGeo*      dchGeo = new PndDchGeo();
  dchGeo->setGeomFile(GetGeometryFileName());
  geoIFace->addGeoModule(dchGeo);
  
  Bool_t rc = geoIFace->readSet(dchGeo);
  if (rc) dchGeo->create(geoLoad->getGeoBuilder());
  else std::cerr<<"PndDchDetector:: geometry could not be read!"<<std::endl;

  TList* volList = dchGeo->getListOfVolumes();
  std::cout<<"volList contains "<<volList->GetEntries()<<" volumes"<<std::endl;

  // store geo parameter
  CbmRun *fRun = CbmRun::Instance();
  CbmRuntimeDb *rtdb= CbmRun::Instance()->GetRuntimeDb();
  PndDchGeoPar* par=(PndDchGeoPar*)(rtdb->getContainer("PndDchGeoPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  CbmGeoNode* node   = NULL;
  CbmGeoVolume *aVol=NULL;

  while( (node = (CbmGeoNode*)iter.Next()) ) {
    std::cout<<"&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  "<<    node->GetName()<<std::endl;
    aVol = dynamic_cast<CbmGeoVolume*> ( node );
    if ( node->isSensitive()  ) {
      fSensNodes->AddLast( aVol );
    }else{
      fPassNodes->AddLast( aVol );
    }
  }
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);

  // set active/inactive
  ProcessNodes( volList );
  std::cout<<"DCH Geometry built"<<std::endl;

}
// ----------------------------------------------------------------------------

// -----   Private method AddPoint   --------------------------------------------
// see PndDchPoint for hit description
PndDchPoint* PndDchDetector::AddPoint(Int_t trackID, Int_t evtID,  Int_t chamber,  Int_t plane,
		   TVector3 pos, TVector3 mom, 
		   Double_t tof, Double_t length, Double_t eLoss) const {
  TClonesArray& clref = *fDchPointCollection;
  Int_t size = clref.GetEntriesFast();
  return  new(clref[size]) PndDchPoint(trackID, evtID, chamber, plane,
				    pos, mom, tof, length, eLoss);
  }
// ----------------------------------------------------------------------------

ClassImp(PndDchDetector)
