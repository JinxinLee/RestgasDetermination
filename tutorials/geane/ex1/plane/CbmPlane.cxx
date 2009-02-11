
// -------------------------------------------------------------------------


#include "iostream.h"

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "CbmGeoPlane.h"
#include "FairGeoRootBuilder.h"
#include "CbmStack.h"
#include "CbmPlane.h"
#include "CbmPlanePoint.h"
#include "FairRootManager.h"
#include "FairVolume.h"
// add on for debug
#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"
#include "CbmGeoPlanePar.h"
#include "TObjArray.h"
#include "FairRun.h"

#include "TGeant3.h"

// -----   Default constructor   -------------------------------------------
CbmPlane::CbmPlane() {
  fPlane1Collection  = new TClonesArray("CbmPlanePoint");
  fPlane2Collection  = new TClonesArray("CbmPlanePoint");
  fPlane3Collection  = new TClonesArray("CbmPlanePoint");

//   fPlaneImPlaneCollection = new TClonesArray("CbmPlanePoint");
  fPosIndex   = 0;
  fpreflag = 0;
  fpostflag = 0;
  volDetector = 0;
  
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmPlane::CbmPlane(const char* name, Bool_t active)
  : FairDetector(name, active) {
    fPlane1Collection        = new TClonesArray("CbmPlanePoint");
    fPlane2Collection        = new TClonesArray("CbmPlanePoint"); 
    fPlane3Collection        = new TClonesArray("CbmPlanePoint");


//   fPlaneImPlaneCollection = new TClonesArray("CbmPlanePoint");
  fPosIndex   = 0;
  fpreflag = 0;
  fpostflag = 0;
  volDetector = 0;
  fevent=-1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmPlane::~CbmPlane() {
  if (fPlane1Collection) {
    fPlane1Collection->Delete();
    delete fPlane1Collection;
  }
  
}
// -------------------------------------------------------------------------



// -----   Public method Intialize   ---------------------------------------
void CbmPlane::Initialize() {
  
  // controlla-- forse non tutto e' necessario.
  FairDetector::Initialize();
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  CbmGeoPlanePar *par=(CbmGeoPlanePar*)(rtdb->getContainer("CbmGeoPlanePar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
 
 //  FairGeoNode *fm1= (FairGeoNode *) fSensNodes->FindObject("Plane0000");
//   cout << "GET COPY NO: " << fm1->getCopyNo() << endl;
//   volDetector = fm1->getMCid();

 
}
// -------------------------------------------------------------------------
void CbmPlane::BeginEvent(){
 
}



// -----   Public method ProcessHits  --------------------------------------
Bool_t CbmPlane::ProcessHits(FairVolume* vol) {
  
 
    Int_t      pdgCode = gMC->TrackPid();
  if(pdgCode==13){
  fmass   = gMC->TrackMass();   // mass (GeV)
  fcharge = gMC->TrackCharge(); //charge?
  fTrackID  = gMC->GetStack()->GetCurrentTrackNumber(); // trk ID
  Int_t copyNo;  
  gMC->CurrentVolID(copyNo);

  // if entering
  if ( gMC->IsTrackEntering() ) {
    fpreflag = 1;
    fELoss  = 0.;
    gMC->TrackPosition(fPosIn); // cm
    gMC->TrackMomentum(fMomIn); // GeV
  }
   
  // Sum energy loss for all steps in the active volume
   fELoss += gMC->Edep();

  // Set additional parameters at exit of active volume. Create CbmPlanePoint.
   if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
 
   fpostflag = 1;
   gMC->TrackPosition(fPosOut); // cm
   gMC->TrackMomentum(fMomOut); // GeV
   fLength = gMC->TrackLength();
   fTime   = gMC->TrackTime() * 1e9; // time
   //    if (fELoss == 0. ) return kFALSE;

   Bool_t fmuch = kFALSE;

//    cout << vol->getMCid() << " " << gMC->CurrentVolPath() << endl;


   if(fmuch == kTRUE) {
     if(vol->getMCid()==8){
       AddHit2(fTrackID, fVolumeID,
	       TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	       TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	       TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	       TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	       fTime, fLength, fELoss,  
	       fcharge, fmass, fpreflag, fpostflag, "P1",fevent);
	ResetParameters();
     }else if(vol->getMCid() ==3){
       AddHit1(fTrackID, fVolumeID,
	       TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
	       TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
	       TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
	       TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
	       fTime, fLength, fELoss,  
	       fcharge, fmass, fpreflag, fpostflag, "P2",fevent);
       ResetParameters();
     } 
   }
   else if(fmuch == kFALSE)
     { 
       // 45
       if(vol->getMCid()==3){ //36 //37 //2814 //1959
	 AddHit1(fTrackID, fVolumeID,
		 TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
		 TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
		 TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
		 TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
		 fTime, fLength, fELoss,  
		 fcharge, fmass, fpreflag, fpostflag, "P1",fevent);
	 ResetParameters();
       }
       else if(vol->getMCid() ==4){ //37 // 38 //2815 //1960
	 AddHit2(fTrackID, fVolumeID,
		 TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
		 TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
		 TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
		 TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
		 fTime, fLength, fELoss,  
		 fcharge, fmass, fpreflag, fpostflag, "P2",fevent);
	 ResetParameters();
       } 
       else if(vol->getMCid() == 2){ //35 // 36 //2813 //1958
	 AddHit3(fTrackID, fVolumeID,
		 TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
		 TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
		 TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
		 TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
		 fTime, fLength, fELoss,  
		 fcharge, fmass, fpreflag, fpostflag, "P3",fevent);
	 ResetParameters();
       } 
     }
   }
  }
  return kTRUE;
  
}
// ----------------------------------------------------------------------------



// -----   Public method EndOfEvent   -----------------------------------------
void CbmPlane::EndOfEvent() {

  if (fVerboseLevel)  Print();
  Reset();
  if (fevent==10000-1) plane();

}
// ----------------------------------------------------------------------------
void CbmPlane::plane() {
  //
 /* // GEANE stuff
  //
  cout<<"Calling GEANE..."<<endl;
  TGeant3* gMC3 = (TGeant3*) gMC;
  //gMC3->Gpcxyz(); //a simple test
  Gctmed_t *afGmed = gMC3->Gctmed(); //a simple test
  //cout<<afGmed->epsil<<endl;
  //EUFILP
  Ertrio_t *afErtrio=gMC3->Ertrio();
  Eropts_t *afEropts=gMC3->Eropts();
  for(Int_t i=0;i<15;i++) afErtrio->errin[i]=0.; 
  afErtrio->nepred=1;
  for(Int_t i=0;i<6;i++) afEropts->erpli[i]=0.; 
  afEropts->erpli[1]=1.;
  afEropts->erpli[5]=1.;
  for(Int_t i=0;i<6;i++) afEropts->erplo[i]=afEropts->erpli[i]; 
  afEropts->erplo[6]=1000.;
  afEropts->erplo[7]=0.;
  afEropts->erplo[8]=0.;
  TVector3 v1(afEropts->erplo[0],afEropts->erplo[1],afEropts->erplo[2]);
  TVector3 v2(afEropts->erplo[3],afEropts->erplo[4],afEropts->erplo[5]);
  TVector3 v3=v1.Cross(v2);
  afEropts->erplo[9]=v3(0);
  afEropts->erplo[10]=v3(1);
  afEropts->erplo[11]=v3(2);
  //ERTRAK
  Float_t x1[3]={0.,0.,0.};
  Float_t p1[3]={1.0,0.,0.};
  Float_t x2[3];
  Float_t p2[3];
  gMC3->Ertrak(x1,p1,x2,p2,5,"P");
  
  ofstream f("plane.out");
  f<<x2[0]<<"\t"<<x2[1]<<"\t"<<x2[2]<<endl;
  f<<p2[0]<<"\t"<<p2[1]<<"\t"<<p2[2]<<endl;
  f<<x1[0]<<"\t"<<x1[1]<<"\t"<<x1[2]<<endl;
  f<<p1[0]<<"\t"<<p1[1]<<"\t"<<p1[2]<<endl;
  f<<afErtrio->erpout[0]<<"\t"<<afErtrio->erpout[1]<<"\t"<<afErtrio->erpout[2]<<endl;
  f<<afEropts->erplo[9]<<"\t"<<afEropts->erplo[10]<<"\t"<<afEropts->erplo[11]<<endl;
  f<<afEropts->leexac<<endl;
  f<<afEropts->leleng<<endl;
  f<<afEropts->leonly<<endl;
  f<<afEropts->leplan<<endl;
  f<<afEropts->lepoin<<endl;
  f<<afEropts->levolu<<endl;
  f<<endl;
  for(Int_t i=0;i<15;i++) f<<i<<"\t"<<afErtrio->errout[i]<<endl;
  f.close();
*/

}



// -----   Public method Register   -------------------------------------------
void CbmPlane::Register() {
  FairRootManager::Instance()->Register("Plane1Point","Plane1", fPlane1Collection, kTRUE);
  FairRootManager::Instance()->Register("Plane2Point","Plane2", fPlane2Collection, kTRUE);
  FairRootManager::Instance()->Register("Plane3Point","Plane3", fPlane3Collection, kTRUE);
}
// ----------------------------------------------------------------------------



// -----   Public method GetCollection   --------------------------------------
TClonesArray* CbmPlane::GetCollection(Int_t iColl) const {
   if (iColl == 0) return fPlane1Collection;

  return NULL;
}
// ----------------------------------------------------------------------------




// -----   Public method Print   ----------------------------------------------
void CbmPlane::Print() const {
   
}
// ----------------------------------------------------------------------------



// -----   Public method Reset   ----------------------------------------------
void CbmPlane::Reset() {
   fPlane1Collection->Clear();
   fPlane2Collection->Clear();
   fPlane3Collection->Clear();

  fPosIndex = 0;
}
// ----------------------------------------------------------------------------


// guarda in FairRootManager::CopyClones
// -----   Public method CopyClones   -----------------------------------------
void CbmPlane::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) {
  
 
}
// ----------------------------------------------------------------------------



// -----   Public method ConstructGeometry   ----------------------------------
void CbmPlane::ConstructGeometry() {

  FairGeoLoader*    PlanegeoLoad = FairGeoLoader::Instance();
  FairGeoInterface* PlanegeoFace = PlanegeoLoad->getGeoInterface();
  CbmGeoPlane*      PlaneGeo = new CbmGeoPlane();
  PlaneGeo->setGeomFile(GetGeometryFileName());
  PlanegeoFace->addGeoModule(PlaneGeo);

  Bool_t rc = PlanegeoFace->readSet(PlaneGeo);
  if (rc) PlaneGeo->create(PlanegeoLoad->getGeoBuilder());
  TList* volList = PlaneGeo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  CbmGeoPlanePar* par=(CbmGeoPlanePar*)(rtdb->getContainer("CbmGeoPlanePar"));
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

  ProcessNodes ( volList );
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);

}
// ----------------------------------------------------------------------------

 CbmPlanePoint* CbmPlane::AddHit1(Int_t trackID, Int_t detID, TVector3 posIn, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t time, Double_t length, Double_t eLoss, Double_t charge, Double_t mass, Bool_t preflag, Bool_t postflag, TString nam,Int_t evt)
{

	TClonesArray& clref = *fPlane1Collection;
  	Int_t size = clref.GetEntriesFast();

 return new(clref[size]) CbmPlanePoint(trackID, detID, posIn, posOut,
				      momIn, momOut, time, length, eLoss, charge,
                                      mass, fpreflag, fpostflag, nam,evt);

}

CbmPlanePoint* CbmPlane::AddHit2(Int_t trackID, Int_t detID, TVector3 posIn, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t time, Double_t length, Double_t eLoss, Double_t charge, Double_t mass, Bool_t preflag, Bool_t postflag, TString nam,Int_t evt)
{
   TClonesArray& clref = *fPlane2Collection;
  	Int_t size = clref.GetEntriesFast();
 return new(clref[size]) CbmPlanePoint(trackID, detID, posIn, posOut,
				      momIn, momOut, time, length, eLoss, charge,
                                      mass, fpreflag, fpostflag, nam,evt);
}


CbmPlanePoint* CbmPlane::AddHit3(Int_t trackID, Int_t detID, TVector3 posIn, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t time, Double_t length, Double_t eLoss, Double_t charge, Double_t mass, Bool_t preflag, Bool_t postflag, TString nam,Int_t evt)
{
   TClonesArray& clref = *fPlane3Collection;
  	Int_t size = clref.GetEntriesFast();
 return new(clref[size]) CbmPlanePoint(trackID, detID, posIn, posOut,
				      momIn, momOut, time, length, eLoss, charge,
                                      mass, fpreflag, fpostflag, nam,evt);
}



ClassImp(CbmPlane)
