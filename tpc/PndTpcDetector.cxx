//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcDetector
//      see PndTpcDetector.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcDetector.h"

// C/C++ Headers ----------------------
#include <string>

// Collaborating Class Headers --------
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "FairRootManager.h"
#include "PndTpcPoint.h"
#include "TVirtualMC.h"
#include "TLorentzVector.h"
#include "PndTpcGeo.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "TList.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndTpcGeoPar.h"
#include "TObjArray.h"
#include "FairGeoNode.h"
#include "FairGeoVolume.h"
#include "FairVolume.h"
#include "TParticle.h"
#include "PndStack.h"

#include "FairGeoMedia.h"
#include "TRandom.h"

// Class Member definitions -----------


PndTpcDetector::PndTpcDetector(const char * Name, Bool_t Active)
  : FairDetector(Name, Active),fAliMC(kFALSE), fDeltaAttach(kFALSE), 
    fCut_el(1.0E-1), fCut_had(1.0E-3),  //standard values 1MeV
    fAllSensitive(kFALSE)
{
  fPndTpcPointCollection= new TClonesArray("PndTpcPoint");
  fMixture="TPCmixture"; //default to PANDA TPC mixture
}

PndTpcDetector::PndTpcDetector()
{ 
  fPndTpcPointCollection= new TClonesArray("PndTpcPoint");
}

PndTpcDetector::~PndTpcDetector()
{
  if (fPndTpcPointCollection) {
      fPndTpcPointCollection->Delete();
      delete fPndTpcPointCollection;
   }
}

void PndTpcDetector::EndOfEvent() 
{

  //fPndTpcPointCollection->Clear();
  
}

void PndTpcDetector::Register() {

/* This will create a branch in the output tree called PndTpcDetectorPoint, 
   setting the last parameter to kFALSE means:
   this collection will not be written to the file, it will exist only during the simulation. */
 
   FairRootManager::Instance()->Register("PndTpcPoint", "PndTpc", fPndTpcPointCollection, kTRUE);
}


//overwrite virtual method of FairDetector
void PndTpcDetector::SetSpecialPhysicsCuts(){
  FairRun* fRun = FairRun::Instance();
  
  //check for GEANT3, else abort
  if (strcmp(fRun->GetName(),"TGeant3") == 0) {

    //get material ID for customs settings
    std::cout<<"PndTpcDetector::SetSpecialPhysicsCuts() "
	     <<"Working on medium "<<fMixture.c_str()<<std::endl;
    int matIdVMC = gGeoManager->GetMedium(fMixture.c_str())->GetId();
    
    double tofmax = 1.E10;    // (s)
    
    // Set new properties, physics cuts etc. for the TPCmixture
    
    
    //gMC->Gstpar(matIdVMC,"PAIR",1); /** pair production*/
    //gMC->Gstpar(matIdVMC,"COMP",1); /**Compton scattering*/
    //gMC->Gstpar(matIdVMC,"PHOT",1); /** photo electric effect */
    //gMC->Gstpar(matIdVMC,"PFIS",0); /**photofission*/
    //gMC->Gstpar(matIdVMC,"DRAY",1); /**delta-ray*/
    //gMC->Gstpar(matIdVMC,"ANNI",1); /**annihilation*/
    //gMC->Gstpar(matIdVMC,"BREM",1); /**bremsstrahlung*/
    //gMC->Gstpar(matIdVMC,"HADR",1); /**hadronic process*/
    //gMC->Gstpar(matIdVMC,"MUNU",1); /**muon nuclear interaction*/
    //gMC->Gstpar(matIdVMC,"DCAY",1); /**decay*/
    //gMC->Gstpar(matIdVMC,"LOSS",1); /**energy loss*/
    //gMC->Gstpar(matIdVMC,"MULS",1); /**multiple scattering*/
    //gMC->Gstpar(matIdVMC,"STRA",0); 
    //gMC->Gstpar(matIdVMC,"RAYL",1);
    
    //gMC->Gstpar(matIdVMC,"CUTGAM",fCut_el); /** gammas (GeV)*/
    //gMC->Gstpar(matIdVMC,"CUTELE",fCut_el); /** electrons (GeV)*/
    //gMC->Gstpar(matIdVMC,"CUTNEU",fCut_had); /** neutral hadrons (GeV)*/
    //gMC->Gstpar(matIdVMC,"CUTHAD",fCut_had); /** charged hadrons (GeV)*/
    //gMC->Gstpar(matIdVMC,"CUTMUO",fCut_el); /** muons (GeV)*/
    //gMC->Gstpar(matIdVMC,"BCUTE",fCut_el);  /** electron bremsstrahlung (GeV)*/
    //gMC->Gstpar(matIdVMC,"BCUTM",fCut_el);  /** muon and hadron bremsstrahlung(GeV)*/ 
    //gMC->Gstpar(matIdVMC,"DCUTE",fCut_el);  /** delta-rays by electrons (GeV)*/
    //gMC->Gstpar(matIdVMC,"DCUTM",fCut_el);  /** delta-rays by muons (GeV)*/
    //gMC->Gstpar(matIdVMC,"PPCUTM",fCut_el); /** direct pair production by muons (GeV)*/
     gMC->SetProcess("PAIR",1); /** pair production*/
     gMC->SetProcess("COMP",1); /**Compton scattering*/
     gMC->SetProcess("PHOT",1); /** photo electric effect */
     gMC->SetProcess("PFIS",0); /**photofission*/
     gMC->SetProcess("DRAY",1); /**delta-ray*/
     gMC->SetProcess("ANNI",1); /**annihilation*/
     gMC->SetProcess("BREM",1); /**bremsstrahlung*/
     gMC->SetProcess("HADR",1); /**hadronic process*/
     gMC->SetProcess("MUNU",1); /**muon nuclear interaction*/
     gMC->SetProcess("DCAY",1); /**decay*/
     gMC->SetProcess("LOSS",1); /**energy loss*/
     gMC->SetProcess("MULS",1); /**multiple scattering*/
     
     Double_t cut1 = 1.0E-3;         // GeV --> 1 MeV
     Double_t cutel = 1.0E-3;
     
     gMC->SetCut("CUTGAM",cutel);   /** gammas (GeV)*/
     gMC->SetCut("CUTELE",cutel);   /** electrons (GeV)*/
     gMC->SetCut("CUTNEU",cut1);   /** neutral hadrons (GeV)*/
     gMC->SetCut("CUTHAD",cut1);   /** charged hadrons (GeV)*/
     gMC->SetCut("CUTMUO",cut1);   /** muons (GeV)*/
     gMC->SetCut("BCUTE",cutel);    /** electron bremsstrahlung (GeV)*/
     gMC->SetCut("BCUTM",cut1);    /** muon and hadron bremsstrahlung(GeV)*/ 
     gMC->SetCut("DCUTE",cutel);    /** delta-rays by electrons (GeV)*/
     gMC->SetCut("DCUTM",cut1);    /** delta-rays by muons (GeV)*/
     gMC->SetCut("PPCUTM",cut1);   /** direct pair production by muons (GeV)*/
     gMC->SetCut("TOFMAX",tofmax); /**time of flight cut in seconds*/
          
      
     gMC->SetMaxNStep((int)1E6);
    
    std::cout<<"\n************************************************************\n"
	     <<"PndTpcDetector::SetSpecialPhysicsCuts():\n"
	     <<"   using special physics cuts ...\n";
    if(fAliMC) {
      std::cout<<"   using LOSS=5 for ALICE MC model\n";
      gMC->Gstpar(matIdVMC,"LOSS",5); /**energy loss*/
    }
    std::cout<<"************************************************************"
	     <<std::endl;

  }
}

Bool_t 
PndTpcDetector::ProcessHits( FairVolume *v)
{
  Double_t q= gMC->TrackCharge();
  if(q==0) {
    //std::cout<<"\nPndTpcDetector::ProcessHits: EXIT q==0"<<std::endl;
    return kTRUE;
  }
  // create Hit for every MC step where energy is deposited
  Double_t eLoss = gMC->Edep();
  if(eLoss<=0) {
    //std::cout<<"\nPndTpcDetector::ProcessHits: EXIT eLoss<=0"<<std::endl;
    return kTRUE;	
  }
  Double_t time   = gMC->TrackTime() * 1.0e09;
  Double_t length = gMC->TrackStep();
  TLorentzVector pos;
  gMC->TrackPosition(pos);
  TLorentzVector mom;
  gMC->TrackMomentum(mom);
  
  
  if(fAliMC) {
    if(mom.Rho()<=1e-12) { //keep ALICE code from dividing by zero
      std::cout<<"\n*** mom.Rho()="<<mom.Rho()<<": protect ALICE mode from low momentum, skipping"
	       <<std::endl;
      return kTRUE;
    }
  }

  Int_t trackID  = gMC->GetStack()->GetCurrentTrackNumber();
  Int_t volumeID = v->getMCid();
  
  TString volumeName = v->GetName();
  if(fAliMC && volumeName.Contains("gas"))	{
  	AliTPCv3_SetStepToNextCollision(); 
  }
    
  if(fDeltaAttach) {   //TODO: implement, this is just junk atm
    
    //PROBLEMS so far:
    // - how are we looping through the steps? sorted by particle type or primary/sec ... ??
    // - what happens in the ClusterizerTask
    
    //if this is a low momentum particle (e.g. delta) assign mother id
    //if(mom.E()<10.*1E-6){ // E<10keV
  }

  TParticle* mother=gMC->GetStack()->GetCurrentTrack();
  while(!mother->IsPrimary()){
    trackID=mother->GetFirstMother();
    mother=dynamic_cast<PndStack*>(gMC->GetStack())->GetParticle(trackID);
    //std::cout<<"Fetching mother id="<<trackID<<std::endl;
  }
  
  
  //gotta love TClonesArray syntax!
  PndTpcPoint* p=AddHit(trackID, volumeID, pos.Vect(), mom.Vect(), 
			time, length, eLoss);
  
  return kTRUE;
}



//copy & paste from AliTPCv3.cxx
void PndTpcDetector::AliTPCv3_SetStepToNextCollision()
{
  const Float_t prim = 14.35; //number of primary collisions per 1 cm for MIPs

  Double_t charge= gMC->TrackCharge();
  Float_t pp;
  TLorentzVector mom;
  gMC->TrackMomentum(mom);
  Float_t ptot=mom.Rho();
  Float_t beta_gamma = ptot/gMC->TrackMass();
  

  /*gMC->IdFromPDG(gMC->TrackPid()) <= 3 : electron(3), positron(2), photon(1)
                                           unused(0)  */

  if(gMC->IdFromPDG(gMC->TrackPid()) <= 3 && ptot > 0.02)  //typo in Alice File ??
    { 
      pp = prim*1.58; // electrons above 20 MeV/c are on the plateau!
    }
  else
    {
      pp=prim*AliTPCv3_BetheBloch(beta_gamma); 
      //assuming form of Bethe Bloch depends only on mean free path
      if(TMath::Abs(charge) > 1.) pp *= (charge*charge);
    }
  
  //Float_t random[1];
  TRandom * rGenerator=gMC->GetRandom();
  Double_t rnd=rGenerator->Rndm();
  
  gMC->SetMaxStep(-TMath::Log(rnd)/pp); 
  //get random free mean path from poisson statistics and mean pp
                                        
}

 Float_t PndTpcDetector::AliTPCv3_BetheBloch(Float_t bg)
{
  //
  // Bethe-Bloch energy loss formula
  //
  const Double_t p1=0.76176e-1;
  const Double_t p2=10.632;
  const Double_t p3=0.13279e-4;
  const Double_t p4=1.8631;
  const Double_t p5=1.9479;

  Double_t dbg = (Double_t) bg;

  Double_t beta = dbg/TMath::Sqrt(1.+dbg*dbg);

  Double_t aa = TMath::Power(beta,p4);
  Double_t bb = TMath::Power(1./dbg,p5);

  bb=TMath::Log(p3+bb);
  
  return ((Float_t)((p2-aa-bb)*p1/aa));
}


//IS NEVER CALLED 
// Float_t PndTpcDetector::AliTPCv3_InitDetector()
// {
// 	//
// 	// Initialises the TPC after that it has been built
// 	//
// 	FairGeoLoader*geoLoad = FairGeoLoader::Instance();
// 	FairGeoInterface *geoFace = geoLoad->getGeoInterface();
// 	FairGeoMedia *Media =  geoFace->getMedia();
	
// 	FairGeoMedium *TPCmixture  = Media->getMedium("TPCmixture");
// 	Int_t mediumId=TPCmixture->getMediumIndex();
// 	std::cout << "mediumId: " << mediumId << std::endl;
// 	gMC->Gstpar(mediumId,"LOSS",5);	//5 -> new geant3 option for
// 					//ALICE TPC see gphys/gfluct.F
//}

TClonesArray* PndTpcDetector::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fPndTpcPointCollection;
  else return NULL;
}


PndTpcPoint* PndTpcDetector::AddHit(Int_t trackID, Int_t detID, TVector3 pos,
    TVector3 mom, Double_t time, Double_t length,
    Double_t eLoss) {
  TClonesArray& clref = *fPndTpcPointCollection;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndTpcPoint(trackID, detID, pos, mom,
      time, length, eLoss);
}


void PndTpcDetector::BeginEvent() {
  //std::cout<<"PndTpcDetector::BeginEvent()"<<std::endl;
  fPndTpcPointCollection->Delete();
}

void PndTpcDetector::Reset() {
  
  //std::cout<<"PndTpcDetector::Reset()"<<std::endl;
  fPndTpcPointCollection->Delete();
  //ResetParameters();
}


void 
PndTpcDetector::ConstructGeometry() {
  /** If you are using the standard ASCII input for the geometry just
      copy this and use it for your detector, otherwise you can
      
  implement here you own way of constructing the geometry. */
  TString fileName = GetGeometryFileName();
  if(fileName.EndsWith(".root")) {
    std::cout<<"PndTpcDetector::ConstructGeometry() "
	     <<"  ...using ROOT geometry"<<std::endl;
    ConstructRootGeometry();
    return;
  }
  
  std::cout<<" --- Building TPC Geometry ---"<<std::endl;

  
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  PndTpcGeo*       Geo  = new PndTpcGeo();
  Geo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(Geo);

  Bool_t rc = geoFace->readSet(Geo);
  if (rc) Geo->create(geoLoad->getGeoBuilder());
  else std::cerr<<"PndTpcDetector:: geometry could not be read!"<<std::endl;

  TList* volList = Geo->getListOfVolumes();

  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  PndTpcGeoPar* par=(PndTpcGeoPar*)(rtdb->getContainer("PndTpcGeoPar"));
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
  //if(fAliMC)
  // AliTPCv3_InitDetector(); 
}


bool
PndTpcDetector::CheckIfSensitive(std::string name) {
  TString test(name.c_str());
  //IMPORTANT:
  //THIS REQUIRES THE ACTIVE VOLUME IN THE ROOT-GEOMETRY FILE TO CONTAIN
  //THE STRING "gas"
  if(test.Contains("gas")) {
    std::cout<<test<<" was set active"<<std::endl;
    TString meh = gGeoManager->GetVolume(name.c_str())->GetMaterial()->GetName();
    std::cout<<"Set active mixture to "<<meh<<std::endl;
    fMixture = meh.Data();
    return true;
  }

  if(fAllSensitive) 
    return true;
  
  return false;
}



ClassImp(PndTpcDetector)
