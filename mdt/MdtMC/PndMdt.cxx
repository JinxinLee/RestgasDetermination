#include <iostream>

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TFile.h"
#include "TSystem.h"
#include "TParticle.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoArb8.h"
#include "TGeoTrd2.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TVirtualMC.h"

#include "FairVolume.h"
// add on for debug
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairModule.h"
#include "PndDetectorList.h"
#include "PndStack.h"

#include "PndMdt.h"


using namespace std;

// -----   Default constructor   -------------------------------------------
PndMdt::PndMdt() 
  :  FairDetector(), fMdtCollection(0), fTrkIn(-1), 
     fBarrel(""), fEndcap(""), fMuonFilter(""), fForward(""),
     mdtMagnet(0), mdtMFI(0), mdtCoil(0), fVerboseLevel(0),
     fELoss(0), fPos_In(), fMom_In(), fTime(0)
{
    fMdtCollection        = new TClonesArray("PndMdtPoint");
}
// -------------------------------------------------------------------------


// -----   Inherited constructor   -----------------------------------------
PndMdt::PndMdt(const char* name, Bool_t active)
  : FairDetector(name,active), fMdtCollection(0), fTrkIn(-1), 
     fBarrel(""), fEndcap(""), fMuonFilter(""), fForward(""),
     mdtMagnet(0), mdtMFI(0), mdtCoil(0), fVerboseLevel(0),
     fELoss(0), fPos_In(), fMom_In(), fTime(0)
{
    fMdtCollection        = new TClonesArray("PndMdtPoint");
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMdt::~PndMdt() 
{
    if (fMdtCollection) {
	fMdtCollection->Delete();
	delete fMdtCollection;
    };
}
// -------------------------------------------------------------------------


// -----   Public method Print   ----------------------------------------------
void PndMdt::Print() const 
{
    Int_t nHits = fMdtCollection->GetEntriesFast();
    for (Int_t i=0; i<nHits; i++) (*fMdtCollection)[i]->Print();
}
// ----------------------------------------------------------------------------


// -----   Public method Reset   ----------------------------------------------
void PndMdt::Reset() 
{
  if (fMdtCollection) fMdtCollection->Delete(); 
}
// ----------------------------------------------------------------------------

// -----   Public method ResetParameters   ------------------------------------
void PndMdt::ResetParameters() 
{
    fELoss = 0.; 
    fTime = 0.;
}
// ----------------------------------------------------------------------------

// -----   Public method ConstructGeometry   ----------------------------------
void PndMdt::ConstructGeometry() 
{
  TString sysFile = gSystem->Getenv("VMCWORKDIR");
  if (fBarrel!="")
    {
      if (fBarrel=="fast" || fBarrel =="Fast")
        {
	  ConstructGeometryFast();
	}
      else if (fBarrel.EndsWith(".root"))
	{
	  SetGeometryFileName(fBarrel);
	  ConstructRootGeometry();
	}
      else
	{
	  std::cout<< "PndMdt::ConstructGeometry : No good MDT Barrel definition " <<std::endl;
	  exit(0);
	}
    }
 
  if (fEndcap!="")
    {
      if (fEndcap.EndsWith(".root"))
	{
	  SetGeometryFileName(fEndcap);
	  ConstructRootGeometry();
	}
      else if (fBarrel!="fast" && fBarrel !="Fast")
	{
	  std::cout<< "PndMdt::ConstructGeometry : No good MDT Endcap definition " <<std::endl;
	  exit(0);
	}
    }
 
  if (fMuonFilter!="")
    {
      if (fMuonFilter=="fast" || fMuonFilter=="Fast")
	{
	  PndMdtMuonFilter();
	}
      else if (fMuonFilter.EndsWith(".root"))
	{
	  SetGeometryFileName(fMuonFilter);
	  ConstructRootGeometry();
	}
      else 
	{
	  std::cout<< "PndMdt::ConstructGeometry : No good MDT Muon Filter definition " <<std::endl;
	  exit(0);
	}
    }

  if (fForward!="")
    {
      if (fForward=="fast" || fForward =="Fast")
	{
	  PndMdtForward(); 
	}
      else if (fForward.EndsWith(".root"))
	{
	  SetGeometryFileName(fForward);
	  ConstructRootGeometry();
	}
      else
	{
	  std::cout<< "PndMdt::ConstructGeometry : No good MDT Forward definition " <<std::endl;
	  exit(0);
	}
    }
  
  if(mdtMagnet) PndMdtMagnet();
  if(mdtMFI) PndMdtMFIron();
  if(mdtCoil) PndMdtCoil();
 
  return;
}
// ----------------------------------------------------------------------------


// -----   Public method Intialize   ---------------------------------------
void PndMdt::Initialize() 
{
  FairDetector::Initialize();
  FairRun* sim = FairRun::Instance();
  FairRuntimeDb* rtdb=sim->GetRuntimeDb();
  //PndGeoMdtPar* par=(PndGeoMdtPar*)(rtdb->getContainer("PndGeoMdtPar"));
  
  //TObjArray *fSensNodes = par->GetSensitiveNodes();
}
// -------------------------------------------------------------------------


// -----   Public method BeginEvent   --------------------------------------
void PndMdt::BeginEvent()
{
}
// -------------------------------------------------------------------------


// -----   Public method ProcessHits  --------------------------------------
Bool_t PndMdt::ProcessHits(FairVolume* vol) 
{
  TString name = gMC->CurrentVolOffName(1);
  if (name.Contains("BA")) ProcessHitsRoot(vol);
  else ProcessHitsFast(vol);
  
  // if (gMC->IsTrackEntering() || gMC->IsNewTrack() )
  return kTRUE;
}

// -----   Public method ProcessHitsFast  --------------------------------------
Bool_t PndMdt::ProcessHitsFast(FairVolume* vol) 
{
  if (gMC->TrackCharge()==0) return kTRUE; // skip neutrals

  TString name = vol->GetName();
 
  if (gMC->IsTrackEntering() || gMC->IsNewTrack() )
    {
      fPos_In.SetXYZM(0.,0.,0.,0.);
      fMom_In.SetXYZM(0.,0.,0.,0.);
      gMC->TrackPosition(fPos_In);
      gMC->TrackMomentum(fMom_In);
      fTrkIn = gMC->GetStack()->GetCurrentTrackNumber();
      fTime = gMC->TrackTime()*1e9;
    }; // end entering
  
  fELoss += gMC->Edep(); 
  
  if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared() )
    {
      Int_t TrNo=gMC->GetStack()->GetCurrentTrackNumber();
      Int_t pdg= gMC->TrackPid();
      if ( (TrNo == fTrkIn) && (fELoss >0.) )
	{
	  TLorentzVector lPos, lMom;
	  Int_t iMod;
	  Int_t iOct;
	  Int_t iLayer;
	  Int_t iBox;
	  Int_t iWire;
	  sscanf(name,"MDT%is%il%ib%iw%i", &iMod, &iOct, &iLayer, &iBox, &iWire);
	  
	  Int_t detectorId = iWire + 10*iBox + 1000*iLayer + 100000*iOct + 1000000*iMod; 
	  gMC->TrackPosition(lPos); // cm
	  gMC->TrackMomentum(lMom); // GeV
	  TClonesArray& clref = *fMdtCollection;
	  Int_t size = fMdtCollection->GetEntriesFast();
	  PndMdtPoint *P= new(clref[size]) PndMdtPoint (TrNo,detectorId, lPos.Vect(), lMom.Vect(), fTime,
							gMC->TrackLength(), fELoss, 
							fPos_In.Vect(), fMom_In.Vect());
	  /**if you add a point then tell the stack! here*/
	  PndStack* stack = (PndStack*) gMC->GetStack();
	  stack->AddPoint(kMDT);
	};
      
      ResetParameters();
    };
  
  return kTRUE;
}

// -----   Public method ProcessHitsRoot  --------------------------------------
Bool_t PndMdt::ProcessHitsRoot(FairVolume* vol) 
{
  if (gMC->TrackCharge()==0) return kTRUE; // skip neutrals
  
  TString name = gMC->CurrentVolName();
  TString path = gMC->CurrentVolPath();
  if (fVerboseLevel) std::cout << "Path: " << path << std::endl;
  
  if (gMC->IsTrackEntering() || gMC->IsNewTrack() )
    { 
      fELoss = 0.;
      gMC->TrackPosition(fPos_In);
      gMC->TrackMomentum(fMom_In);
      fTrkIn = gMC->GetStack()->GetCurrentTrackNumber();
      fTime = gMC->TrackTime()*1e9;
    }; // end entering
  
  fELoss += gMC->Edep();
  
  if (fVerboseLevel) cout << "pdg: " <<  gMC->TrackPid() << "\teloss: " << fELoss << endl;
  
  if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared() )
    {
      Int_t TrNo=gMC->GetStack()->GetCurrentTrackNumber();
      Int_t pdg= gMC->TrackPid();
      if ( (TrNo == fTrkIn) && (fELoss >0.) )
	{
	  TLorentzVector lPos, lMom;
	  Int_t iMod = -1;
	  Int_t iOct;
	  Int_t iLayer;
	  Int_t iBox;
	  Int_t iWire;
	  gMC->CurrentVolID(iWire);
	  gMC->CurrentVolOffID(2,iBox);
	  gMC->CurrentVolOffID(3,iLayer);
	  gMC->CurrentVolOffID(4,iOct);
	  if (path.Contains("Barrel")) iMod = 1;
	  if (path.Contains("Endcap")) iMod = 2;
	  if (path.Contains("MF"))     iMod = 3; 
	  if (path.Contains("Forward")) iMod = 4;
	  
	  if (fVerboseLevel) cout << iMod << "\t" <<  iOct << "\t" << iLayer << "\t" << iBox << "\t" << iWire;
	  Int_t detectorId = iWire + 10*iBox + 1000*iLayer + 100000*iOct + 1000000*iMod;
	  if (fVerboseLevel) cout << "\t" << detectorId << endl;
	  gMC->TrackPosition(lPos); // cm
	  gMC->TrackMomentum(lMom); // GeV
	  TClonesArray& clref = *fMdtCollection;
	  Int_t size = fMdtCollection->GetEntriesFast();
	  PndMdtPoint *P= new(clref[size]) PndMdtPoint (TrNo,detectorId, lPos.Vect(), lMom.Vect(), fTime,
							gMC->TrackLength(), fELoss, 
							fPos_In.Vect(), fMom_In.Vect());
	  /**if you add a point then tell the stack! here*/
	  PndStack* stack = (PndStack*) gMC->GetStack();
	  stack->AddPoint(kMDT);
	};
      
      ResetParameters();
    };
  
  return kTRUE;
}

// -------------------------------------------------------------------------
Bool_t PndMdt::CheckIfSensitive(std::string name)
{
  if (name.find("MDT") != std::string::npos)
    {
      return kTRUE;
    }
  
  return kFALSE;
}

// ----------------------------------------------------------------------------

TClonesArray* PndMdt::GetCollection(Int_t iColl) const 
{ 

  if(iColl==0) {
           return fMdtCollection; 
  }else{ 
     return NULL; 
  }

}


// -----   Public method EndOfEvent   -----------------------------------------
void PndMdt::EndOfEvent() 
{
  if (fVerboseLevel)  Print();
  Reset();
}
// ----------------------------------------------------------------------------


ClassImp(PndMdt)

