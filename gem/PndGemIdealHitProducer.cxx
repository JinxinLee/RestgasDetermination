/////////////////////////////////////////////////////////////
//  PndGemIdealHitProducer
//  Filler of PndGemHit
///////////////////////////////////////////////////////////////// 

/** PndGemFindHits
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 15.02.2009
 *@version 1.0
 **
 ** PANDA task class for producing ideal PndGemHits
 ** Task level RECO
 **/

// Includes from GEM
#include "PndGemDigitize.h"

// Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Includes from ROOT
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TGeoNode.h"

#include "PndGemIdealHitProducer.h"

#include "PndGemMCPoint.h"
#include "PndGemHit.h"
#include "PndGemMCPoint.h"
#include "PndGemDigiPar.h"
#include "PndGemSensor.h"
#include "PndGemDigi.h"

#include "TRandom2.h"


// -----   Default constructor   -------------------------------------------
PndGemIdealHitProducer::PndGemIdealHitProducer() :
  FairTask("Ideal GEM hit Producer") { 
  fDigiPar     = NULL;
}

// -----   Constructor   ---------------------------------------------------
PndGemIdealHitProducer::PndGemIdealHitProducer(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose) { 
  fDigiPar     = NULL;
}


// -----   Destructor   ----------------------------------------------------
PndGemIdealHitProducer::~PndGemIdealHitProducer() {
  if(fHitArray){
    fHitArray->Delete();
    delete fHitArray;
  }
  if ( fDigiPar)   delete fDigiPar;
}

// -----   Private method SetParContainers   -------------------------------
void PndGemIdealHitProducer::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get GEM digitisation parameter container
  fDigiPar = (PndGemDigiPar*)(db->getContainer("PndGemDetectors"));

}
// -------------------------------------------------------------------------


// -----   Public method Init   --------------------------------------------
InitStatus PndGemIdealHitProducer::Init() {
 
  std::cout << " INITIALIZATION OF Ideal Gem Hit Producer***************" << std::endl;
  
  fGen = new TRandom3();
  time_t curtime;
  time(&curtime);
  fGen->SetSeed(curtime);

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if( !ioman ) {
    std::cout << "-E- PndGemIdealHitProducer::Init: "
	      << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  
  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("GEMPoint");
  if( !fPointArray ) {
    std::cout << "-W- PndGemIdealHitProducer::Init: "
	      << "Array of GEMPoints not found!" << std::endl;
    return kERROR;
  }
  
  // Create and register output array
  fHitArray = new TClonesArray("PndGemHit");
  
  ioman->Register("GEMHit","GEM ideal hit",fHitArray,kTRUE);
  
  std::cout << "-I- PndGemIdealHitProducer: Intialization successfull" << std::endl;
  return kSUCCESS;

}

// -----   Public method Exec   --------------------------------------------
void PndGemIdealHitProducer::Exec(Option_t* opt) {

  // Reset output array
  if( !fHitArray ) Fatal("Exec", "No hit array");

  PndGemSensor* sensor;

  Int_t nofHits = 0;
  fHitArray->Clear();
  Double_t dr, dp;
  Double_t radius, innerR;
  
  for ( Int_t iPoint = 0 ; iPoint < fPointArray->GetEntriesFast() ; iPoint++ ) {
    PndGemMCPoint* currentPndGemMCPoint = (PndGemMCPoint*)fPointArray->At(iPoint);
    
    Double_t posIn[3] = {currentPndGemMCPoint->GetX(),
 			 currentPndGemMCPoint->GetY(),
 			 currentPndGemMCPoint->GetZ()};
    
    TString nodeName = currentPndGemMCPoint->GetDetName();

    if ( !nodeName.Contains("_Gem") ) continue;

    gGeoManager->cd(nodeName.Data());
    TGeoNode* curNode = gGeoManager->GetCurrentNode();

    nodeName.Remove(0,nodeName.Last('/')+1);
    nodeName.Remove(nodeName.Length()-2,2);

    sensor = (PndGemSensor*)fDigiPar->GetSensorByName(nodeName.Data());

    Double_t locPosIn[4];

    curNode->MasterToLocal(posIn,locPosIn);

    radius = TMath::Sqrt(locPosIn[0]*locPosIn[0]+locPosIn[1]*locPosIn[1]);
    innerR = sensor->GetInnerRadius();
    if ( sensor->GetStripAngle(0) == 0. && sensor->GetStripAngle(1) == 90. ) {
      dp = sensor->GetPitch(0)*radius/innerR/TMath::Sqrt(12.);
      dr = sensor->GetPitch(1)/TMath::Sqrt(12.);
    }

    if ( sensor->GetStripAngle(0) == -sensor->GetStripAngle(1) ) {
      Double_t pitch = ( sensor->GetPitch(0)+sensor->GetPitch(1) ) / 2.;
      dp = pitch*radius/innerR/TMath::Sqrt(12.);
      Double_t cosAng = TMath::Cos(TMath::DegToRad()*sensor->GetStripAngle(0));
      Double_t xh = - innerR*cosAng + TMath::Sqrt(innerR*innerR*cosAng*cosAng-innerR*innerR+radius*radius);
      Double_t backAng = TMath::ACos((radius*radius+xh*xh-innerR*innerR)/(2.*radius*xh));
      dr = dp / TMath::Tan(backAng);
    }

    Double_t phiAValue = TMath::ATan(locPosIn[0]/locPosIn[1]);
    if ( locPosIn[1] < 0 ) phiAValue += TMath::Pi();
    else if ( locPosIn[0] < 0 ) phiAValue +=  2.*TMath::Pi();

    Double_t rSmear, pSmear;

    fGen->Rannor(rSmear,pSmear);
    rSmear = radius + rSmear*dr;
    pSmear = 0.     + pSmear*dp;

    if ( rSmear < innerR || rSmear > sensor->GetOuterRadius() ) rSmear = radius;
    
    locPosIn[0] = rSmear*TMath::Sin(phiAValue) + pSmear*TMath::Cos(phiAValue);
    locPosIn[1] = rSmear*TMath::Cos(phiAValue) - pSmear*TMath::Sin(phiAValue);
    
    TVector3 pos(locPosIn[0],locPosIn[1],sensor->GetZ0());
    
    Double_t sigma = dp;
    if ( dr > sigma ) sigma = dr;
    TVector3 dpos(sigma, sigma, sensor->GetD());

    new ((*fHitArray)[nofHits++]) PndGemHit(1,//sensor->GetDetectorId(),
					    (currentPndGemMCPoint->GetDetName()).Data(), 
					    pos, dpos,  
					    -1, -1, dr, dp, iPoint);
  }  // end of loop over Points
}

// -----   Private method Finish   -----------------------------------------
void PndGemIdealHitProducer::Finish() {
}

ClassImp(PndGemIdealHitProducer)
