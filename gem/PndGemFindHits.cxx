//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemFindHits source file                 -----
// -----                  Created 15/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemFindHits
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 15.02.2009
 *@version 1.0
 **
 ** PANDA task class for finding PndGemHits from PndGemDigis
 ** Task level RECO
 **/

#include "PndGemFindHits.h"

#include "PndGemDigi.h"
#include "PndGemDigiPar.h"
#include "PndGemHit.h"
#include "PndGemSensor.h"
#include "PndGemStation.h"

#include "PndDetectorList.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TMath.h"

#include <iomanip>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::fixed;
using std::right;
using std::left;
using std::setw;
using std::setprecision;
using std::set;
using std::map;

// -----   Default constructor   ------------------------------------------
PndGemFindHits::PndGemFindHits() : FairTask("GEM Hit Finder", 1) {
  fDigiPar = NULL;
  fDigis   = NULL;
  fHits    = NULL;

  fTNofEvents = 0;
  fTNofDigis  = 0;
  fTNofHits = 0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndGemFindHits::PndGemFindHits(Int_t iVerbose) 
  : FairTask("GEMFindHits", iVerbose) {
  fDigiPar = NULL;
  fDigis   = NULL;
  fHits    = NULL;

  fTNofEvents = 0;
  fTNofDigis  = 0;
  fTNofHits = 0;
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
PndGemFindHits::PndGemFindHits(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose) {
  fDigiPar = NULL;
  fDigis   = NULL;
  fHits    = NULL;

  fTNofEvents = 0;
  fTNofDigis  = 0;
  fTNofHits = 0;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndGemFindHits::~PndGemFindHits() {
  if ( fHits ) {
    fHits->Delete();
    delete fHits;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndGemFindHits::Exec(Option_t* opt) {

  fTNofEvents++;

  fTimer.Start();
  Bool_t warn = kFALSE;

  // Clear output array
  fHits->Clear();

  // Sort STS digis with respect to sectors
  SortDigis();

  // Find hits in sectors
  Int_t nDigisF = 0;
  Int_t nDigisB = 0;
  Int_t nHits   = 0;

  fTimer.Stop();  

  //  cout << "GEM found hits: " << endl;

  Int_t nStations = fDigiPar->GetNStations();
  for (Int_t iStation=0; iStation<nStations; iStation++) {
    PndGemStation* station = (PndGemStation*)fDigiPar->GetStation(iStation);
    Int_t nDigisFInStation = 0;
    Int_t nDigisBInStation = 0;
    Int_t nHitsInStation   = 0;
    Int_t nSensors = station->GetNSensors();
    for (Int_t iSensor=0; iSensor<nSensors; iSensor++) {
      PndGemSensor* sensor = (PndGemSensor*)station->GetSensor(iSensor);

      set <Int_t> fSet, bSet;
      if ( fDigiMapF.find(sensor) == fDigiMapF.end() ) {
	cout << "-E- " << fName << "::Exec: sensor " 
	     << sensor->GetSensorNr() << " of station " 
	     << station->GetStationNr() << "not found in front map!" 
	     << endl;
	warn = kTRUE;
	continue;
      }
      fSet = fDigiMapF[sensor];
      if ( fDigiMapB.find(sensor) == fDigiMapB.end() ) {
	cout << "-E- " << fName << "::Exec: sensor " 
	     << sensor->GetSensorNr() << " of station " 
	     << station->GetStationNr() << "not found in back map!" 
	     << endl;
	warn = kTRUE;
	continue;
      }
      bSet = fDigiMapB[sensor];

      Int_t nDigisFInSensor = fSet.size();
      Int_t nDigisBInSensor = bSet.size();

      Int_t nHitsInSensor   = FindHits(sensor, fSet, bSet);
      if ( fVerbose > 2 ) 
	cout << "Sensor " << sensor->GetSensorNr() 
	     << ", Digis front " << nDigisFInSensor 
	     << ", Digis Back " << nDigisBInSensor
	     << ", Hits " << nHitsInSensor << endl;
      nHitsInStation   += nHitsInSensor;
      nDigisFInStation += nDigisFInSensor;
      nDigisBInStation += nDigisBInSensor;      
    }      // Sector loop

    if ( fVerbose > 1 ) cout << "Total for station " 
			     << station->GetStationNr() << ": Digis front "
			     << nDigisFInStation << ", digis back "
			     << nDigisBInStation << ", hits "
			     << nHitsInStation << endl;
    nDigisB += nDigisBInStation;
    nDigisF += nDigisFInStation;
    nHits   += nHitsInStation;
    
  }       // Station loop

  fTimer.Stop();  

  if ( fVerbose > 1 ) {
    //    cout << "-I- PndGemFindHits::Exec() Created " << fHits->GetEntriesFast() << " hits from " 
      //<< nDigisF << " front and " << nDigisB << " back digis" << endl;
    cout << endl;
    cout << "-I- " << fName << ":Event summary" << endl;
    cout << "    Active channels front side: " << nDigisF << endl;
    cout << "    Active channels back side : " << nDigisB << endl;
    cout << "    Hits created              : " << fHits->GetEntriesFast()   << endl;
    cout << "    Real time                 : " << fTimer.RealTime() 
	 << endl;
  }
  if ( fVerbose == 1 ) {
    if ( warn ) cout << "- ";
    else        cout << "+ ";
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8) 
	 << fixed << right << fTimer.RealTime() 
	 << " s, digis " << nDigisF << " / " << nDigisB << ", hits: " 
	 << nHits << endl;
  }
}
// -------------------------------------------------------------------------

    


// -----   Private method SetParContainers   -------------------------------
void PndGemFindHits::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get GEM digitisation parameter container
  fDigiPar = (PndGemDigiPar*) db->getContainer("PndGemDetectors");
}
// -------------------------------------------------------------------------




// -----   Private method Init   -------------------------------------------
InitStatus PndGemFindHits::Init() {

  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("GEMDigi");

  // Register output array
  fHits = new TClonesArray("PndGemHit", 1000);
  ioman->Register("GEMHit", "Hit in GEM", fHits, kTRUE);
  
  // Create sectorwise digi sets
  MakeSets();

  cout << "-I- " << fName.Data() << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations." << endl;
  cout << "-I- " << fName.Data() << "::Init(). Initialization succesfull." << endl;  
  
  PndGemStation* station = (PndGemStation*)fDigiPar->GetStation(0);
  PndGemSensor* sensor = (PndGemSensor*)station->GetSensor(0);
  cout << "sensor out rad is " << sensor->GetOuterRadius() << endl;

  return kSUCCESS;
}
// -------------------------------------------------------------------------




// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemFindHits::ReInit() {

  // Create sectorwise digi sets
  MakeSets();

  return kSUCCESS;
}
// -------------------------------------------------------------------------




// -----   Private method MakeSets   ---------------------------------------
void PndGemFindHits::MakeSets() {

  fDigiMapF.clear();
  fDigiMapB.clear();
  Int_t nStations = fDigiPar->GetNStations();
  for (Int_t iStation=0; iStation<nStations; iStation++) {
    PndGemStation* station = (PndGemStation*)fDigiPar->GetStation(iStation);
    Int_t nSensors = station->GetNSensors();
    for (Int_t iSensor=0; iSensor<nSensors; iSensor++) {
      PndGemSensor* sensor = (PndGemSensor*)station->GetSensor(iSensor);
      set<Int_t> a;
      fDigiMapF[sensor] = a;
      set<Int_t> b;
      fDigiMapB[sensor] = b;
    }
  }
}
// -------------------------------------------------------------------------




// -----   Private method SortDigis   --------------------------------------
void PndGemFindHits::SortDigis() {

  // Check input array
  if ( ! fDigis ) {
    cout << "-E- " << fName << "::SortDigis: No input array!" << endl;
    return;
  }

  // Clear sensor digi sets
  map<PndGemSensor*, set<Int_t> >::iterator mapIt;
  for (mapIt=fDigiMapF.begin(); mapIt!=fDigiMapF.end(); mapIt++)
    ((*mapIt).second).clear();
  for (mapIt=fDigiMapB.begin(); mapIt!=fDigiMapB.end(); mapIt++)
    ((*mapIt).second).clear();

  // Fill digis into sets
  PndGemDigi* digi = NULL;
  PndGemSensor* sensor = NULL;
  Int_t stationNr = -1;
  Int_t sensorNr  = -1;
  Int_t iSide     = -1;
  Int_t nDigis = fDigis->GetEntriesFast();

  fTNofDigis += nDigis;

  for (Int_t iDigi=0; iDigi<nDigis; iDigi++) {
    digi = (PndGemDigi*) fDigis->At(iDigi);
    stationNr = digi->GetStationNr();
    sensorNr  = digi->GetSensorNr();
    iSide     = digi->GetSide();
    //    cout << "digi #" << iDigi+1 << " in station " << stationNr << " sensor " << sensorNr << " channel " << digi->GetChannelNr() << endl;
    sensor = fDigiPar->GetSensor(stationNr, sensorNr);
    //    cout << "Looking for station " << stationNr << " sensor " << sensorNr << " side " << iSide << endl;
    if (iSide == 0 ) {
      if ( fDigiMapF.find(sensor) == fDigiMapF.end() ) {
	cerr << "-E- " << fName << "::SortDigits:: sensor " << sensorNr
	     << " of station " << stationNr 
	     << " not found in digi scheme (F)!" << endl;
	continue;
      }
      fDigiMapF[sensor].insert(iDigi);      
    }
    else if (iSide == 1 ) {
      if ( fDigiMapB.find(sensor) == fDigiMapB.end() ) {
	cerr << "-E- " << fName << "::SortDigits:: sensor " << sensorNr
	     << " of station " << stationNr 
	     << " not found in digi scheme (B)!" << endl;
	continue;
      }
      fDigiMapB[sensor].insert(iDigi);      
    }
  }

}
// -------------------------------------------------------------------------


    

// -----   Private method FindHits   ---------------------------------------
Int_t PndGemFindHits::FindHits(PndGemSensor* sensor, 
			       set<Int_t>& fSet, set<Int_t>& bSet) {

  
  Int_t    iType  = sensor->GetType();
 
  Double_t sigmaX = 1., sigmaY = 2.;
  
  Int_t iDigiF = -1;
  Int_t iDigiB = -1;
  Int_t iChanF = -1;
  Int_t iChanB = -1;
  Int_t nHits = fHits->GetEntriesFast();
  Double_t xHit;
  Double_t yHit;
  Double_t zHit;
  Double_t dr, dp;
  TVector3 pos, dpos;
  PndGemDigi* digiF = NULL;
  PndGemDigi* digiB = NULL;

  set<Int_t>::iterator it1;
  set<Int_t>::iterator it2;

  for (it1=fSet.begin(); it1!=fSet.end(); it1++) {
    iDigiF = (*it1);
    digiF  = (PndGemDigi*) fDigis->At(iDigiF);
    if ( ! digiF ) {
      cout << "-W- " << GetName() << "::FindHits: Invalid digi index " 
	   << iDigiF << " in front set of sensor " 
	   << sensor->GetDetectorName() << endl;
      continue;
    }
    iChanF = digiF->GetChannelNr();
    for (it2=bSet.begin(); it2!=bSet.end(); it2++) {
      iDigiB = (*it2);
      digiB  = (PndGemDigi*) fDigis->At(iDigiB);
      if ( ! digiB ) {
	cout << "-W- " << GetName() << "::FindHits: Invalid digi index " 
	     << iDigiB << " in front set of sensor " 
	     << sensor->GetDetectorName() << endl;
	continue;
      }
      iChanB = digiB->GetChannelNr();
      
      Int_t sensorDetId = sensor->Intersect(iChanF,iChanB,xHit,yHit,zHit,dr,dp);
      //      cout << "intersecting channels " << iChanF << " and " << iChanB << " gave " << sensorDetId << endl;
      // 	cout << "got the following position from sensor " << sensorDetId << " : (" 
      // 	     << xHit << ", " << yHit << ", " << zHit << ")" << endl;
      if ( sensorDetId == -1 ) continue;

      Double_t rad = TMath::Sqrt(xHit*xHit+yHit*yHit);
      if ( rad < sensor->GetInnerRadius() || rad > sensor->GetOuterRadius() ) {
	cout << " point " << xHit << "," << yHit << " (" << rad << ") is still ok??? at station " 
	     << sensor->GetStationNr() << "." 
	     << sensor->GetSensorNr() << endl;
      }
      
      sigmaX = dp;
      if ( dr > sigmaX ) sigmaX = dr;
      sigmaY = sigmaX;

      pos.SetXYZ(xHit, yHit, zHit);
      dpos.SetXYZ(sigmaX, sigmaY, sensor->GetD());

      //      if ( TMath::Abs(xHit) < 2. ) continue;
      
      Int_t refIndex = -1;
      for ( Int_t irf1 = digiF->GetNIndices()-1 ; irf1 >= 0 ; irf1-- ) 
	for ( Int_t irf2 = digiB->GetNIndices()-1 ; irf2 >= 0 ; irf2-- ) 
	  if ( digiF->GetIndex(irf1) == digiB->GetIndex(irf2) )
	    refIndex = digiF->GetIndex(irf1);

      Int_t hitDetId = sensorDetId | kGemHit << 21;

      //      cout << nHits << " : " << pos.X() << " " << pos.Y() << " " << pos.Z() << endl;
      new ((*fHits)[nHits++]) PndGemHit(hitDetId, pos, dpos,  
					iDigiF, iDigiB, dr, dp, refIndex);

      fTNofHits++;
    }
  }

  return 0;
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void PndGemFindHits::Finish() {
  if ( fHits ) fHits->Clear();

  cout << "-------------------- " << fName.Data() << " : Summary -----------------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents << endl;
  cout << " Digis:         " << setw(10) << fTNofDigis  << "    ( " << (Double_t)fTNofDigis/((Double_t)fTNofEvents) << " per event )" << endl;
  cout << " Hits:          " << setw(10) << fTNofHits   << "    ( " << (Double_t)fTNofHits /((Double_t)fTNofEvents) << " per event )" << endl;
  cout << "                       -->    ( " << (Double_t)fTNofHits  /((Double_t)fTNofEvents)/((Double_t)fDigiPar->GetNSensors()) << " per sensor )" << endl;
  cout << "                       -->    ( " << (Double_t)fTNofHits  /((Double_t)fTNofDigis ) << " per digi )" << endl;
  cout << "---------------------------------------------------------------------" << endl; 

}
// -------------------------------------------------------------------------


ClassImp(PndGemFindHits)


	
	
	
  
		      





 
