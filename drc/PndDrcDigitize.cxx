//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemDigitize source file                 -----
// -----                  Created 12/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

// Includes from GEM
#include "PndDrcDigitize.h"

// Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLink.h"

// Includes from ROOT
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TGeoNode.h"

#include "PndDetectorList.h"
#include "PndDrcHit.h"
#include "PndDrcPDPoint.h"
#include "PndDrcDigiPar.h"
//#include "PndGemSensor.h"
#include "PndDrcDigi.h"

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
PndDrcDigitize::PndDrcDigitize() : FairTask("GEM Digitizer", 1) {
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  //fSaveOutsideHits = kFALSE;

  //fTNofEvents = 0;
  //fTNofPoints = 0;
  //fTNofDigis  = 0;

  Reset();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndDrcDigitize::PndDrcDigitize(Int_t iVerbose) 
  : FairTask("GEM Digitizer", iVerbose) { 
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  //fSaveOutsideHits = kFALSE;

  //fTNofEvents = 0;
  //fTNofPoints = 0;
  //fTNofDigis  = 0;

  Reset();
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
PndDrcDigitize::PndDrcDigitize(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose) { 
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  //fSaveOutsideHits = kFALSE;

  //fTNofEvents = 0;
  //fTNofPoints = 0;
  //fTNofDigis  = 0;

  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndDrcDigitize::~PndDrcDigitize() { 
  Reset();
  if ( fDigiPar)   delete fDigiPar;
  if ( fDigis ) {
    fDigis->Delete();
    delete fDigis;
  }
  if ( fDigiMatches ) {
    fDigiMatches->Delete();
    delete fDigiMatches;
  }

}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndDrcDigitize::Exec(Option_t* opt) {


  Reset();

 Int_t nofPoints = fPoints->GetEntriesFast();

  for ( Int_t iPoint = 0 ; iPoint < nofPoints ; iPoint++ ) 
    {
      PndDrcPDPoint* currentPndDrcPDPoint = (PndDrcPDPoint*)fPoints->At(iPoint);
      
     
      //Int_t iDigi = fChannelMap[a];
      //PndDrcDigi* ddigi = dynamic_cast<PndDrcDigi*>(fDigis->At(iDigi));
      //ddigi->AddIndex(iPoint);
    }
  
}
// -------------------------------------------------------------------------



// -----   Private method SetParContainers   -------------------------------
void PndDrcDigitize::SetParContainers() {
  
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get GEM digitisation parameter container
  fDigiPar = (PndDrcDigiPar*)(db->getContainer("PndDrcDetectors"));
}
// -------------------------------------------------------------------------



// -----   Private method Init   -------------------------------------------
InitStatus PndDrcDigitize::Init() {

  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fPoints = (TClonesArray*) ioman->GetObject("DrcPDPoint");

  // if ( fSaveOutsideHits ) {
  //fHitOutsideArray = new TClonesArray("PndDrcHit");
  //ioman->Register("GEMOutsideHit", "PndDrc Hits in inactive region",
  //	    fHitOutsideArray, kTRUE);
  //}

  // Register output array StsDigi
  fDigis = new TClonesArray("PndDrcDigi",1000);
  ioman->Register("GEMDigi", "Digital response in GEM", fDigis, kTRUE);
  cout << "-I- " << fName.Data() << "::Init(). Initialization succesfull." << endl;
  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Private method ReInit   -----------------------------------------
InitStatus PndDrcDigitize::ReInit() {

  // Clear digitisation scheme
  //  fDigiScheme->Clear();

  // Build new digitisation scheme
  //  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) return kSUCCESS;

  return kERROR;

}
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void PndDrcDigitize::Reset() {
  //fNPoints = fNFailed = fNOutside = fNMulti = fNDigis = 0;
  //fChannelMap.clear();
  if ( fDigis ) fDigis->Clear();
  if ( fDigiMatches ) fDigiMatches->Clear();
}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
void PndDrcDigitize::Finish() {
  if ( fDigis ) fDigis->Clear();
  cout << "PndDrcDigitize::Finish()  ---------" << endl; 
}
// -------------------------------------------------------------------------



ClassImp(PndDrcDigitize)

