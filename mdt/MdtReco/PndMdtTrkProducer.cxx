// -------------------------------------------------------------------------
// -----                PndMdtTrkProducer source file                  -----
// -----                  Created 16/06/08  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndMdtTrkProducer.h"

#include "PndMdtTrk.h"
#include "PndMdtHit.h"
#include "PndMdtPoint.h"

#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TVector3.h"
#include "TMath.h"
#include "TClonesArray.h"
#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMdtTrkProducer::PndMdtTrkProducer() :
  FairTask(" MDT Tracklet Producer") { 
  fUseSimulation = kTRUE;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMdtTrkProducer::~PndMdtTrkProducer() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndMdtTrkProducer::Init() {
  
  cout << "-I- PndMdtTrkProducer::Init: "
       << "INITIALIZATION *********************" << endl;
  
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
    
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndMdtTrkProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }
  
  if (fUseSimulation) // use MC for extrapolation
    {
      fPointArray = (TClonesArray*) ioman->GetObject("MdtPoint");
      if ( ! fPointArray ) {
	cout << "-W- PndMdtTrkProducer::Init: "
	     << "No MdtPoint array!" << endl;
	return kERROR;
      }
    }
  else
    {
      cout << "-W- PndMdtTrkProducer::Init: "
	   << "At the moment only Simulation Mode available. Please switch simulation mode ON" << endl;
      return kERROR;
    }
  
  fHitArray = (TClonesArray*) ioman->GetObject("MdtHit");
  if ( ! fHitArray ) {
    cout << "-W- PndMdtTrkProducer::Init: "
	 << "No MdtHit array!" << endl;
    return kERROR;
  }
  
  // Create and register output array
  fTrkArray = new TClonesArray("PndMdtTrk");
  
  ioman->Register("MdtTrk","Mdt",fTrkArray,kTRUE);
  
  cout << "-I- PndMdtTrkProducer: Intialization successfull" << endl;
  
  return kSUCCESS;
  
}
// -------------------------------------------------------------------------

//______________________________________________________
void PndMdtTrkProducer::SetParContainers() {
  
  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("PndMdtTrkProducer:: SetParContainers", "No analysis run");
  
  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("PndMdtTrkProducer:: SetParContainers", "No runtime database");

  // Get Mdt Reconstruction parameter container
  fRecoPar = (PndMdtRecoPar*) db->getContainer("PndMdtRecoPar");
  
}

// -----   Public method Exec   --------------------------------------------
void PndMdtTrkProducer::Exec(Option_t* opt) {
  
  // Reset output array
  if ( ! fTrkArray ) Fatal("Exec", "No TrkArray");
  
  fTrkArray->Clear();
  
  Int_t nHits = fHitArray->GetEntriesFast();
  if (nHits==0) return; // exit if the event contains no Mdt hits
  
  PndMdtHit    *mdtHit0 = NULL;
  PndMdtPoint  *mdtPoint0 = NULL;
  
  // Loop over layer 0
  for (Int_t iHit0=0; iHit0<nHits; iHit0++) {  
    mdtHit0  = (PndMdtHit*) fHitArray->At(iHit0);
    if (mdtHit0->GetLayerID()!=0) continue; // select only inner hits
    mdtPoint0 = (PndMdtPoint*)fPointArray->At(mdtHit0->GetRefIndex());

    Int_t mdtCount = 1;
    Int_t mdtLayerCount = 1;
    PndMdtTrk *mdtTrk = new PndMdtTrk();

    // Set the first hit of layer0
    mdtTrk->SetHitNumber(0, iHit0);
    mdtTrk->SetModule(mdtHit0->GetModule());
    
    TVector3 pos0(0.,0.,0.);
    mdtHit0->Position(pos0);
    TVector3 mom0(mdtPoint0->GetPx(),mdtPoint0->GetPy(),mdtPoint0->GetPz());
    
    if (mdtHit0->GetModule()==1) 
      { // select barrel
	// transformation in local coordinates
	Float_t xt0, yt0;
	xt0 = (cos(-mdtHit0->GetSector()*45*TMath::DegToRad())*pos0.X() - sin(-mdtHit0->GetSector()*45*TMath::DegToRad())*pos0.Y());
	yt0 =  sin(-mdtHit0->GetSector()*45*TMath::DegToRad())*pos0.X() +  cos(-mdtHit0->GetSector()*45*TMath::DegToRad())*pos0.Y();
	mom0.SetPhi(mom0.Phi()-mdtHit0->GetSector()*45.*TMath::DegToRad());
	if (mom0.X()==0)
	  {
	    cout << "PndMdtTrkProducer:: Exec: Px == 0 error"<< endl;
	    continue;
	  }
	Float_t xlayer0 = -fRecoPar->GetLayerPos(mdtHit0->GetModule()-1,0);
	 
	for (Int_t iHit=0; iHit<nHits; iHit++) {
	    
	  PndMdtHit *mdtHit  = (PndMdtHit*) fHitArray->At(iHit);
	  if (mdtHit->GetLayerID()==0) continue; // skip inner hits
	  
	  Float_t xlayer = -fRecoPar->GetLayerPos(mdtHit0->GetModule()-1,mdtHit->GetLayerID());
	  Float_t yproj = yt0 + (xlayer - xt0) * mom0.Y() / mom0.X();
	  Float_t zproj = pos0.Z() + (xlayer - xt0) * mom0.Z() / mom0.X(); 
	  
	  TVector3 pos(0.,0.,0.);
	  mdtHit->Position(pos);
	  Float_t xt = (cos(-mdtHit0->GetSector()*45*TMath::DegToRad())*pos.X() - sin(-mdtHit0->GetSector()*45*TMath::DegToRad())*pos.Y());
	  Float_t yt =  sin(-mdtHit0->GetSector()*45*TMath::DegToRad())*pos.X() +  cos(-mdtHit0->GetSector()*45*TMath::DegToRad())*pos.Y();
	  //Float_t dist = sqrt(((yproj[yy]-yt0)*(yproj[yy]-yt0))+((zproj[yy]-hpos.Z())*(zproj[yy]-hpos.Z())));
	  TVector3 vext(xlayer-xlayer0, yproj-yt0, zproj-pos0.Z());
	  TVector3 vhit(xlayer-xlayer0, yt-yt0   , pos.Z()-pos0.Z());
	  Float_t angle = vext.Angle(vhit);

	  if (((mdtHit->GetLayerID()!=1)&&(angle>(fRecoPar->GetAngleCut(mdtHit0->GetModule()-1)*TMath::DegToRad()))) ||
	      ((mdtHit->GetLayerID()==1)&&(angle>(fRecoPar->GetAngleCut(mdtHit0->GetModule()-1)*TMath::DegToRad()*1.5)))) 
	    continue; // hit too far
	  
	  if (mdtTrk->GetHitBit(mdtHit->GetLayerID())==0) // if the layer is not used
	    {
	      mdtTrk->SetHitNumber(mdtHit->GetLayerID(), iHit);
	      mdtTrk->SetHitMult  (mdtHit->GetLayerID(), mdtTrk->GetHitMult(mdtHit->GetLayerID())+1); 
	      mdtTrk->SetHitAngle (mdtHit->GetLayerID(), angle);
	      mdtCount++;
	      mdtLayerCount++;
	    }
	  else
	    {
	      mdtTrk->SetHitMult  (mdtHit->GetLayerID(), mdtTrk->GetHitMult(mdtHit->GetLayerID())+1); 
	      mdtCount++;
	      if (mdtTrk->GetHitAngle(mdtHit->GetLayerID())> angle)
		{
		  mdtTrk->SetHitNumber(mdtHit->GetLayerID(), iHit);
		  mdtTrk->SetHitAngle (mdtHit->GetLayerID(), angle);
		}
	    }
	   
	} // end of outer layer loop
      } // end of barrel loop

    mdtTrk->SetHitCount(mdtCount);
    mdtTrk->SetLayerCount(mdtLayerCount);
    
    if (mdtLayerCount>1)  AddTrk(mdtTrk);
  } // loop over layer0
  
   
}
// -------------------------------------------------------------------------

// -----   Private method AddTrk   --------------------------------------------
PndMdtTrk* PndMdtTrkProducer::AddTrk(PndMdtTrk* track) {
  // Creates a new hit in the TClonesArray.
  
  Float_t chi2=0;
  for (Int_t ll=1; ll<10; ll++)
    {
      if (track->GetHitBit(ll)==0) continue;
      if (ll==1)
	chi2 = chi2 + (track->GetHitAngle(ll)/1.5)*(track->GetHitAngle(ll)/1.5); // manual correction for the first layer
      else
	chi2 = chi2 + track->GetHitAngle(ll)*track->GetHitAngle(ll);
    }
  track->SetChi2(chi2);
  
  TClonesArray& trkRef = *fTrkArray;
  Int_t size = trkRef.GetEntriesFast();
  return new(trkRef[size]) PndMdtTrk(*track);
}
// ----


ClassImp(PndMdtTrkProducer)
