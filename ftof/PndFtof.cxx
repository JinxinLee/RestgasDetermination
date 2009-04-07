/////////////////////////////////////////////////////////////
//
//  FairFtof
//
//
//  created by A. Sanchez
//
///////////////////////////////////////////////////////////////

#include "PndFtof.h"

#include "PndFtofPoint.h"

#include "FairGeoTransform.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoMedium.h"
#include "PndGeoFtof.h"
#include "FairGeoRootBuilder.h"
#include "PndStack.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "PndDetectorList.h"


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

#include <string>
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;
using std::ostringstream;

// -----   Default constructor   -------------------------------------------
PndFtof::PndFtof() {
  fFtofCollection        = new TClonesArray("PndFtofPoint");
 
  SiId = 0;
  CId = 0;
  CpipeId = 0;
  alId = 0;
  beId = 0;
  fPosIndex   = 0; 
  // fpreflag = 0;  
  //fpostflag = 0;
  fEventID=-1; 

}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndFtof::PndFtof(const char* name, Bool_t active)
  : FairDetector(name, active) {
    fFtofCollection        = new TClonesArray("PndFtofPoint");
    
    SiId = 0;
     CId = 0;
     alId = 0;
     beId = 0;
    fPosIndex   = 0;
    
    fEventID=-1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndFtof::~PndFtof() {
  if (fFtofCollection) {
    fFtofCollection->Delete();
    delete fFtofCollection;
  }


 
  
}
// -------------------------------------------------------------------------



// -----   Public method Intialize   ---------------------------------------
void PndFtof::Initialize() {
  // Init function
  
  FairDetector::Initialize();
 
  
  //TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  
  
  //FairGeoMedium* Si = gGeoManager->GetMedium("silicon");->getMediumIndex();

  
  //TGeoMedium *Si= gGeoManager->GetMedium("polypropylene");
  TGeoMedium *Si= gGeoManager->GetMedium("polyvinyltoluene");
  
  SiId=  Si->GetId();
  
 
  
  
}
// -------------------------------------------------------------------------
void PndFtof::BeginEvent(){
  // Begin of the event
  
}



// -----   Public method ProcessHits  --------------------------------------

Bool_t PndFtof::ProcessHits(FairVolume* vol) 
{
  //FairGeoMedium* Si = vol->getGeoNode()->getMedium();
  //volSi = Si->getMediumIndex();
  //    TString nameSi = Si->getName();
  //  FairGeoMedium* Si = gGeoManager->GetMedium("silicon");
  //   volSi = Si->getMediumIndex();
  
  fpdgCode = gMC->TrackPid(); 

  TString nam2 = gMC->CurrentVolName();   
	 

  Double_t beta, gamma;	TString nam;
  ostringstream FullName,matName;
 
  Int_t medId =  gMC->CurrentMedium();
  TVector3 radt;
  

  if (nam2.Contains("Ftof") ){

    if ( gMC->IsTrackEntering() ) 
      {
	fELoss  = 0.;
	fEventID = gMC->CurrentEvent();
	fTime   = gMC->TrackTime() * 1.0e09;
	fLength = gMC->TrackLength();
	fmass   = gMC->TrackMass();   // mass (GeV)
	fcharge = gMC->TrackCharge(); // charge?
	fpdgCode = gMC->TrackPid(); 
	gMC->TrackPosition(fPosIn);
	gMC->TrackMomentum(fMomIn);

      }

    // Sum energy loss for all steps in the active volume
  
    fELoss += gMC->Edep();

    // Set additional parameters at exit of active volume. Create CbmStsPoint.
    //cout<<"Energy Loss  "<< fpdgCode << "  " << fELoss <<endl;
    TLorentzVector PL; 
	 gMC->TrackMomentum(PL);
	 
	 if ( (gMC->IsTrackExiting()    ||
	       gMC->IsTrackStop()       ||
	       gMC->IsTrackDisappeared() ))//&& gMC->TrackCharge()  ) 
      {
	fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
	 Int_t cp=-1;
	 //TString nam2 = gMC->CurrentVolOffName(1);  
	  gMC->CurrentVolOffID(1,cp) ;
	  

	
	  if(cp==0)fVolumeID = (cp+1) * (1 + vol->getCopyNo());
	  else fVolumeID = (cp) * (1 + vol->getCopyNo());
	    FairGeoNode* node = vol->getGeoNode();
	TList* nodeList = node->getTree();
	//cout << "FullName: " << vol->getName() << "/"<<endl;
	for (Int_t index=0; index < nodeList->GetSize(); index++)
	  {
	    FairGeoNode* myNode = dynamic_cast<FairGeoNode*> ( nodeList->At(index) );
	    //cout << myNode->getName() << "/";
	  }
	//**************///
	  

	
	 
	
	//FullName << gGeoManager->GetPath();
      
	  
	  
	  //cout << "*******  Info from gMC *************" << endl;
	  //Int_t cp=-1;
	  Int_t fVolid = gMC->CurrentVolID(cp);
	  Int_t nSiL = -1;
	  
	 
	 
	  FullName <<gMC->CurrentVolPath();
 
	 
	  nam = FullName.str();
	  
	  
	   gMC->TrackPosition(fPosOut);
	  gMC->TrackMomentum(fMomOut);

	 

	if (fELoss == 0. ) return kFALSE;
    
	radt= fPosOut.Vect();
	fdist=radt.Perp();
	//beta = fMomOut.Beta();

	//fPLin = beta;
	fPLin =fMomIn.P();
	

	fPLout = fMomOut.P();

	AddHit(fTrackID, fEventID,fVolumeID, FullName.str(),
	       TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	       TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	       TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	       TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	      fTime, fLength,fELoss,fcharge,fmass,fpdgCode,
	       fdist,fPLin,fPLout);

        PndStack* stack = (PndStack*) gMC->GetStack();
        stack->AddPoint(kTOF);
	
        ResetParameters();
      }

    //return kTRUE;
  
   
  
   }
  
    
   return kTRUE;


}//ProcessHits

// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void PndFtof::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void PndFtof::Register() {
  FairRootManager::Instance()->Register("FtofPoint","Ftof", fFtofCollection, kTRUE);

  
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* PndFtof::GetCollection(Int_t iColl) const {
   if (iColl == 0) return fFtofCollection;

  

  return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndFtof::Print() const {
    Int_t nHits = fFtofCollection->GetEntriesFast();
    cout << "-I- PndFtof: " << nHits << " points registered in this event."
 	<< endl;

    if (fVerboseLevel>1)
      for (Int_t i=0; i<nHits; i++) (*fFtofCollection)[i]->Print();
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void PndFtof::Reset() {
   fFtofCollection->Clear();

 
  fPosIndex = 0;
}
// ----------------------------------------------------------------------------


// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void PndFtof::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
  Int_t nEntries = cl1->GetEntriesFast();
  //cout << "-I- PndFtof: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  PndFtofPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (PndFtofPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndFtofPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- PndFtof: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}
// ----------------------------------------------------------------------------
 // -----   Public method ConstructGeometry   ----------------------------------
void PndFtof::ConstructGeometry() {
 FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  PndGeoFtof*      hypGeo = new PndGeoFtof();
  hypGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(hypGeo);

  Bool_t rc = geoFace->readSet(hypGeo);
  if (rc) hypGeo->create(geoLoad->getGeoBuilder());
  TList* volList = hypGeo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  PndGeoFtofPar* par=(PndGeoFtofPar*)(rtdb->getContainer("PndGeoFtofPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume *aVol=NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
    aVol = dynamic_cast<FairGeoVolume*> ( node );


    if ( node->isSensitive()  ) {
      fSensNodes->AddLast( aVol );
    }else{
      fPassNodes->AddLast( aVol );
    }
  }
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);

  ProcessNodes ( volList );



}
  
 

// -----   Private method AddHit   --------------------------------------------

PndFtofPoint* PndFtof::AddHit(Int_t trackID, Int_t evtID, Int_t detID, TString detName,
			    TVector3 pos, TVector3 mom,
			    TVector3 posout, 
			    TVector3 momout,
			   Double_t time,
			    Double_t length, 
			    Double_t eLoss,
			    Double_t charge, Double_t mass,
			    Int_t pdgCode,
			    Double_t dist,
			    Double_t PLin,Double_t PLout) {
  TClonesArray& clref = *fFtofCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndFtofPoint(trackID, evtID,detID, detName,pos, mom, 
				      posout, momout,
				      time, length, eLoss,charge, 
				      mass,pdgCode,
				      dist,PLin,PLout);
 }



// ----





// ----



// ----


ClassImp(PndFtof)
