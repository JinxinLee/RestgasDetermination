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
#include "PndGemDigiWriteoutBuffer.h"

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
  : FairTask("GEM Digitizer", 0),
    fDigiPar(NULL),
    fPoints(NULL),
    fDigis(NULL),
    fDigiMatches(NULL),
    fSaveOutsideHits(kFALSE),
    fRealisticResponse(kFALSE),
    fTNofEvents(0),
    fTNofPoints(0),
    fTNofDigis(0),
    fRand(new TRandom2()),
    fDataBuffer(0),
    fTimeOrderedDigi(kFALSE) {
  
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
    fSaveOutsideHits(kFALSE),
    fRealisticResponse(kFALSE),
    fTNofEvents(0),
    fTNofPoints(0),
    fTNofDigis(0),
    fRand(new TRandom2()),
    fDataBuffer(0),
    fTimeOrderedDigi(kFALSE) {
  
  Reset();
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
PndGemDigitize::PndGemDigitize(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose),
    fDigiPar(NULL),
    fPoints(NULL),
    fDigis(NULL),
    fDigiMatches(NULL),
    fSaveOutsideHits(kFALSE),
    fRealisticResponse(kFALSE),
    fTNofEvents(0),
    fTNofPoints(0),
    fTNofDigis(0),
    fRand(new TRandom2()),
    fDataBuffer(0),
    fTimeOrderedDigi(kFALSE) {

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

  if ( fRealisticResponse ) 
    DigitizeRealisticEvent();
  else
    DigitizeEvent();

  PndGemDigi* adigi;
  Double_t EventTime = FairRootManager::Instance()->GetEventTime();
  cout << "---------------> Event " << fTNofEvents << endl;
  cout << "--- at  " << EventTime << " ns" << endl;
  cout << "--- has " << fDigis->GetEntriesFast() << " digis made from " << fPoints->GetEntriesFast() << " points" << endl;
  cout << "---------------> Put them in output buffer" << endl;
  for ( Int_t idigi = 0 ; idigi < fDigis->GetEntriesFast() ; idigi++ ) {
    adigi = dynamic_cast<PndGemDigi*>(fDigis->At(idigi));
    adigi->SetTimeStamp(adigi->GetTimeStamp()+EventTime);
    fDataBuffer->FillNewData(adigi,adigi->GetTimeStamp()+EventTime,10.);
  }
  cout << "------------------------------------------" << endl;
}
// -------------------------------------------------------------------------

// -----   Private method DigitizeEvent  --------------------------------------------
void PndGemDigitize::DigitizeEvent() {

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
      ActivateChannel(sensorDetId,0,channelNumber,1,0.,iPoint);
    }

    channelNumber = sensor->GetChannel(locPosIn[0],locPosIn[1],1);
    if ( channelNumber == -1 ) continue;
    
    ActivateChannel(sensorDetId,1,channelNumber,1.,0.,iPoint);
    
  }
}
// -------------------------------------------------------------------------

// -----   Private method DigitizeRealisticEvent  --------------------------------------------
void PndGemDigitize::DigitizeRealisticEvent() {
  //  cout << "DRE " << fTNofEvents << endl;

  Double_t showerSigma =   0.1; // radius of smearing, in cm
  Double_t totalSignal = 100.;  // total signal strength
  Double_t sigMult = 1.e6;

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

    Double_t stripWidth;

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
    Double_t rectSig  = 100.;

    Double_t channelInd = sensor->GetChannel(locPosIn[0],locPosIn[1],0,stripWidth);
    Int_t channelNumber = (Int_t)channelInd;

    //    cout << "channelNumber = " << channelNumber << " / channelInd = " << channelInd << " / stripWidth = " << stripWidth << endl;

    //    SimulateRectangularResponse(sensorDetId,0,channelInd,stripWidth,showerSigma,rectSig,iPoint);
    //    SimulateGaussianResponse(sensor,0,currentPndGemMCPoint,showerSigma,rectSig,iPoint);    
    SimulateGaussianResponse(sensor,0,currentPndGemMCPoint,showerSigma,sigMult*currentPndGemMCPoint->GetEnergyLoss(),iPoint);

    channelInd = sensor->GetChannel(locPosIn[0],locPosIn[1],1,stripWidth);
    channelNumber = (Int_t)channelInd;

    //    cout << "channelNumber = " << channelNumber << " / channelInd = " << channelInd << " / stripWidth = " << stripWidth << endl;

    //    SimulateRectangularResponse(sensorDetId,1,channelInd,stripWidth,showerSigma,rectSig,iPoint);
    //SimulateGaussianResponse(sensor,1,currentPndGemMCPoint,showerSigma,rectSig,iPoint);
    SimulateGaussianResponse(sensor,1,currentPndGemMCPoint,showerSigma,sigMult*currentPndGemMCPoint->GetEnergyLoss(),iPoint);
    
  }
}
// -------------------------------------------------------------------------

// -----   Private method SimulateGaussianResponse  --------------------------------------------
void PndGemDigitize::SimulateGaussianResponse(PndGemSensor* sensor, Int_t side, PndGemMCPoint* gemPoint, Double_t showerSigma, Double_t showerStrength, Int_t iPoint) {
  Double_t posIn [3] = {gemPoint->GetX(),
			gemPoint->GetY(),
			gemPoint->GetZ()};
  Double_t posOut[3] = {gemPoint->GetXOut(),
			gemPoint->GetYOut(),
			gemPoint->GetZOut()};
  
  Int_t sensorDetId = sensor->GetDetectorId();

  Int_t sensorId = gemPoint->GetSensorId();
  TString nodeName = fDigiPar->GetNodeName(sensorId);
  gGeoManager->cd(nodeName.Data());
  TGeoNode* curNode = gGeoManager->GetCurrentNode();
  
  Double_t locPosIn [4];
  Double_t locPosOut[4];
  
  curNode->MasterToLocal(posIn ,locPosIn );
  curNode->MasterToLocal(posOut,locPosOut);

  Double_t scanOrient = sensor->GetStripOrientation(locPosIn[0],locPosIn[1],side) + TMath::Pi()/2.;

  Int_t nofBBB = 1000;
  Double_t unitSig = showerStrength/((Double_t)nofBBB);

  Double_t deltaPos[4];
  for ( Int_t ico = 0 ; ico < 4 ; ico++ ) deltaPos[ico] = (locPosOut[ico] - locPosIn[ico])/((Double_t)(nofBBB));

  Int_t chanNr = sensor->GetChannel(locPosIn[0],locPosIn[1],side);

  Double_t time = gemPoint->GetTime();

  if ( scanOrient > TMath::TwoPi() ) 
    scanOrient -= TMath::TwoPi();

  for ( Int_t ibbb = 0 ; ibbb < nofBBB ; ibbb++ ) {
    Double_t dist = fRand->Gaus(0.,showerSigma);

    Double_t xloc = locPosIn[0]+ibbb*deltaPos[0]-dist*TMath::Sin(scanOrient);
    Double_t yloc = locPosIn[1]+ibbb*deltaPos[1]+dist*TMath::Cos(scanOrient);
    Double_t feeDist;
    chanNr = sensor->GetChannel2(xloc,yloc,side,feeDist);
    time = gemPoint->GetTime()+fRand->Gaus(5,0.5)+fRand->Gaus(feeDist/30.,0.5);
    //time = fTNofEvents;
    if ( chanNr == -1 ) continue;
    ActivateChannel(sensorDetId,side,chanNr,unitSig,time,iPoint);
  }

}
// -------------------------------------------------------------------------

// -----   Private method SimulateRectangularResponse  --------------------------------------------
void PndGemDigitize::SimulateRectangularResponse(Int_t sensorDetId, Int_t side, Double_t channelInd, Double_t stripWidth, Double_t showerSigma, Double_t showerStrength, Int_t iPoint) {
  Int_t channelNumber = (Int_t)channelInd;

  Double_t thisASig = 0.;
  
  Double_t leftSide = showerSigma;
  Double_t leftTBin = (channelInd-(Double_t)channelNumber)*stripWidth;
  Int_t    leftChan = channelNumber;
  
  for ( Int_t ibin = 0 ; ibin < 50 ; ibin++ ) {
    Double_t leftASig = leftTBin*showerStrength/showerSigma;
    if ( leftTBin > leftSide ) {
      leftASig = leftSide*showerStrength/showerSigma;
      ibin = 100; // to finish the loop
    }
    
    //cout << "Lwill activate channel " << leftChan << " with " << leftASig << endl;
    if ( ibin == 0 ) 
      thisASig += leftASig;
    else 
      ActivateChannel(sensorDetId,side,leftChan,leftASig,0.,iPoint);
    
    leftSide  = leftSide - leftTBin;
    leftTBin  = stripWidth;
    leftChan -= 1;
  }
  
  Double_t rightSide = showerSigma;
  Double_t rightTBin = (1.+(Double_t)channelNumber-channelInd)*stripWidth;
  Int_t    rightChan = channelNumber;
  
  for ( Int_t ibin = 0 ; ibin < 50 ; ibin++ ) {
    Double_t rightASig = rightTBin*showerStrength/showerSigma;
    if ( rightTBin > rightSide ) {
      rightASig = rightSide*showerStrength/showerSigma;
      ibin = 100; // to finish the loop
    }
    
    //cout << "Rwill activate channel " << rightChan << " with " << rightASig << endl;
    if ( ibin == 0 ) 
      thisASig += rightASig;
    else 
      ActivateChannel(sensorDetId,side,rightChan,rightASig,0.,iPoint);
    
    rightSide  = rightSide - rightTBin;
    rightTBin  = stripWidth;
    rightChan += 1;
  }
  ActivateChannel(sensorDetId,side,channelNumber,thisASig,0.,iPoint);
}
// ------------------------------------------------------------------------

// -----   Private method ActivateChannel   -------------------------------
void PndGemDigitize::ActivateChannel(Int_t sensorDetId, Int_t sensorSide, Int_t channelNumber, Double_t signalHeight, Double_t signalTime, Int_t iPoint) {
  Int_t digiDetId = sensorDetId | kGemDigi << 21 | sensorSide << 5;

  pair<Int_t, Int_t> a (digiDetId, channelNumber);
  if ( fChannelMap.find(a) == fChannelMap.end() ) {
    // Channel not yet active, create new digi
    
    //	cout << fNDigis << " : " << sensorDetId << " " << channelNumber << endl;
    
    new ((*fDigis)[fNDigis]) PndGemDigi(digiDetId, channelNumber, iPoint, signalHeight, signalTime);

    fChannelMap[a] = fNDigis;
    fNDigis++;
    fTNofDigis++;
  }
  else {
    Int_t iDigi = fChannelMap[a];
    PndGemDigi* ddigi = dynamic_cast<PndGemDigi*>(fDigis->At(iDigi));

    ddigi->AddCharge(signalHeight);

    //	cout << "another mc point to this digi, last one " << ddigi->GetIndex(ddigi->GetNIndices()-1) << ", now adding " << iPoint << endl;
    ddigi->AddIndex(iPoint);
  }
}
// -------------------------------------------------------------------------


// -----   Private method PrintDigis   -------------------------------
void PndGemDigitize::PrintDigis() {
  Int_t nofDigis = fDigis->GetEntriesFast();
  for ( Int_t idigi = 0 ; idigi < nofDigis ; idigi++ ) {
    PndGemDigi* digiToDraw = (PndGemDigi*)fDigis->At(idigi);
    Int_t digiStationNr   = digiToDraw->GetStationNr();
    Int_t digiSensorNr    = digiToDraw->GetSensorNr();
    Int_t digiSide        = digiToDraw->GetSide();
    Int_t channelNumber   = digiToDraw->GetChannelNr();

    cout << "HAVE digi " << idigi << " at " << digiStationNr << "." << digiSensorNr << "." << digiSide << "." << channelNumber << " ( " << digiToDraw->GetDetectorId() << " ) with signal = " << digiToDraw->GetCharge() << endl;

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
  fDigis = new TClonesArray("PndGemDigi",10000);
  ioman->Register("GEMDigiNormal", "Digital response in GEM", fDigis, kTRUE);

  // Register output buffer
  fDataBuffer = new PndGemDigiWriteoutBuffer("GEMDigi", "GEM", kTRUE);
  fDataBuffer = (PndGemDigiWriteoutBuffer*)ioman->RegisterWriteoutBuffer("GEMDigi", fDataBuffer);
  fDataBuffer->ActivateBuffering(fTimeOrderedDigi);

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

