//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemDigitize source file                 -----
// -----                  Created 30/08/06  by V. Friese               -----
// -------------------------------------------------------------------------

// Includes from STS
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

#include "PndGemHit.h"
#include "PndGemMCPoint.h"
#include "PndGemDigiPar.h"
#include "PndGemSensor.h"
#include "PndGemDigi.h"

#include <iostream>
#include <iomanip>
#include <map>

using std::cout;
using std::cerr;
using std::endl;
using std::pair;
using std::setw;
using std::left;
using std::right;
using std::fixed;
using std::setprecision;
using std::map;



// -----   Default constructor   ------------------------------------------
PndGemDigitize::PndGemDigitize() : FairTask("GEM Digitizer", 1) {
  //  fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  //  fDigiScheme  = new PndGemDigiScheme();
  // Create and register output array
  Reset();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndGemDigitize::PndGemDigitize(Int_t iVerbose) 
  : FairTask("GEM Digitizer", iVerbose) { 
  //  fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
   // fDigiScheme  = new PndGemDigiScheme();
  // Create and register output array
  Reset();
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
PndGemDigitize::PndGemDigitize(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose) { 
//   fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  //  fDigiScheme  = new PndGemDigiScheme();
  // Create and register output array
  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndGemDigitize::~PndGemDigitize() { 
  //  if ( fGeoPar)    delete fGeoPar;
  if ( fDigiPar)   delete fDigiPar;
  if ( fDigis ) {
    fDigis->Delete();
    delete fDigis;
  }
  if ( fDigiMatches ) {
    fDigiMatches->Delete();
    delete fDigiMatches;
  }
  //  if ( fDigiScheme ) delete fDigiScheme;
  Reset();
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndGemDigitize::Exec(Option_t* opt) {

//   cout << "************************************************************************************************************************************" << endl;
//   cout << "pixel sensor position = " << fDigiPar->GetPosition (0) << ", " << fDigiPar->GetPosition (1) << ", " << fDigiPar->GetPosition (2) << ", " 
//        << " nofsides = " << fDigiPar->GetNofSides   () << ", from " << fDigiPar->GetInnerRadius() << " to " << fDigiPar->GetOuterRadius() << " -- pixel " 
//        << fDigiPar->GetPixelSize(0) << " x " << fDigiPar->GetPixelSize(1) << " cm2" 
//        << endl;
//   cout << "************************************************************************************************************************************" << endl;
  if ( ! fHitOutputArray ) Fatal("Exec", "No fHitOutputArray");
  fHitOutputArray->Clear();
  Reset();
  //  if ( fDigis ) fDigis->Clear();
  //  fDigis->Clear();

  PndGemSensor* sensor;

  Int_t nofHitsOutside = 0;

  Double_t posCheck[3] = {20.,20.,80.};

  for ( Int_t iPoint=fPoints->GetEntriesFast()-1 ; iPoint >= 0 ; iPoint--) {
    PndGemMCPoint* currentPndGemMCPoint = (PndGemMCPoint*)fPoints->At(iPoint);
    
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

    if ( sensor->GetType()!=1 ) { locPosIn[3] = locPosIn[2]; locPosIn[2] = locPosIn[1]; locPosIn[1] = locPosIn[3]; locPosIn[0] = -locPosIn[0]; }

    /*    cout << "                         global (" 
	 << posIn[0] << ", "
	 << posIn[1] << ", "
	 << posIn[2] << ") cm to local coord: (" 
	 << locPosIn[0] << ", " 
	 << locPosIn[1] << ", " 
	 << locPosIn[2] << ") cm" << endl;*/
    
    Int_t channelNumber = sensor->GetChannel(locPosIn[0],locPosIn[1],0);
    if ( channelNumber == -1 ) {
      TVector3 pos;
      currentPndGemMCPoint->Position(pos);
      TVector3 dposLocal(0.,0.,0.);
      
      new ((*fHitOutputArray)[nofHitsOutside++]) PndGemHit(sensor->GetDetectorId(),
 							   (currentPndGemMCPoint->GetDetName()).Data(),
 							   pos,dposLocal,iPoint,currentPndGemMCPoint->GetEnergyLoss(),1);
    }
    else {
      new ((*fDigis)[fNDigis]) PndGemDigi(sensor->GetDetectorId(), 0, channelNumber);
      fNDigis++;   
    }
    
    channelNumber = sensor->GetChannel(locPosIn[0],locPosIn[1],1);
    if ( channelNumber == -1 ) continue;

    /*    cout << "creating digiB#" << fNDigis << " in sensor " << sensor->GetDetectorName() << " chann. " << channelNumber 
	 << " (" << posIn[0] << ", " << posIn[1] << ", " << posIn[2] << ") -- (" 
	 << locPosIn[0] << ", " 
	 << locPosIn[1] << ", " 
	 << locPosIn[2] << ") cm" << endl;*/
    new ((*fDigis)[fNDigis]) PndGemDigi(sensor->GetDetectorId()+1, 1, channelNumber);
    fNDigis++;   
  }
}
// -------------------------------------------------------------------------


 
// -----   Private method SetParContainers   -------------------------------
void PndGemDigitize::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get GEM digitisation parameter container
  fDigiPar = (PndGemDigiPar*)(db->getContainer("PndGemDetectors"));

  // Get STS geometry parameter container
  //  fGeoPar = (FairGeoStsPar*) db->getContainer("FairGeoGemPar");

  // Get STS digitisation parameter container
  //  fDigiPar = (PndGemDigiPar*) db->getContainer("PndGemDigiPar");
 
}
// -------------------------------------------------------------------------



// -----   Private method Init   -------------------------------------------
InitStatus PndGemDigitize::Init() {

  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fPoints = (TClonesArray*) ioman->GetObject("GEMPoint");

   fHitOutputArray = new TClonesArray("PndGemHit");
   ioman->Register("GEMOutsideHit", "PndGem Hits in inactive region",
 									fHitOutputArray, kTRUE);

  // Register output array StsDigi
   fDigis = new TClonesArray("PndGemDigi",1000);
   ioman->Register("GEMDigi", "Digital response in GEM", fDigis, kTRUE);

  // Register output array StsDigiMatches
//   fDigiMatches = new TClonesArray("PndGemDigiMatch",1000);
//   ioman->Register("STSDigiMatch", "Digi Match in STS", fDigiMatches, kTRUE);

  // Build digitisation scheme
  /*  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) {
    if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
    else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
    cout << "-I- " << fName << "::Init: "
	 << "STS digitisation scheme succesfully initialised" << endl;
    cout << "    Stations: " << fDigiScheme->GetNStations() 
	 << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: " 
	 << fDigiScheme->GetNChannels() << endl;
    return kSUCCESS;
  }
  
  return kERROR;*/

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemDigitize::ReInit() {

  // Clear digitisation scheme
  //  fDigiScheme->Clear();

  // Build new digitisation scheme
  //  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) return kSUCCESS;

  return kERROR;

}
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void PndGemDigitize::Reset() {
  fNPoints = fNFailed = fNOutside = fNMulti = fNDigis = 0;
  fChannelMap.clear();
  if ( fDigis ) fDigis->Clear();
  if ( fDigiMatches ) fDigiMatches->Clear();
}
// -------------------------------------------------------------------------





ClassImp(PndGemDigitize)

