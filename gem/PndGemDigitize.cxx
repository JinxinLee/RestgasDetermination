//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemDigitize source file                 -----
// -----                  Created 12/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

// Includes from GEM
#include "PndGemDigitize.h"

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
PndGemDigitize::PndGemDigitize() 
  : FairTask("GEM Digitizer", 1),
    fDigiPar(NULL),
    fPoints(NULL),
    fDigis(NULL),
    fDigiMatches(NULL),
    fNPoints(0),
    fNFailed(0),
    fNOutside(0),
    fNMulti(0),
    fNDigis(0),
    fTNofEvents(0),
    fTNofPoints(0),
    fTNofDigis(0),
    fHitOutsideArray(NULL),
    fSaveOutsideHits(kFALSE)
{
  Reset();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndGemDigitize::PndGemDigitize(Int_t iVerbose) 
  : FairTask("GEM Digitizer", iVerbose),
    fDigiPar(NULL),
    fPoints(NULL),
    fDigis(NULL),
    fDigiMatches(NULL),
    fNPoints(0),
    fNFailed(0),
    fNOutside(0),
    fNMulti(0),
    fNDigis(0),
    fTNofEvents(0),
    fTNofPoints(0),
    fTNofDigis(0),
    fHitOutsideArray(NULL),
    fSaveOutsideHits(kFALSE)
{ 
  Reset();
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
PndGemDigitize::PndGemDigitize(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose) { 
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  fSaveOutsideHits = kFALSE;

  fTNofEvents = 0;
  fTNofPoints = 0;
  fTNofDigis  = 0;

  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndGemDigitize::~PndGemDigitize() { 
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
void PndGemDigitize::Exec(Option_t* opt) {

  if ( fSaveOutsideHits ) {
    if ( ! fHitOutsideArray ) Fatal("Exec", "No fHitOutsideArray");
    fHitOutsideArray->Clear();
  }

  Reset();

  fTNofEvents++;

  PndGemSensor* sensor;

  Int_t nofHitsOutside = 0;
  Int_t nofPoints = fPoints->GetEntriesFast();

//   cout << "GEM digitize points:" << endl;
//   for ( Int_t iPoint = 0 ; iPoint < nofPoints ; iPoint++ ) {
//     PndGemMCPoint* currentPndGemMCPoint = (PndGemMCPoint*)fPoints->At(iPoint);
//     cout << iPoint << " : " 
// 	 << currentPndGemMCPoint->GetX() << " "
// 	 << currentPndGemMCPoint->GetY() << " "
// 	 << currentPndGemMCPoint->GetZ() << endl;
//   }
//   cout << "GEM digis: " << endl;

  for ( Int_t iPoint = 0 ; iPoint < nofPoints ; iPoint++ ) {
    PndGemMCPoint* currentPndGemMCPoint = (PndGemMCPoint*)fPoints->At(iPoint);

    Double_t posIn[3] = {currentPndGemMCPoint->GetX(),
 			 currentPndGemMCPoint->GetY(),
 			 currentPndGemMCPoint->GetZ()};
    
    Int_t sensorId = currentPndGemMCPoint->GetSensorId();

    TString nodeName = fDigiPar->GetNodeName(sensorId);

    gGeoManager->cd(nodeName.Data());
    TGeoNode* curNode = gGeoManager->GetCurrentNode();

//     cout << "id " << sensorId << " -> " 
// 	 << fDigiPar->GetStationNr(sensorId) << "."
// 	 << fDigiPar->GetSensorNr(sensorId) << "."
// 	 << fDigiPar->GetSegmentNr(sensorId) << " > " 
// 	 << nodeName.Data() << endl;

    sensor = (PndGemSensor*)fDigiPar->GetSensor(sensorId);
    if ( !sensor ) {
      cout << " -E- " << GetName() << ":Exec() There is no sensor: \"" 
	   << nodeName.Data() << "\"." << endl;
      continue;
    }
    Double_t locPosIn[4];

    fTNofPoints++;
    
    curNode->MasterToLocal(posIn,locPosIn);

    //    cout << "position:  " << posIn[0] << " " << posIn[1] << " " << posIn[2] << endl;
    //	 << " transf to " << locPosIn[0] << " " << locPosIn[1] << " " << locPosIn[2] << endl;
    
    //    if ( sensor->GetType()!=1 ) { locPosIn[3] = locPosIn[2]; locPosIn[2] = locPosIn[1]; locPosIn[1] = locPosIn[3]; locPosIn[0] = -locPosIn[0]; }
    
    Int_t sensorDetId = sensor->GetDetectorId();

    Int_t channelNumber = sensor->GetChannel(locPosIn[0],locPosIn[1],0);
    if ( fDigiPar->GetStationNr(sensorId) == 1 ) {
      //      cout << sensor->GetType() << ": for point " << locPosIn[0] << " " << locPosIn[1] << " ---> channel " << channelNumber << endl;
    }
    if ( channelNumber == -1 ) {
      if ( fSaveOutsideHits ) {
	TVector3 pos;
	currentPndGemMCPoint->Position(pos);
	TVector3 dposLocal(0.,0.,0.);

	Int_t hitDetId = sensorDetId | kGemHit << 21;
      
	new ((*fHitOutsideArray)[nofHitsOutside]) PndGemHit(sensorDetId,
							    pos,dposLocal,iPoint,currentPndGemMCPoint->GetEnergyLoss(),1);
      }
      nofHitsOutside++;
    }
    else {
      pair<Int_t, Int_t> a (sensorDetId, channelNumber);
      if ( fChannelMap.find(a) == fChannelMap.end() ) {
	// Channel not yet active, create new digi

	Int_t digiDetId = sensorDetId | kGemDigi << 21 | 0 << 5;
      
	//	cout << fNDigis << " : " << sensorDetId << " " << channelNumber << endl;

	new ((*fDigis)[fNDigis]) PndGemDigi(digiDetId, channelNumber, iPoint);
	fChannelMap[a] = fNDigis;
	fNDigis++;
	fTNofDigis++;
      }
      else {
	Int_t iDigi = fChannelMap[a];
	PndGemDigi* ddigi = dynamic_cast<PndGemDigi*>(fDigis->At(iDigi));
	//	cout << "another mc point to this digi, last one " << ddigi->GetIndex(ddigi->GetNIndices()-1) << ", now adding " << iPoint << endl;
	ddigi->AddIndex(iPoint);
      }
    }

    channelNumber = sensor->GetChannel(locPosIn[0],locPosIn[1],1);
    if ( channelNumber == -1 ) continue;

    pair<Int_t, Int_t> a (sensorDetId, channelNumber);
    if ( fChannelMap.find(a) == fChannelMap.end() ) {
      // Channel not yet active, create new digi

      Int_t digiDetId = sensorDetId | kGemDigi << 21 | 1 << 5;
      
      //      cout << fNDigis << " : " << sensorDetId << " " << channelNumber << endl;

      new ((*fDigis)[fNDigis]) PndGemDigi(digiDetId, channelNumber, iPoint);
      fChannelMap[a] = fNDigis;
      fNDigis++;
      fTNofDigis++;
    }
    else {
      Int_t iDigi = fChannelMap[a];
      PndGemDigi* ddigi = dynamic_cast<PndGemDigi*>(fDigis->At(iDigi));
      //      cout << "another mc point to this digi, last one " << ddigi->GetIndex(ddigi->GetNIndices()-1) << ", now adding " << iPoint << endl;
      ddigi->AddIndex(iPoint);
    }
  }

  if ( fVerbose ) 
    cout << "-I- PndGemDigitize::Exec() Created " << fNDigis << " digis from " << nofPoints << " points" << endl;
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
}
// -------------------------------------------------------------------------



// -----   Private method Init   -------------------------------------------
InitStatus PndGemDigitize::Init() {

  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fPoints = (TClonesArray*) ioman->GetObject("GEMPoint");

  if ( fSaveOutsideHits ) {
    fHitOutsideArray = new TClonesArray("PndGemHit");
    ioman->Register("GEMOutsideHit", "PndGem Hits in inactive region",
		    fHitOutsideArray, kTRUE);
  }

  // Register output array StsDigi
  fDigis = new TClonesArray("PndGemDigi",1000);
  ioman->Register("GEMDigi", "Digital response in GEM", fDigis, kTRUE);

  cout << "-I- " << fName.Data() << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations." << endl;
  cout << "-I- " << fName.Data() << "::Init(). Initialization succesfull." << endl;
  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemDigitize::ReInit() {
  // Clear digitisation scheme
  //  fDigiScheme->Clear();

  // Build new digitisation scheme
  //  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) return kSUCCESS;

  return kSUCCESS;

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

// -----   Public method Finish   ------------------------------------------
void PndGemDigitize::Finish() {
  if ( fDigis ) fDigis->Clear();

  cout << "-------------------- " << fName.Data() << " : Summary ------------------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents << endl;
  cout << " MC Points:     " << setw(10) << fTNofPoints << "    ( " << (Double_t)fTNofPoints/((Double_t)fTNofEvents) << " per event )" << endl;
  cout << " Digis:         " << setw(10) << fTNofDigis  << "    ( " << (Double_t)fTNofDigis /((Double_t)fTNofEvents) << " per event )" << endl;
  cout << "                       -->    ( " << (Double_t)fTNofDigis /((Double_t)fTNofEvents)/((Double_t)fDigiPar->GetNSensors()) << " per sensor )" << endl;
  cout << "                       -->    ( " << (Double_t)fTNofDigis /((Double_t)fTNofEvents)/((Double_t)fDigiPar->GetNChannels())*100. << "% occupancy )" << endl;
  cout << "                       -->    ( 2 x " << (Double_t)fTNofDigis /((Double_t)fTNofPoints)/2. << " per point )" << endl;
  cout << "---------------------------------------------------------------------" << endl; 
}
// -------------------------------------------------------------------------



ClassImp(PndGemDigitize)

