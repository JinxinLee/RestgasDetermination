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
#include "PndGemCluster.h"
#include "PndGemDigiPar.h"
#include "PndGemHit.h"
#include "PndGemSensor.h"
#include "PndGemStation.h"
#include "PndGemMonitor.h"

#include "PndDetectorList.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TH2F.h"

#include <iomanip>

using std::cerr;
using std::cout;
using std::endl;
using std::fixed;
using std::flush;
using std::left;
using std::map;
using std::right;
using std::set;
using std::setprecision;
using std::setw;

// -----   Default constructor   ------------------------------------------
PndGemFindHits::PndGemFindHits() : 
  FairTask("GEM Hit Finder", 1),
  fMonitor(NULL),
  fDigiPar(NULL),
  fDigis  (NULL),
  fHits   (NULL),
  fUseClusters(kFALSE),
  fPrepTime(0.),
  fSortTime(0.),
  fCreateTime(0.),
  fConfirmTime(0.),
  fActivateTime(0.),
  fAllTime(0.),
  fTNofEvents(0),
  fTNofDigis (0),
  fTNofHits  (0)
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndGemFindHits::PndGemFindHits(Int_t iVerbose) 
  : FairTask("GEMFindHits", iVerbose),
  fMonitor(NULL),
  fDigiPar(NULL),
  fDigis  (NULL),
  fHits   (NULL),
  fUseClusters(kFALSE),
  fPrepTime(0.),
  fSortTime(0.),
  fCreateTime(0.),
  fConfirmTime(0.),
  fActivateTime(0.),
  fAllTime(0.),
  fTNofEvents(0),
  fTNofDigis (0),
  fTNofHits  (0)
{
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
PndGemFindHits::PndGemFindHits(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose) {
  fDigiPar = NULL;
  fDigis   = NULL;
  fHits    = NULL;

  fUseClusters = kFALSE;

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

  //  cout << "======== PndGemFindHits::Exec(Event = " << fTNofEvents << " ) ====================" << endl;
  fTimer.Start();

  fTNofEvents++;

  Bool_t warn = kFALSE;

  // Clear output array
  fHits->Clear();

  fPrepTime+=fTimer.RealTime(); // preparation time
  fTimer.Continue();

  // Sort GEM digis with respect to sectors
  SortDigis();

  fSortTime+=fTimer.RealTime(); 
  fTimer.Continue();

  // Find hits in sectors
  Int_t nDigisF = 0;
  Int_t nDigisB = 0;
  Int_t nHits   = 0;

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
  
  fCreateTime+=fTimer.RealTime();
  fTimer.Continue();
  
  if ( fUseClusters ) {
    // can only confirm hits using clusters fromt the same event
    ConfirmHits();
    fConfirmTime+=fTimer.RealTime();
    fTimer.Continue();
    // there should be another funciton, that will confirm the hits using fired strips in previous events, that could have obscured the cluster/hit finding in this event because of the small time difference
    // activate digis from the clusters in the PndGemMonitor
    ActivateDigis();
    fActivateTime+=fTimer.RealTime();
    fTimer.Continue();
    // check the digis in the previous events...
  }

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

  //  cout << "         PndGemFindHits::Exec. Created " << fHits->GetEntries() << " hits out of " << fDigis->GetEntries() << "  digis/clusters." << endl;
  
  fAllTime+=fTimer.RealTime();
  
  fTimer.Stop();
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
  if ( fUseClusters ) 
    fDigis = (TClonesArray*) ioman->GetObject("GEMCluster");
  else
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

  // fChargeCorrHist = new TH2F("fChargeCorrHist","fChargeCorrHist",1000,0.,100.,1000,0.,100.);
  // fChargeDiffHist = new TH2F("fChargeDiffHist","fChargeDiffHist",1000,0.,100.,1000,-5.,5.);

  // Open GEM Monitor
  fMonitor = PndGemMonitor::Instance();

  for ( Int_t istat = 0 ; istat < fDigiPar->GetNStations() ; istat++ ) {
    station = (PndGemStation*)fDigiPar->GetStation(istat);
    for ( Int_t isens = 0 ; isens < station->GetNSensors() ; isens++ ) {
      sensor = (PndGemSensor*)station->GetSensor(isens);
      fMonitor->CreateSensorMonitor(*sensor);
    }
  }

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

// -----   Private method ActivateDigis   --------------------------------------
void PndGemFindHits::ActivateDigis() {
  PndGemCluster* cluster;
  for ( Int_t iclus = 0 ; iclus < fDigis->GetEntries() ; iclus++ ) {
    cluster = (PndGemCluster*)fDigis->At(iclus);
 
    fMonitor->EnableCluster(fTNofEvents,iclus,cluster);
  }
}
// -------------------------------------------------------------------------
 
// -----   Private method ConfirmHits   --------------------
void PndGemFindHits::ConfirmHits() {
  Bool_t printInfo = kFALSE;
  PndGemSensor* sensor = NULL;
  PndGemHit* hit = NULL;
  PndGemCluster* cluster = NULL;

  for ( Int_t ihit = 0 ; ihit < fHits->GetEntriesFast() ; ihit++ ) {
    hit = (PndGemHit*)fHits->At(ihit);

    sensor = (PndGemSensor*)fDigiPar->GetSensor(hit->GetStationNr(),
						(3-hit->GetSensorNr ()));
    Double_t hitProjX = hit->GetX()*(sensor->GetZ0()/hit->GetZ());
    Double_t hitProjY = hit->GetY()*(sensor->GetZ0()/hit->GetZ());
    // if ( abs(hit->GetX()+3.8)<0.1 &&
    // 	 abs(hit->GetY()-2.0)<0.1 &&
    // 	 hit->GetStationNr()==2 )
    //   printInfo = kTRUE;
    // else
    //   printInfo = kFALSE;

    if ( printInfo ) {
      cout << "COMPARING HIT " << ihit << " at " 
	   << hit->GetX() << ","
	   << hit->GetY() << ","
	   << hit->GetZ() << " @ "
	   << hit->GetTimeStamp() << endl;
    }

    Bool_t hitConfirmed[2] = {kFALSE,kFALSE};
    Int_t  channelNr   [2] = {sensor->GetChannel(hitProjX,hitProjY,0),
			      sensor->GetChannel(hitProjX,hitProjY,1)};

    // check in previous events....
    for ( Int_t iside = 0 ; iside < 2 ; iside++ ) {
      Double_t lastChannelActivateTime = fMonitor->ChannelLastActiveAt(hit->GetStationNr(),
								       (3-hit->GetSensorNr()),
								       iside,
								       channelNr[iside]);
      if ( printInfo ) {
	cout << " on " << hit->GetStationNr() << "." << 3-hit->GetSensorNr() << "." << iside << " channel " << channelNr[iside] << " was last active at " << lastChannelActivateTime << endl;
	//	fMonitor->Print();
      }
      if ( lastChannelActivateTime < 0 ) continue;
      if ( hit->GetTimeStamp()-lastChannelActivateTime < 100 && 
	   hit->GetTimeStamp()-lastChannelActivateTime > -10 ) {
	if (printInfo) cout << "HIT CONFIRMED ON SIDE " << iside << " IN SOME PREVIOUS EVENT (" << hit->GetTimeStamp()-lastChannelActivateTime << " ns ago)" << endl;
	hitConfirmed[iside] = kTRUE;
      }
    }

    // check clusters in this event only if in the previous events the hit was not confirmed
    if ( hitConfirmed[0] == kFALSE || hitConfirmed[1] == kFALSE ) {
      for ( Int_t iclus = 0 ; iclus < fDigis->GetEntries() ; iclus++ ) {
	cluster = (PndGemCluster*)fDigis->At(iclus);
	
	if ( cluster->GetTimeStamp() < hit->GetTimeStamp() - 111. ||
	     cluster->GetTimeStamp() > hit->GetTimeStamp() +  11. )
	  continue;
	
	if ( cluster->GetStationNr() != hit->GetStationNr() ) continue;
	if ( cluster->GetSensorNr () == hit->GetSensorNr () ) continue;
	
	// should have some function in sensor for that:
	if ( channelNr[cluster->GetSide()] <= cluster->GetClusterEnd() &&
	     channelNr[cluster->GetSide()] >= cluster->GetClusterBeg() ) {
	  hitConfirmed[cluster->GetSide()] = kTRUE;
	  if ( printInfo ) {
	    cout << "HIT CONFIRMED ON SIDE " << cluster->GetSide() << " BY CLUSTER " << iclus << " at "
		 << (cluster->GetSide()==0?"F":"B") << "side on "
		 << cluster->GetStationNr() << "."
		 << cluster->GetSensorNr() << " from "
		 << (cluster->GetClusterBeg()) << " to "
		 << (cluster->GetClusterEnd()) << " **** "
		 << sensor->GetChannel(hitProjX,hitProjY,cluster->GetSide()) 
		 << ", time_diff = " << hit->GetTimeStamp() << " - " << cluster->GetTimeStamp() << "ns" 
		 << endl;
	  }
	}
      }
    }

    if ( hitConfirmed[0] == kTRUE && hitConfirmed[1] == kTRUE ) {
      if ( printInfo ) cout << "GOOD HIT" << endl;
      ((PndGemHit*)fHits->At(ihit))->SetCharge(10.);
    }
    else {
      if ( printInfo ) cout << "   NO CLUSTER MATCHES TO THIS HIT" << endl;
    }
  }
}
// -------------------------------------------------------------------------

// // -----   Private method ConfirmHits   --------------------------------------
// void PndGemFindHits::ConfirmHits() {
//   PndGemSensor* sensor = NULL;
//   PndGemHit* hit = NULL;
//   PndGemCluster* cluster = NULL;

//   for ( Int_t ihit = 0 ; ihit < fHits->GetEntriesFast() ; ihit++ ) {
//     hit = (PndGemHit*)fHits->At(ihit);

//     cout << "trying to confirm hit " << ihit << " at " 
// 	 << hit->GetX() << ","
// 	 << hit->GetY() << ","
// 	 << hit->GetZ() << " @ "
// 	 << hit->GetTimeStamp() << endl;

//     sensor = (PndGemSensor*)fDigiPar->GetSensor(hit->GetStationNr(),
// 						(3-hit->GetSensorNr ()));
//     Double_t hitProjX = hit->GetX()*(sensor->GetZ0()/hit->GetZ());
//     Double_t hitProjY = hit->GetY()*(sensor->GetZ0()/hit->GetZ());

//     cout << "   with hits at sensor @ z = " << sensor->GetZ0() << endl;
//     Int_t hitConfirmed = 0;
      
//     }
//     if ( hitConfirmed < 2 ) 
//       cout << "   NO CLUSTER MATCHES TO THIS HIT" << endl;
//     else
//       ((PndGemHit*)fHits->At(ihit))->SetCharge(10.);
//   }
// }
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
  PndGemDigi*     digi = NULL;
  PndGemCluster* clust = NULL;
  PndGemSensor* sensor = NULL;
  Int_t stationNr = -1;
  Int_t sensorNr  = -1;
  Int_t iSide     = -1;
  Int_t nDigis = fDigis->GetEntriesFast();

  fTNofDigis += nDigis;

  for (Int_t iDigi=0; iDigi<nDigis; iDigi++) {
    digi  = (PndGemDigi*)    fDigis->At(iDigi);
    clust = (PndGemCluster*) fDigis->At(iDigi);
    stationNr = digi->GetStationNr();
    sensorNr  = digi->GetSensorNr();
    iSide     = digi->GetSide();
    if ( !( sensorNr == 1 && iSide == 0 ) && clust->GetClusterEnd()-clust->GetClusterBeg() <= 1 ) {
      //      cout << "THE DIGI " << iDigi << ", DET ID = " << digi->GetDetectorId() << " in STATION " << stationNr << " SENSOR " << sensorNr << " SIDE " << iSide << " -> " << clust->GetClusterBeg() << " : " << clust->GetClusterEnd() << endl;
      continue;
    }
    //    cout << "THE DIGI " << iDigi << ", DET ID = " << digi->GetDetectorId() << " in STATION " << stationNr << " SENSOR " << sensorNr << " SIDE " << iSide << endl;
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
  Double_t iChanF = -1;
  Double_t iChanB = -1;
  Int_t nHits = fHits->GetEntriesFast();
  Double_t xHit;
  Double_t yHit;
  Double_t zHit;
  Double_t dx, dy, dr, dp;
  TVector3 pos, dpos;
  PndGemDigi* digiF = NULL;
  PndGemDigi* digiB = NULL;
  PndGemCluster* clusterB = NULL;

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
      clusterB = (PndGemCluster*) fDigis->At(iDigiB);
      if ( ! digiB ) {
	cout << "-W- " << GetName() << "::FindHits: Invalid digi index " 
	     << iDigiB << " in front set of sensor " 
	     << sensor->GetDetectorName() << endl;
	continue;
      }
      iChanB = digiB->GetChannelNr();
      
      Int_t sensorDetId = sensor->Intersect(iChanF,iChanB,xHit,yHit,zHit,dx,dy,dr,dp);
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
      
      sigmaX = dx;
      sigmaY = dy;

//       if ( fUseClusters ) {
// 	zHit += sensor->GetD()/2.;
// 	if ( sensor->GetType() ) 
// 	  zHit -= sensor->GetD();
//       }
      //      cout << "what about sensorz0 = " << sensor->GetZ0() << " or zHit = " << zHit << endl;

      pos.SetXYZ(xHit, yHit, zHit);
      dpos.SetXYZ(sigmaX, sigmaY, sensor->GetD());

      //      cout << "SETTING ERROR TO " << sigmaX << " " << sigmaY << " " << sensor->GetD() << " at " << zHit << endl;

      //      if ( TMath::Abs(xHit) < 2. ) continue;
      
      Int_t refIndex = -1;
      for ( Int_t irf1 = digiF->GetNIndices()-1 ; irf1 >= 0 ; irf1-- ) 
	for ( Int_t irf2 = digiB->GetNIndices()-1 ; irf2 >= 0 ; irf2-- ) 
	  if ( digiF->GetIndex(irf1) == digiB->GetIndex(irf2) )
	    refIndex = digiF->GetIndex(irf1);

      Int_t hitDetId = sensorDetId | kGemHit << 21;

      //      cout << nHits << " : " << pos.X() << " " << pos.Y() << " " << pos.Z() << endl;

      // fChargeCorrHist->Fill(digiF->GetCharge(),digiB->GetCharge());
      // fChargeDiffHist->Fill((digiF->GetCharge()+digiB->GetCharge())/2.,
      // 			    (digiF->GetCharge()-digiB->GetCharge()));

      //      if ( TMath::Abs(digiF->GetCharge()-digiB->GetCharge()) > 0.02+(digiF->GetCharge()+digiB->GetCharge())/200. ||
      if ( TMath::Abs(digiF->GetCharge()-digiB->GetCharge()) > 5. || 
	   TMath::Abs(digiF->GetTimeStamp()-digiB->GetTimeStamp()) > 5. )
	continue;

      new ((*fHits)[nHits++]) PndGemHit(hitDetId, pos, dpos,  
					(digiF->GetCharge()+digiB->GetCharge())/2.,
					(digiF->GetTimeStamp()+digiB->GetTimeStamp())/2.,
					iDigiF, iDigiB, 
					dr, dp, refIndex);

      fTNofHits++;
    }
  }

  return 0;
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void PndGemFindHits::Finish() {
  if ( fHits ) fHits->Clear();

//  fChargeDiffHist->Draw("colz");

  cout << "-------------------- " << fName.Data() << " : Summary -----------------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents << endl;
  cout << " Digis:         " << setw(10) << fTNofDigis  << "    ( " << (Double_t)fTNofDigis/((Double_t)fTNofEvents) << " per event )" << endl;
  cout << " Hits:          " << setw(10) << fTNofHits   << "    ( " << (Double_t)fTNofHits /((Double_t)fTNofEvents) << " per event )" << endl;
  cout << "                       -->    ( " << (Double_t)fTNofHits  /((Double_t)fTNofEvents)/((Double_t)fDigiPar->GetNSensors()) << " per sensor )" << endl;
  cout << "                       -->    ( " << (Double_t)fTNofHits  /((Double_t)fTNofDigis ) << " per digi )" << endl;
  cout << "---------------------------------------------------------------------" << endl; 

  cout << " >>> HF >>> prep    time = " << fPrepTime << "s      (get data from input)" << endl;
  cout << " >>> HF >>> sort    time = " << fSortTime-fPrepTime << "s    (sort clusters, " << fSortTime << ")" << endl;
  cout << " >>> HF >>> create  time = " << fCreateTime-fSortTime << "s    (create hits, " << fCreateTime << ")" << endl;
  cout << " >>> HF >>> confirm time = " << fConfirmTime-fCreateTime << "s    (confirm hits, " << fConfirmTime << ")" << endl;
  cout << " >>> HF >>> activ.  time = " << fActivateTime-fConfirmTime << "s      (activate digis, " << fActivateTime << ")" << endl;
  cout << " >>> HF >>> all     time = " << fAllTime-fActivateTime << "s       (all time spent in Exec, " << fAllTime << ")" << endl;
  cout << "---------------------------------------------------------------------" << endl; 

}
// -------------------------------------------------------------------------


ClassImp(PndGemFindHits)


	
	
	
  
		      





 
