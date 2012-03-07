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

#include "TRandom.h"

#include "FairVolume.h"
// add on for debug
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairModule.h"
#include "PndDetectorList.h"
#include "PndStack.h"

#include "PndMdt.h"


using namespace std;

Int_t PndMdt::fTrkIn = -1;
TLorentzVector PndMdt::fPos_In;
TLorentzVector PndMdt::fMom_In;


// -----   Default constructor   -------------------------------------------
PndMdt::PndMdt() 
{
    fMdtCollection        = new TClonesArray("PndMdtPoint");
    fPosIndex   = 0;
    fTrkIn = -1;
    ResetParameters();
    SetVerbosity(kFALSE);
    fBarrel = "";
    fEndcap = "";
    fMuonFilter = "";
    fForward = "";
    mdtMagnet = kFALSE;
    mdtMFI = kFALSE;
    mdtCoil = kFALSE;
}
// -------------------------------------------------------------------------


// -----   Inherited constructor   -----------------------------------------
PndMdt::PndMdt(const char* name, Bool_t active) : FairDetector(name,active)
{
    fMdtCollection        = new TClonesArray("PndMdtPoint");
    fPosIndex   = 0;
    fTrkIn = -1;
    ResetParameters();
    SetVerbosity(kFALSE); 
    fBarrel = "";
    fEndcap = "";
    fMuonFilter = "";
    fForward = "";
    mdtMagnet = kFALSE;
    mdtMFI = kFALSE;
    mdtCoil = kFALSE;
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
   fMdtCollection->Delete(); 
   fPosIndex = 0;
}
// ----------------------------------------------------------------------------


// -----   Public method CopyClones   -----------------------------------------
void PndMdt::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset ) 
{
/*  Int_t nEntries = cl1->GetEntriesFast();
  TClonesArray& clref = *cl2;
  PndMdtPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (PndMdtPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) PndMdtPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- PndMdt: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
*/
}
// ----------------------------------------------------------------------------

// -----   Public method ResetParameters   ------------------------------------
void PndMdt::ResetParameters() 
{
  /*  fEventID = -999;
    fTrackID = -999;
    fTrackParentID = -999;
    fDetectorID = -999;
    fPDG = -999;
    */
    fELoss = 0.;
    fPos.SetXYZT(0., 0., 0., 0.);
    fMom.SetXYZT(0., 0., 0., 0.) ;
}
// ----------------------------------------------------------------------------


// -----   Public method SetParFile   --------------------------------------
void PndMdt::SetParFile(TString filename)
{
    ffn = filename;
}
// -------------------------------------------------------------------------

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
	  std::cout<< "PndMdt::ConstructGeometry : No Torino design for Forward MDT" <<std::endl;
	  exit(0); 
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
  //par=(PndGeoMdtPar*)(rtdb->getContainer("PndGeoMdtPar"));
  
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
  TString name = vol->GetName();
 
  if (gMC->IsTrackEntering() || gMC->IsNewTrack() )
    {
      fPos_In.SetXYZM(0.,0.,0.,0.);
      fMom_In.SetXYZM(0.,0.,0.,0.);
      gMC->TrackPosition(fPos_In);
      gMC->TrackMomentum(fMom_In);
      fTrkIn = gMC->GetStack()->GetCurrentTrackNumber();
    }; // end entering
  
  fELoss = fELoss + gMC->Edep(); 
  
  if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared() )
    {
      Int_t TrNo=gMC->GetStack()->GetCurrentTrackNumber();
      Int_t pdg= gMC->TrackPid();
      if ( (TrNo == fTrkIn) && (fELoss >0.) )
	{
//	  Float_t ran = gRandom->Uniform(0,1);
//	  cout << "ran= " << ran << endl;
	  TLorentzVector lPos, lMom;
	  Int_t iMod;
	  Int_t iOct;
	  Int_t iLayer;
	  Int_t iBox;
	  Int_t iWire;
	  sscanf(name,"MDT%is%il%ib%iw%i", &iMod, &iOct, &iLayer, &iBox, &iWire);

//          cout << "name: " << name << endl;
	      
	  Int_t detectorId = iWire + 10*iBox + 1000*iLayer + 100000*iOct + 1000000*iMod; 
//          cout << "name: " << name << "  iMod= " << iMod << " iOct= " << iOct << 
//               " iLAyer= " << iLayer << " iBox= " << iBox << endl;

	  gMC->TrackPosition(lPos); // cm
	  gMC->TrackMomentum(lMom); // GeV
	  TClonesArray& clref = *fMdtCollection;
	  Int_t size = fMdtCollection->GetEntriesFast();
	  PndMdtPoint *P= new(clref[size]) PndMdtPoint (TrNo,detectorId, lPos.Vect(), lMom.Vect(), gMC->TrackTime(),
							gMC->TrackLength(), fELoss, gMC->GetStack()->GetCurrentParentTrackNumber(),pdg,
							fPos_In.Vect(), fMom_In.Vect());
	  /**if you add a point then tell the stack! here*/
	  PndStack* stack = (PndStack*) gMC->GetStack();
	  stack->AddPoint(kMDT);
	};
      
      ResetParameters();
    };
  
  ResetParameters();
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

