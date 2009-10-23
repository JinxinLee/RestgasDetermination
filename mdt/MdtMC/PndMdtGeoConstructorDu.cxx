#include <iostream>

#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TParticle.h"
#include "TGeoManager.h"
#include "TVirtualMC.h"

#include "FairVolume.h"
// add on for debug
//#include "FairGeoG3Builder.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairModule.h"

#include "PndDetectorList.h"
#include "PndStack.h"
#include "PndMdt.h"

using namespace std;


// -----   Public method ConstructGeometry   ----------------------------------
void PndMdt::ConstructGeometryDu() 
{
  TString fileName=GetGeometryFileName();
  if(fileName.EndsWith(".root"))
    {
      ConstructRootGeometry();
    }
  else
    {
      std::cout<< "Geometry format not supported " <<std::endl;
    }
}

// ----------------------------------------------------------------------------

Bool_t PndMdt::CheckIfSensitiveDu(std::string name)
{
  
  if (name.find("MDT") != std::string::npos) 
    {
      //std::cout << name << std::endl;
      return kTRUE;
    }
  return kFALSE;
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t PndMdt::ProcessHitsDu(FairVolume* vol) 
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
  
  if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared() )
    {
      Int_t TrNo=gMC->GetStack()->GetCurrentTrackNumber();
      Int_t pdg= gMC->TrackPid();
      if ( TrNo == fTrkIn )
	{
	  TLorentzVector lPos, lMom;
	  Int_t iMod;
	  Int_t iOct;
	  Int_t iLayer;
	  Int_t iBox;
	  Int_t iWire;
	  sscanf(name,"MDT%is%il%ib%iw%i", &iMod, &iOct, &iLayer, &iBox, &iWire);
	      
	  Int_t detectorId = 0; 
	  gMC->TrackPosition(lPos); // cm
	  gMC->TrackMomentum(lMom); // GeV
	  TClonesArray& clref = *fMdtCollection;
	  Int_t size = fMdtCollection->GetEntriesFast();
	  PndMdtPoint *P= new(clref[size]) PndMdtPoint (TrNo,detectorId, lPos.Vect(), lMom.Vect(), gMC->TrackTime(),
							gMC->TrackLength(), gMC->Edep(), gMC->GetStack()->GetCurrentParentTrackNumber(),pdg,
							fPos_In.Vect(), fMom_In.Vect());
	  /**if you add a point then tell the stack! here*/
	  PndStack* stack = (PndStack*) gMC->GetStack();
	  stack->AddPoint(kMDT);
	};
      
      ResetParameters();
    };
  
  return kTRUE;
}
// ----------------------------------------------------------------------------

ClassImp(PndMdt)
