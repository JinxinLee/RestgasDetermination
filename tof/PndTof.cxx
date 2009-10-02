/////////////////////////////////////////////////////////////
//
//  CbmHyp
//
//  Filler of CbmHypPoint
//
//  created by A. Sanchez
//
///////////////////////////////////////////////////////////////

#include "PndTof.h"

#include "PndTofPoint.h"

#include "FairGeoTransform.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoMedium.h"
#include "PndGeoTof.h"
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
PndTof::PndTof() {
  fTofCollection        = new TClonesArray("PndTofPoint");
  fTofSciFCollection  = new TClonesArray("PndTofPoint");
 
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
PndTof::PndTof(const char* name, Bool_t active)
  : FairDetector(name, active) {
    fTofCollection        = new TClonesArray("PndTofPoint");
    fTofSciFCollection  = new TClonesArray("PndTofPoint");
   
    SiId = 0;
     CId = 0;
     alId = 0;
     beId = 0;
    fPosIndex   = 0;
    
    fEventID=-1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndTof::~PndTof() {
  if (fTofCollection) {
    fTofCollection->Delete();
    delete fTofCollection;
  }
  if (fTofSciFCollection) {
    fTofSciFCollection->Delete();
    delete fTofSciFCollection;
  }

 
  
}
// -------------------------------------------------------------------------



// -----   Public method Intialize   ---------------------------------------
void PndTof::Initialize() {
  // Init function
  
  FairDetector::Initialize();
 
  
  //TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  
  
  //FairGeoMedium* Si = gGeoManager->GetMedium("silicon");->getMediumIndex();

  
  //TGeoMedium *Si= gGeoManager->GetMedium("polypropylene");
  TGeoMedium *Si= gGeoManager->GetMedium("polyvinyltoluene");
  
  if (Si!=0) SiId=  Si->GetId();
  
  
}
// -------------------------------------------------------------------------
void PndTof::BeginEvent(){
  // Begin of the event
  
}



// -----   Public method ProcessHits  --------------------------------------

Bool_t PndTof::ProcessHits(FairVolume* vol) 
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
  

  if (nam2.Contains("tof") ){
    //&& ((fpdgCode==321)||(fpdgCode==-321))){
    //fpdgCode = gMC->TrackPid(); 
     
    if ( gMC->IsTrackEntering() ) 
      {
	fELoss  = 0.;fEventID = gMC->CurrentEvent();
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
	  
	  //if ((nam2.Contains("Si"))) {
	  // sscanf(nam2,"stglSi%d#01", &nSiL);
	  //cout << "hyp::ProcessHits> : " << nam2 <<" # "
	  // <<gMC->CurrentVolOffID(1,cp)<<" "<<"Hit in "<<cp<<" "
	  // << gGeoManager->GetPath()<<endl;
	     // } 

	//fVolumeID = vol->getMCid();
	
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
  
  if (nam2.Contains("sci") ){
    //&& ((fpdgCode==321)||(fpdgCode==-321))){
         //
       
//	 cout<<"name sciF"<<nam2<<endl;
	 
       if ( gMC->IsTrackEntering() ) 
	 {
	     fELoss  = 0.;fEventID = gMC->CurrentEvent();
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
	 // Gamma, Beta, tau(proper time) of ximnus
	 TLorentzVector PL;
	 gMC->TrackMomentum(PL);
	 beta = PL.P();
	
	
	 if ( (gMC->IsTrackExiting()    ||
	      gMC->IsTrackStop()       ||
	       gMC->IsTrackDisappeared()) )//&& gMC->TrackCharge()) 
	   {    
	     fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
	    
	     //vol->getMCid();
	     gMC->TrackPosition(fPosOut);
	     gMC->TrackMomentum(fMomOut);
	     TString nam3 = gMC->CurrentVolOffName(0); 
	     if (nam3.Contains("tsciF04"))fVolumeID = vol->getCopyNo();
	     if( nam3.Contains("tsciF05"))fVolumeID = 501+vol->getCopyNo();
	     // cout<<" sciF "<< fVolumeID<<" "<<vol->getMCid()<<nam3<<endl;
	     
	     
	    // ostringstream matName;
	    TString mat[4]={"CAbs","Si","Be","Al"};
	    
	    matName <<gMC->CurrentVolPath();
	    
	    //if (medId==SiId) matName<<"polypropilene";
	    //cout << "Hit in fullname " << matName.str() <<endl;
	     
	    if ((fELoss == 0. )&&(fpdgCode!=2112 )) {
	      return kFALSE;
	    }
	 
	  
	     //---Kinetic energy:###(PL.P())^2 + Mass^2 -Mass##
	     radt= fPosOut.Vect();
	     fdist=radt.Perp();
	     //beta = fMomOut.Beta();
	     //gamma = fMomOut.Gamma();
	     fPLin = beta;
	     //fPLin = fMomIn.P();
	     fPLout = fMomOut.P();
 
	     AddSciFHit(fTrackID, fEventID,fVolumeID,matName.str(),
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

	  return kTRUE;
  
    
  
     }//no volSi
   

  
    
   return kTRUE;


}//ProcessHits

// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void PndTof::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Reset();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void PndTof::Register() {
  FairRootManager::Instance()->Register("TofPoint","Tof", fTofCollection, kTRUE);
  FairRootManager::Instance()->Register("TofSciFPoint","TofSciF", 
				       fTofSciFCollection, kTRUE);
  
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* PndTof::GetCollection(Int_t iColl) const {
   if (iColl == 0) return fTofCollection;
   if (iColl == 1) return fTofSciFCollection;
  

  return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndTof::Print() const {
    Int_t nHits = fTofCollection->GetEntriesFast();
    cout << "-I- PndTof: " << nHits << " points registered in this event."
 	<< endl;

    if (fVerboseLevel>1)
      for (Int_t i=0; i<nHits; i++) (*fTofCollection)[i]->Print();
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void PndTof::Reset() {
   fTofCollection->Clear();
   fTofSciFCollection->Clear();
   
 
  fPosIndex = 0;
}
// ----------------------------------------------------------------------------


// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void PndTof::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
  Int_t nEntries = cl1->GetEntriesFast();
  //cout << "-I- PndTof: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  PndTofPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (PndTofPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndTofPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- PndTof: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}
// ----------------------------------------------------------------------------
 // -----   Public method ConstructGeometry   ----------------------------------
void PndTof::ConstructGeometry() {
 FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  PndGeoTof*      hypGeo = new PndGeoTof();
  hypGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(hypGeo);

  Bool_t rc = geoFace->readSet(hypGeo);
  if (rc) hypGeo->create(geoLoad->getGeoBuilder());
  TList* volList = hypGeo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  PndGeoTofPar* par=(PndGeoTofPar*)(rtdb->getContainer("PndGeoTofPar"));
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

PndTofPoint* PndTof::AddHit(Int_t trackID, Int_t evtID, Int_t detID, TString detName,
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
  TClonesArray& clref = *fTofCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndTofPoint(trackID, evtID,detID, detName,pos, mom, 
				      posout, momout,
				      time, length, eLoss,charge, 
				      mass,pdgCode,
				      dist,PLin,PLout);
 }



// ----

// -----   Private method AddSciFHit   --------------------------------------------

PndTofPoint* PndTof::AddSciFHit(Int_t trackID, Int_t evtID, Int_t detID, TString detName,
			    TVector3 pos, TVector3 mom,  
				  TVector3 posout, 
				  TVector3 momout, 
				  Double_t time, 
				  Double_t length, 
				  Double_t eLoss,
				  Double_t charge, Double_t mass,
				  Int_t pdgCode,Double_t dist,
				  Double_t PLin,Double_t PLout) {
  TClonesArray& clref = *fTofSciFCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndTofPoint(trackID, evtID,detID, detName, pos, mom,  posout, momout,
				     time, length, eLoss,charge, mass, pdgCode,
				      dist,PLin,PLout);
 }



// ----



// ----


ClassImp(PndTof)
