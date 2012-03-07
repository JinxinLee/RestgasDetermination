/////////////////////////////////////////////////////////////
//
//  PndFtof
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
#include "FairGeoInterface.h"
#include "FairGeoMedia.h"

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
#include "TGeoVoxelFinder.h"
#include "TGeoMatrix.h"
//#include "FairGeoG3Builder.h"

#include <string>
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;
using std::ostringstream;

// -----   Default constructor   -------------------------------------------
PndFtof::PndFtof() {
  fFtofCollection        = new TClonesArray("PndFtofPoint");
 
  fListOfSensitives.push_back("Ftof_Central_Strip");//Root_Test.root
  fListOfSensitives.push_back("Ftof_Beam_Strip");//Root_Test.root
  fListOfSensitives.push_back("Ftof_Vertical_Strip");//Root_Test.root
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndFtof::PndFtof(const char* name, Bool_t active)
  : FairDetector(name, active) {
    fFtofCollection        = new TClonesArray("PndFtofPoint");

    fListOfSensitives.push_back("Ftof_Central_Strip");//Root_Test.root
    fListOfSensitives.push_back("Ftof_Beam_Strip");//Root_Test.root
    fListOfSensitives.push_back("Ftof_Vertical_Strip");//Root_Test.root
 
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
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  par=(PndGeoFtofPar*)(rtdb->getContainer("PndGeoFtofPar"));
  par->setChanged();
  par->setInputVersion(sim->GetRunId(),1);

  //TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  std::cout<<" -I- Initializing PndFtofDetector()"<<std::endl;
  
  if(0==gGeoManager) {
    std::cout<<" -E- No gGeoManager in PndFtofDetector::Initialize()!"<<std::endl;
    abort();
  }
 
  
  
}
// -------------------------------------------------------------------------
void PndFtof::BeginEvent(){
  // Begin of the event
  
}



// -----   Public method ProcessHits  --------------------------------------

Bool_t PndFtof::ProcessHits(FairVolume* vol) 
{
  
  TString nam2 = gMC->CurrentVolName();   
	 

  Double_t beta, gamma;	TString nam;
  ostringstream FullName,matName;
 
  Int_t medId =  gMC->CurrentMedium();
  TVector3 radt;
  

  //if (nam2.Contains("Ftof") )cout<<"Energy Loss  "<<endl;
  

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
	  

	  fVolumeID = vol->getMCid();
	 
	 
	  
	  cout << "*******  Info from gMC *************" << endl;
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
	// that has to be replaced by 
	// the corresponding KFTOF
        stack->AddPoint(kTOF);
	
        ResetParameters();
      }
	 
	 

    //return kTRUE;
  
   return kTRUE;


}//ProcessHits

// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void PndFtof::EndOfEvent() {
  if (fVerboseLevel)  Print();
  Print();
  
  Reset();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void PndFtof::Register() {
  FairRootManager::Instance()->Register("FtofPoint","PndFtof", fFtofCollection, kTRUE);

  
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

 TString fileName=GetGeometryFileName();

  if(fileName.EndsWith(".root")){
    ConstructRootGeometry();
  }else{

   ConstructASCIIGeometry();
    
  }
 
}
void PndFtof::ConstructASCIIGeometry() {


  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();

  FairGeoMedia *Media =  geoFace->getMedia();
  FairGeoBuilder *geobuild=geoLoad->getGeoBuilder();

  FairGeoMedium *CbmMediumSci  = Media->getMedium("polyvinyltoluene");
 
  
  Int_t nmed=geobuild->createMedium(CbmMediumSci);
  TString vname = "cave";
  TGeoVolume* vcave = gGeoManager->FindVolumeFast(vname);

   const Double_t  kBCentX    = 2.5;   // half length(cm) //from EMC TDR
  const Double_t  kBCentY    = 70.0;   //half length (cm) //?
  const Double_t  kBCentZ    = 0.75;   //half length (cm) //?

  const Double_t  kBBeamX    = 2.5;   // half length(cm) //from EMC TDR
  const Double_t  kBBeamY    = 32.5;  //alf length (cm) //?
  const Double_t  kBBeamZ    = 0.75;   //half length (cm) //?

  const Double_t  kBVertX    = 5.0;   //half length (cm) //from EMC TDR
  const Double_t  kBVertY    = 70.0;   //half length (cm) //?
  const Double_t  kBVertZ    = 0.75;   //half length (cm) //from EMC TDR

 
  TGeoBBox *CentShape;
  TGeoBBox *BeamShape;
  TGeoBBox *VertShape;
  
  TGeoVolume* CentVol;
  TGeoVolume* BeamVol;
  TGeoVolume* VertVol;
  
  
  Double_t bx[22],by[22],bz[22];
  Double_t bvx[46],bvy[46],bvz[46];
  TGeoVolumeAssembly* SubunitVol = new TGeoVolumeAssembly("Ftof_strips");
  
  
  char name[13];
  char namB[13];
  char namV[13];
  char name1[13];
  char namB1[13];
  char namV1[20];
  char name2[13];
  char namB2[13];
  char namV2[13];
  
  
  for(int i=0;i<22;i++){
  bx[i]=-40+kBCentX*((2*i)+1);
}

  for(int i=0;i<22;i++){
    if(i<9||i>12){
      TGeoCombiTrans* trc1;
      
      Double_t offset;
      sprintf (name,"Centshape%d",i);
      sprintf (name1,"Ftof_Central_Strip%d",i);
      
      CentShape = new TGeoBBox("Centshape",kBCentX,kBCentY,kBCentZ);
      CentVol = new TGeoVolume("Ftof_Central_Strip",CentShape,gGeoManager->GetMedium("polyvinyltoluene"));
      if(i<9)trc1= new TGeoCombiTrans(bx[i],0,750,new TGeoRotation ()); 
      else if (i>12)trc1= new TGeoCombiTrans(bx[i-2],0,750,new TGeoRotation ());
      
      trc1->SetName(name);
      trc1->RegisterYourself();
      SubunitVol->AddNode(CentVol,i,trc1);
      AddSensitiveVolume(CentVol);
    }
    else{
       Double_t offset;
      sprintf (namB,"Beamshape%d",i);
      sprintf (namB1,"Ftof_Beam_Strip%02d",i);
      BeamShape = new TGeoBBox("Beamshape",kBBeamX,kBBeamY,kBBeamZ);
      BeamVol = new TGeoVolume("Ftof_Beam_Strip",BeamShape,gGeoManager->GetMedium("polyvinyltoluene"));
      
      if(i==9||i==10){
	
	
	(i==9 ? (offset = -70+kBBeamY) : (offset = 70-kBBeamY));
	TGeoCombiTrans* trc1= new TGeoCombiTrans(bx[9],offset,750,new TGeoRotation ()); 
	
	trc1->SetName(namB);
	trc1->RegisterYourself();
	SubunitVol->AddNode(BeamVol,i,trc1);
	AddSensitiveVolume(BeamVol);
      }
      if(i==11||i==12){
	
	(i==11 ?  (offset = -70+kBBeamY) : (offset = 70-kBBeamY));
	
	TGeoCombiTrans* trc1= new TGeoCombiTrans(bx[10],offset,750,new TGeoRotation ()); 
	
	trc1->SetName(namB);
	trc1->RegisterYourself();
	SubunitVol->AddNode(BeamVol,i,trc1);
	AddSensitiveVolume(BeamVol);
      }
    }
    
  }
  
  for(int i=0;i<24;i++){
    bvx[i]=-280+kBVertX*((2*i)+1);
  }
  
  for(int i=0;i<22;i++){
    
    bvx[i+24]=60+kBVertX*((2*i)+1);
  }
  
  for(int i=0;i<46;i++){
    
    
    sprintf (namV,"VertShape%d",i);
    sprintf (namV1,"Ftof_Vertical_Strip%d",i);
    
    VertShape = new TGeoBBox("VertShape",kBVertX,kBVertY,kBVertZ);
    VertVol = new TGeoVolume("Ftof_Vertical_Strip",VertShape,gGeoManager->GetMedium("polyvinyltoluene"));
    TGeoCombiTrans* trc1= new TGeoCombiTrans(bvx[i],0,750,new TGeoRotation ());
    
    trc1->SetName(namV);
    trc1->RegisterYourself();
    SubunitVol->AddNode(VertVol,i,trc1);
    AddSensitiveVolume(VertVol);
  }
  
  
  vcave->AddNode(SubunitVol,0,new TGeoCombiTrans());
 

}
  
// -------------------------------------------------------------------------
bool PndFtof::CheckIfSensitive(std::string name)
{
  for (Int_t i = 0; i < fListOfSensitives.size(); i++){
    if (name.find(fListOfSensitives[i]) != std::string::npos)
    return true;
  }
  return false;
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
