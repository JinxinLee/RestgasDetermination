//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemFindClusters source file                 -----
// -----                  Created 15/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemFindClusters
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 15.02.2009
 *@version 1.0
 **
 ** PANDA task class for finding PndGemHits from PndGemDigis
 ** Task level RECO
 **/

#include "PndGemFindClusters.h"

#include "PndGemDigi.h"
#include "PndGemDigiPar.h"
#include "PndGemSensor.h"
#include "PndGemStation.h"

#include "PndDetectorList.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom2.h"

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
PndGemFindClusters::PndGemFindClusters() : FairTask("GEMFindClusters", 1) {
  fDigiPar  = NULL;
  fDigis    = NULL;
  fClusters = NULL;

  fTNofEvents   = 0;
  fTNofDigis    = 0;
  fTNofClusters = 0;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
PndGemFindClusters::PndGemFindClusters(Int_t iVerbose) 
  : FairTask("GEMFindClusters", iVerbose) {
  fDigiPar  = NULL;
  fDigis    = NULL;
  fClusters = NULL;

  fTNofEvents   = 0;
  fTNofDigis    = 0;
  fTNofClusters = 0;
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
PndGemFindClusters::PndGemFindClusters(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose) {
  fDigiPar  = NULL;
  fDigis    = NULL;
  fClusters = NULL;

  fTNofEvents   = 0;
  fTNofDigis    = 0;
  fTNofClusters = 0;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndGemFindClusters::~PndGemFindClusters() {
  if ( fClusters ) {
    fClusters->Delete();
    delete fClusters;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndGemFindClusters::Exec(Option_t* opt) {
  if ( fVerbose )
    cout << "==================== EVENT " << fTNofEvents << " =================== " << endl;

  fTNofEvents++;

  fTimer.Start();
  Bool_t warn = kFALSE;

  // Clear output array
  fClusters->Clear();

  Int_t nDigis    = 0;
  Int_t nClusters = 0;

  // Sort digis with respect to time
  nDigis = SortDigis();

  FindClusters();

  if ( fVerbose ) {
    Int_t nofC2W = 0;
    for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ )
      if ( fDigiClusters[idc].cluPos > -0.5 )
	nofC2W++;
    cout << ">>>> created  " << fDigiClusters.size() << " clusters, " << nofC2W << " to write" << endl;
  }

  AnalyzeClusters();
  
  if ( fVerbose ) {
    Int_t nofC2W = 0;
    for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ )
      if ( fDigiClusters[idc].cluPos > -0.5 )
	nofC2W++;
    cout << ">>>> analyzed " << fDigiClusters.size() << " clusters, " << nofC2W << " to write" << endl;
  }

  ClearClusters();
  
  if ( fVerbose ) {
    Int_t nofC2W = 0;
    for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ )
      if ( fDigiClusters[idc].cluPos > -0.5 )
	nofC2W++;
    cout << ">>>> cleared  " << fDigiClusters.size() << " clusters, " << nofC2W << " to write" << endl;
  }

  nClusters = WriteClusters();

  fTimer.Stop();  

  //  cout << "CREATED " << nClusters << " CLUSTERS OUT OF " << nDigis << " DIGIS IN " << fTimer.RealTime() << "s" << endl;

  fTNofClusters += nClusters;
}
// -------------------------------------------------------------------------
    


// -----   Private method SetParContainers   -------------------------------
void PndGemFindClusters::SetParContainers() {

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
InitStatus PndGemFindClusters::Init() {

  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("GEMDigi");

  // Register output array
  fClusters = new TClonesArray("PndGemDigi", 1000);
  ioman->Register("GEMCluster", "Cluster in GEM", fClusters, kTRUE);
  
  cout << "-I- " << fName.Data() << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations." << endl;
  cout << "-I- " << fName.Data() << "::Init(). Initialization succesfull." << endl;  
  
  PndGemStation* station = (PndGemStation*)fDigiPar->GetStation(0);
  PndGemSensor* sensor = (PndGemSensor*)station->GetSensor(0);
  cout << "sensor out rad is " << sensor->GetOuterRadius() << endl;

  return kSUCCESS;
}
// -------------------------------------------------------------------------




// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemFindClusters::ReInit() {

  // Create sectorwise digi sets
  //  MakeSets();

  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Private method SortDigis   --------------------------------------
Int_t PndGemFindClusters::SortDigis() {

  // Check input array
  if ( ! fDigis ) {
    cout << "-E- " << fName << "::SortDigis: No input array!" << endl;
    return -1;
  }
  TRandom2* randNumber = new TRandom2();

  PndGemDigi* digi;
  Int_t nDigis = fDigis->GetEntriesFast();

  fTimeOrderedDigis.clear();

  fTNofDigis += nDigis;

  for (Int_t iDigi=0; iDigi<nDigis; iDigi++) {
    digi = (PndGemDigi*) fDigis->At(iDigi);
    //    Double_t time = digi->GetTimeStamp();
    Double_t time = randNumber->Gaus(0.,5.);

    fTimeOrderedDigis[time] = iDigi;
  }

  //  cout << "have " << nDigis << " digis ( " << fTimeOrderedDigis.size() << " ) " << endl;

  map<Double_t, Int_t>::iterator mapIt;
  Int_t nd = 0;
  for (mapIt=fTimeOrderedDigis.begin(); mapIt!=fTimeOrderedDigis.end(); mapIt++) {
    //    cout << mapIt->first << " --> " << mapIt->second << endl;
    nd++;
  }
  // cout << " ---> " << nd << endl;

  return nDigis;
}
// -------------------------------------------------------------------------


// -----  Private method FindClusters   --------------------------------------
void PndGemFindClusters::FindClusters() {

  PndGemDigi* digi;

  map<Double_t, Int_t>::iterator mapIt;

  fDigiClusters.clear();

  for (mapIt=fTimeOrderedDigis.begin(); mapIt!=fTimeOrderedDigis.end(); mapIt++) {

    if ( fVerbose ) {
      cout << fTNofEvents << ". fighting for " << mapIt->second << endl;
      for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ ) {
	cout << "     cluster at " << fDigiClusters[idc].cluPos << ", h = " << fDigiClusters[idc].cluADC << ", for detId = " << fDigiClusters[idc].detId << " from " << fDigiClusters[idc].digiNr.size() << " digis:" << endl;// (from " << lowChannel << " to " << topChannel << "):" << endl;
      }
    }

    Bool_t digiUsed = CompareDigiToClusters(mapIt->second);
    if ( digiUsed ) continue;

    digi = (PndGemDigi*) fDigis->At(mapIt->second);

    if ( fVerbose > 1 ) 
      cout << "creating cluster " << fDigiClusters.size() << " from digi " << mapIt->second << " : " << digi->GetDetectorId() << " / " << digi->GetChannelNr() << endl;
    DigiCluster tempDC;
    tempDC.detId  = digi->GetDetectorId();
    tempDC.digiNr.push_back(mapIt->second);
    tempDC.chanNr.push_back((Int_t)digi->GetChannelNr());
    tempDC.sigADC.push_back(digi->GetCharge());
    tempDC.cluTDC = digi->GetTimeStamp();
    tempDC.cluADC = digi->GetCharge();
    tempDC.cluPos = digi->GetChannelNr();
    tempDC.cluMPs = digi->GetChannelNr();
    tempDC.cluMVl = digi->GetCharge();

    fDigiClusters.push_back(tempDC);
  }

}
// -------------------------------------------------------------------------

// -----  Private method AnalyzeClusters   --------------------------------------
void PndGemFindClusters::AnalyzeClusters() {
  if ( fVerbose > 0 ) {
    cout << "STARTING CLUSTER ANALYSIS WITH " << fDigiClusters.size() << " CLUSTERS" << endl;
    for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ ) {
      cout << idc << ".th cluster at " << fDigiClusters[idc].cluPos << ", h = " << fDigiClusters[idc].cluADC << ", for detId = " << fDigiClusters[idc].detId << " from " << fDigiClusters[idc].digiNr.size() << " digis:" << endl;// (from " << lowChannel << " to " << topChannel << "):" << endl;
    }
  }

  Int_t nClusters = fDigiClusters.size();

  PndGemSensor* sensor;
  Int_t         side;
  PndGemDigi*   digi;
  Double_t      dist;

  std::vector<Bool_t> digiVect(20000,kFALSE);
  std::vector<Bool_t> commVect(20000,kFALSE);

  for ( Int_t idc1 = 0 ; idc1 < nClusters ; idc1++ ) {

    digiVect.assign(20000,kFALSE);

    Bool_t jointDigi = kFALSE;

    for ( Int_t idc2 = idc1+1 ; idc2 < nClusters ; idc2++ ) {
      if ( fDigiClusters[idc1].detId != fDigiClusters[idc2].detId ) continue;
      digi   = (PndGemDigi*)fDigis->At(fDigiClusters[idc1].digiNr[0]);
      sensor = fDigiPar->GetSensor(digi->GetStationNr(),digi->GetSensorNr());
      side   = digi->GetSide();

      dist   = sensor->GetDistance(side,fDigiClusters[idc1].cluPos,fDigiClusters[idc2].cluPos);

      if ( dist <  0 ) continue;
      if ( dist > 30 ) continue;

      //      cout << "WILL JOIN cluster " << idc1 << " and " << idc2 << endl;

      commVect.assign(20000,kFALSE);
      for ( Int_t id = 0 ; id < fDigiClusters[idc2].digiNr.size() ; id++ ) {
	//	cout << "cluster2 id = " << id << " and in fact " << fDigiClusters[idc2].digiNr[id] << endl;
	commVect[fDigiClusters[idc2].digiNr[id]] = kTRUE;
      }

      if ( fVerbose > 1 ) 
	cout << "CLUSTER " << idc2 << " (" << fDigiClusters[idc2].digiNr.size() << " digis at " << fDigiClusters[idc2].cluPos << ") joined with cluster " << idc1 << " ---> " << flush;
      for ( Int_t id = 0 ; id < fDigiClusters[idc1].digiNr.size() ; id++ ) {
	if ( commVect[fDigiClusters[idc1].digiNr[id]] == kFALSE ) {
	  //	  cout << "adding digi id " << id << " and in fact " << fDigiClusters[idc1].digiNr[id] << endl;
	  digi   = (PndGemDigi*)fDigis->At(fDigiClusters[idc1].digiNr[id]);
	  fDigiClusters[idc2].digiNr.push_back(fDigiClusters[idc1].digiNr[id]);
	  fDigiClusters[idc2].chanNr.push_back((Int_t)digi->GetChannelNr());
	  fDigiClusters[idc2].sigADC.push_back(digi->GetCharge());
	  fDigiClusters[idc2].cluTDC = digi->GetTimeStamp();
	  fDigiClusters[idc2].cluPos = sensor->GetMeanChannel(side,fDigiClusters[idc2].cluPos,fDigiClusters[idc2].cluADC,digi->GetChannelNr(),digi->GetCharge());
	  if ( digi->GetCharge() > fDigiClusters[idc2].cluMVl ) {
	    fDigiClusters[idc2].cluMVl = digi->GetCharge();
	    fDigiClusters[idc2].cluMPs = digi->GetChannelNr();
	  }

	  fDigiClusters[idc2].cluADC =  fDigiClusters[idc2].cluADC+digi->GetCharge();
	}
      }
      if ( fVerbose > 1 ) 
	cout << "(" << fDigiClusters[idc2].digiNr.size() << " digis at " << fDigiClusters[idc2].cluPos << ")." << endl;

      jointDigi = kTRUE;
      if ( fVerbose > 1 ) 
	cout << "comparing cluster at " << fDigiClusters[idc1].cluPos << " with " << fDigiClusters[idc2].cluPos << " gives dist of " << dist << endl;
    }

    if ( jointDigi == kTRUE ) 
      fDigiClusters[idc1].cluPos = -1.;
  }

  if ( fVerbose ) {
    cout << "FINISHING CLUSTER ANALYSIS WITH " << fDigiClusters.size() << " CLUSTERS" << endl;
    for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ ) {
      cout << idc << ".th cluster at " << fDigiClusters[idc].cluPos << ", h = " << fDigiClusters[idc].cluADC << ", for detId = " << fDigiClusters[idc].detId << " from " << fDigiClusters[idc].digiNr.size() << " digis:" << endl;// (from " << lowChannel << " to " << topChannel << "):" << endl;
    }
    
    for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ ) {
      // if ( fDigiClusters[idc].cluADC < 105 ) continue;
      cout << "cluster at " << fDigiClusters[idc].cluPos << ", h = " << fDigiClusters[idc].cluADC << ", for detId = " << fDigiClusters[idc].detId << " from " << fDigiClusters[idc].digiNr.size() << " digis:" << endl;// (from " << lowChannel << " to " << topChannel << "):" << endl;
      for ( Int_t idigi = 0 ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
	cout << idigi << " > " 
	     << fDigiClusters[idc].digiNr[idigi] << " @ " 
	     << fDigiClusters[idc].chanNr[idigi] << " h "
	     << fDigiClusters[idc].sigADC[idigi] << endl;
      }
    }
  }

}
// -------------------------------------------------------------------------

// -----  Private method ClearClusters   --------------------------------------
void PndGemFindClusters::ClearClusters() {
  if ( fVerbose )
    cout << "CLEAR CLUSTERS STARTING FROM " << fDigiClusters.size() << endl;

  Int_t intEvent = -1;

  PndGemDigi*   digi;
  PndGemSensor* sensor;

  Int_t nofClustersAtBegin = fDigiClusters.size();
  for ( Int_t idc = 0 ; idc < nofClustersAtBegin ; idc++ ) {
    if ( fDigiClusters[idc].cluPos < -0.5 ) continue;

    if ( fVerbose || fTNofEvents == intEvent )
      cout << "-------------------------------------------------" << endl;

    // GET THE LOWEST AND HIGHEST CHANNEL NUMBER
    Int_t lowChannel = 1000000;
    Int_t topChannel = -1;
    for ( Int_t idigi = 0 ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
      if ( fVerbose ) {
	cout << idigi << " > " 
	     << fDigiClusters[idc].digiNr[idigi] << " @ " 
	     << fDigiClusters[idc].chanNr[idigi] << " h "
	     << fDigiClusters[idc].sigADC[idigi] << endl;
      }
      if ( fDigiClusters[idc].chanNr[idigi] < lowChannel ) lowChannel = fDigiClusters[idc].chanNr[idigi];
      if ( fDigiClusters[idc].chanNr[idigi] > topChannel ) topChannel = fDigiClusters[idc].chanNr[idigi];
    }
    // ---------------------------------------------------------

    // MAKE ORDER IN THE CLUSTER
    Int_t channelPosition = 0;
    for ( Int_t ich = lowChannel ; ich <= topChannel ; ich++ ) {
      for ( Int_t idigi = channelPosition ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
	if ( fDigiClusters[idc].chanNr[idigi] == ich ) {
	  Int_t    dN = fDigiClusters[idc].digiNr[idigi];
	  Int_t    cN = fDigiClusters[idc].chanNr[idigi];
	  Double_t sA = fDigiClusters[idc].sigADC[idigi];
	  fDigiClusters[idc].digiNr[idigi] = fDigiClusters[idc].digiNr[channelPosition];
	  fDigiClusters[idc].chanNr[idigi] = fDigiClusters[idc].chanNr[channelPosition];
	  fDigiClusters[idc].sigADC[idigi] = fDigiClusters[idc].sigADC[channelPosition];
	  fDigiClusters[idc].digiNr[channelPosition] = dN;
	  fDigiClusters[idc].chanNr[channelPosition] = cN;
	  fDigiClusters[idc].sigADC[channelPosition] = sA;
	  channelPosition++;
	}
      }
    }
    // ---------------------------------------------------------

    // PRINT
    if ( fVerbose || fTNofEvents == intEvent ) {
      cout << fTNofEvents << " cluster at " << fDigiClusters[idc].cluPos << ", h = " << fDigiClusters[idc].cluADC << ", for detId = " << fDigiClusters[idc].detId << " from " << fDigiClusters[idc].digiNr.size() << " digis (from " << lowChannel << " to " << topChannel << "):" << endl;
      for ( Int_t idigi = 0 ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
	cout << idigi << " > " 
	     << fDigiClusters[idc].digiNr[idigi] << " @ " 
	     << fDigiClusters[idc].chanNr[idigi] << " h "
	     << fDigiClusters[idc].sigADC[idigi] << endl;
      }
    }
    // ---------------------------------------------------------

    // FIND PEAKS AND HOLES BETWEEN TEHM
    Bool_t showDets = kFALSE;
    if ( fVerbose ) {
      showDets = kTRUE;
      cout << fTNofEvents << " cluster at " << fDigiClusters[idc].cluPos << ", h = " << fDigiClusters[idc].cluADC << ", for detId = " << fDigiClusters[idc].detId << " from " << fDigiClusters[idc].digiNr.size() << " digis (from " << lowChannel << " to " << topChannel << "):" << endl;
      /*for ( Int_t idigi = 0 ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
	cout << idigi << " > " 
	     << fDigiClusters[idc].digiNr[idigi] << " @ " 
	     << fDigiClusters[idc].chanNr[idigi] << " h "
	     << fDigiClusters[idc].sigADC[idigi] << endl;
	     }*/
    }
    Int_t    lastPos = fDigiClusters[idc].chanNr[0];
    Double_t lastADC = fDigiClusters[idc].sigADC[0];
    Double_t maxADC  = fDigiClusters[idc].sigADC[0];
    Double_t gotPeak = 40.;
    Double_t newPeak = 0;
    Double_t minADC = 1000.;
    Int_t    minPos = -1;
    Int_t nofPeaks = 0;
    std::vector<Int_t> clusterBorders;
    if ( showDets )      cout << "0 > " 
			      << fDigiClusters[idc].digiNr[0] << " @ " 
			      << fDigiClusters[idc].chanNr[0] << " h "
			      << fDigiClusters[idc].sigADC[0] << endl;
    //    for ( Int_t idigi = 1 ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
    Int_t    thisDigi = 1;
    Double_t thisADC  = 0.;
    for ( Int_t ichan = lowChannel+1 ; ichan <= topChannel ; ichan++ ) {
      thisADC = 0.;
      if ( fDigiClusters[idc].chanNr[thisDigi] == ichan ) {
	thisADC = fDigiClusters[idc].sigADC[thisDigi];
	thisDigi++;
      }
//       if ( fDigiClusters[idc].chanNr[idigi] > lastPos+2 )
// 	cout << "will probably make a break here between " << idigi-1 << " and " << idigi << endl;
//      if ( gotPeak < 0. && 
      if ( thisADC < minADC ) { // this is min
	minADC = thisADC;
	minPos = ichan;
	//	cout << "minadc " << minADC << " at " << minPos << endl;
      }
      if ( thisADC < maxADC*.5 && gotPeak < 0. ) { // this is max
	gotPeak = 40.;
	if ( showDets ) 		cout << "is less than half of last max" << endl;
      }
      if ( gotPeak > 0. ) { // got peak already, adc should be going down
	if ( thisADC >= lastADC ) { // but adc not falling down
	  if ( thisADC >  lastADC ) newPeak+=1.0; // possibly a new peak
	  if ( thisADC == lastADC ) {
	    if ( thisADC != 0. ) newPeak+=0.5;
	    if ( thisADC == 0. ) newPeak+=1.0;
	  }
	  if ( newPeak >= 2 ) { // it is a new peak
	    nofPeaks++;
	    if ( showDets || fTNofEvents == intEvent )
	      cout << "NEW PEAK starts from " << minPos << " ( " << minADC << " )." << endl;
	    clusterBorders.push_back(minPos);
	    minADC = 1000.;
	    minPos = -1;
	    gotPeak = -1.;
	    maxADC = 0.;
	  }
	}
	else if ( thisADC < lastADC ) {
	  newPeak = 0;
	}
      }
      lastPos = ichan;
      lastADC = thisADC;
      if ( showDets ) 
	cout << thisDigi << " > " << gotPeak << " " 
	     << fDigiClusters[idc].digiNr[thisDigi] << " @ " 
	     << fDigiClusters[idc].chanNr[thisDigi] << " h "
	     << fDigiClusters[idc].sigADC[thisDigi] << endl;
      if ( lastADC > maxADC ) {
	maxADC = lastADC;
	if ( newPeak == 0 ) { 
	  minPos = -1;
	  minADC = 1000.;
	}
      }
    }

    if ( showDets ) {
      cout << "NOF PEAKS = " << nofPeaks << "  : " << flush;
      for ( Int_t ipk = 0 ; ipk < nofPeaks ; ipk++ )
	cout << "New" << flush;
      cout << endl;
      for ( Int_t ipk = 0 ; ipk < nofPeaks ; ipk++ )
	cout << "---> " << clusterBorders[ipk] << endl;
    }
    // ---------------------------------------------------------

    // ----> still do not consider strips with 0 charge...

    if ( nofPeaks < 2 ) continue; // do not have to do anything...
    // DIVIDE CLUSTER IF nofPeaks > 1
    digi = (PndGemDigi*) fDigis->At(fDigiClusters[idc].digiNr[0]);

    Int_t stationNr = digi->GetStationNr();
    Int_t sensorNr  = digi->GetSensorNr();
    Int_t iSide     = digi->GetSide();

    sensor = fDigiPar->GetSensor(stationNr, sensorNr);

    thisDigi = 0;
    Int_t ichan = lowChannel;

    if ( fTNofEvents == intEvent ) {    
      cout << fTNofEvents << " - - - - CLUSTER " << idc << " FROM " << lowChannel << " TO " << topChannel << " SHOULD BE DIVIDED IN " << nofPeaks << " PEAKS" << endl;
      for ( Int_t idigi = 0 ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
	cout << " > > > > > > > " << idigi << " > " 
	     << fDigiClusters[idc].digiNr[idigi] << " @ " 
	     << fDigiClusters[idc].chanNr[idigi] << " h "
	     << fDigiClusters[idc].sigADC[idigi] << endl;
      }
    }

    for ( Int_t inp = 0 ; inp < nofPeaks ; inp++ ) {
      if ( fTNofEvents == intEvent ) {    
	cout << "*** p e a k " << inp << " to " << clusterBorders[inp+1] << endl;
      }

      DigiCluster tempDC;
      tempDC.detId  = fDigiClusters[idc].detId;
      tempDC.cluTDC = fDigiClusters[idc].cluTDC;
      tempDC.cluADC = 0.;
      tempDC.cluPos = 0.;
      tempDC.cluMPs = 0.;
      tempDC.cluMVl = 0.;

      /*      for ( ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
	if ( fDigiClusters[idc].chanNr[idigi] > clusterBorders[inp+1] ) break;
	tempDC.digiNr.push_back(fDigiClusters[idc].digiNr[idigi]);
	tempDC.chanNr.push_back(fDigiClusters[idc].chanNr[idigi]);
	tempDC.sigADC.push_back(fDigiClusters[idc].sigADC[idigi]);
	if ( fDigiClusters[idc].sigADC[idigi] > tempDC.cluMVl ) {
	  tempDC.cluMVl = fDigiClusters[idc].sigADC[idigi];
	  tempDC.cluMPs = fDigiClusters[idc].chanNr[idigi];
	}
	if ( tempDC.digiNr.size() == 1 ) {
	  tempDC.cluPos = fDigiClusters[idc].chanNr[idigi];
	  tempDC.cluADC = fDigiClusters[idc].sigADC[idigi];
	}
	else {
	  tempDC.cluPos = sensor->GetMeanChannel(iSide,tempDC.cluPos,tempDC.cluADC,fDigiClusters[idc].chanNr[idigi],fDigiClusters[idc].sigADC[idigi]);
	  tempDC.cluADC = tempDC.cluADC+fDigiClusters[idc].sigADC[idigi];
	}
	}*/
      for ( ; ichan <= topChannel ; ichan++ ) {
	if ( fTNofEvents == intEvent ) {    
	  cout << " checking channel " << ichan << endl;
	}

	if ( inp < nofPeaks-1 && ichan > clusterBorders[inp+1] ) break;

	if ( fDigiClusters[idc].chanNr[thisDigi] != ichan ) 
	  continue;

	//	if ( fDigiClusters[idc].digiNr[thisDigi] > 3000 ) 
	if ( fTNofEvents == intEvent ) {    
	  cout << fTNofEvents << " > putting digiNr = " << fDigiClusters[idc].digiNr[thisDigi] 
	       << " (" << fDigiClusters[idc].chanNr[thisDigi] << ") "
	       << " from " << thisDigi << " coming from split of cluster " << idc << endl;
	}

	tempDC.digiNr.push_back(fDigiClusters[idc].digiNr[thisDigi]);
	tempDC.chanNr.push_back(fDigiClusters[idc].chanNr[thisDigi]);
	tempDC.sigADC.push_back(fDigiClusters[idc].sigADC[thisDigi]);
	if ( fDigiClusters[idc].sigADC[thisDigi] > tempDC.cluMVl ) {
	  tempDC.cluMVl = fDigiClusters[idc].sigADC[thisDigi];
	  tempDC.cluMPs = fDigiClusters[idc].chanNr[thisDigi];
	}
	if ( tempDC.digiNr.size() == 1 ) {
	  tempDC.cluPos = fDigiClusters[idc].chanNr[thisDigi];
	  tempDC.cluADC = fDigiClusters[idc].sigADC[thisDigi];
	}
	else {
	  tempDC.cluPos = sensor->GetMeanChannel(iSide,tempDC.cluPos,tempDC.cluADC,fDigiClusters[idc].chanNr[thisDigi],fDigiClusters[idc].sigADC[thisDigi]);
	  tempDC.cluADC = tempDC.cluADC+fDigiClusters[idc].sigADC[thisDigi];
	}
	thisDigi++;
      }
      if ( showDets ) 
	cout << "just to confirm, added cluster with ADC = " << tempDC.cluADC << " at " << tempDC.cluPos 
	     << ", with MAX = " << tempDC.cluMVl << " at " << tempDC.cluMPs 
	     << " with " << tempDC.digiNr.size() << " digis" << endl;
      
      fDigiClusters.push_back(tempDC);
    }
    fDigiClusters[idc].cluPos = -1.;

    // ---------------------------------------------------------
  }


}
// -------------------------------------------------------------------------

// -----  Private method WriteClusters   --------------------------------------
Int_t PndGemFindClusters::WriteClusters() {

  Int_t nClusters = 0;
  std::vector<Int_t> clusterRefs;
  PndGemDigi* digi;

  for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ ) {

    //    if ( fDigiClusters[idc].cluADC < 1. ) continue;
    if ( fDigiClusters[idc].cluPos < -0.5 ) continue;

    clusterRefs.clear();
    
    for ( Int_t id = 0 ; id < fDigiClusters[idc].digiNr.size() ; id++ ) {
      digi   = (PndGemDigi*)fDigis->At(fDigiClusters[idc].digiNr[id]);

      if ( !digi ) cout << "there is no digi number " << fDigiClusters[idc].digiNr[id] << ", cause there are only " << fDigis->GetEntries() << " of them" << endl;
      for ( Int_t irf = digi->GetNIndices()-1 ; irf >= 0 ; irf-- ) {
	Int_t refIndex = digi->GetIndex(irf);

	Bool_t alreadyKnown = kFALSE;
	for ( Int_t icr = clusterRefs.size()-1 ; icr >= 0 ; icr-- ) {
	  if ( refIndex == clusterRefs[icr] ) {
	    alreadyKnown = kTRUE;
	    break;
	  }
	}
	if ( !alreadyKnown ) clusterRefs.push_back(refIndex);
      }
    }

    if ( fVerbose > 1 ) {
      Int_t lowChannel = 1000000;
      Int_t topChannel = -1;
      for ( Int_t idigi = 0 ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
	if ( fDigiClusters[idc].chanNr[idigi] < lowChannel ) lowChannel = fDigiClusters[idc].chanNr[idigi];
	if ( fDigiClusters[idc].chanNr[idigi] > topChannel ) topChannel = fDigiClusters[idc].chanNr[idigi];
      }
      
      cout << "cluster at " << fDigiClusters[idc].cluPos << ", h = " << fDigiClusters[idc].cluADC << ", for detId = " << fDigiClusters[idc].detId << " from " << fDigiClusters[idc].digiNr.size() << " digis (from " << lowChannel << " to " << topChannel << "): " << endl;
      
      for ( Int_t ich = lowChannel ; ich <= topChannel ; ich++ ) {
	for ( Int_t idigi = 0 ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
	  if ( fDigiClusters[idc].chanNr[idigi] == ich ) {
	    cout << fDigiClusters[idc].digiNr[idigi] << ". " 
		 << fDigiClusters[idc].chanNr[idigi] << " at " 
		 << fDigiClusters[idc].sigADC[idigi] << "." << endl;
	  }
	}
      }
    }

    digi = new ((*fClusters)[nClusters]) PndGemDigi(fDigiClusters[idc].detId, 
						    fDigiClusters[idc].cluPos,
						    clusterRefs[0],
						    fDigiClusters[idc].cluADC,
						    fDigiClusters[idc].cluTDC);
    for ( Int_t icr = clusterRefs.size()-1 ; icr > 0 ; icr-- ) 
      digi->AddIndex(clusterRefs[icr]);
      if ( fVerbose > 1 ) 
	cout << "creating cluster at " << fDigiClusters[idc].cluPos << " with height of " << fDigiClusters[idc].cluADC << " /// compare maximum at " << fDigiClusters[idc].cluMPs << endl;
    nClusters++;
  }
  return nClusters;
}
// -------------------------------------------------------------------------


// -----   Private method AddDigiToCluster()   ------------------------------------------
Bool_t PndGemFindClusters::CompareDigiToClusters(Int_t digiNumber) {
  PndGemSensor* sensor;
  PndGemDigi* digi;

  Bool_t digiUsed = kFALSE;
  for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ ) {
    digi = (PndGemDigi*) fDigis->At(digiNumber);

    if ( digi->GetDetectorId() != fDigiClusters[idc].detId ) continue;

    Int_t stationNr = digi->GetStationNr();
    Int_t sensorNr  = digi->GetSensorNr();
    Int_t iSide     = digi->GetSide();

    sensor = fDigiPar->GetSensor(stationNr, sensorNr);

    Double_t digiClustDist = sensor->GetDistance(iSide,digi->GetChannelNr(),fDigiClusters[idc].cluPos);
    if ( digiClustDist < 0. ) continue; // there's no connection
    if ( digiClustDist > 50. ) continue; // they are too far

    if ( fVerbose > 1 ) 
      cout << "digi " << digiNumber << " ( " << digi->GetDetectorId() << " / " << digi->GetChannelNr() << " ) could attach to cluster " 
	   << idc << " ( " << fDigiClusters[idc].cluPos << " ) with distance " << digiClustDist << " . " << fDigiClusters[idc].digiNr.size() << " digis in cluster" << flush;

    fDigiClusters[idc].digiNr.push_back(digiNumber);
    fDigiClusters[idc].chanNr.push_back((Int_t)digi->GetChannelNr());
    fDigiClusters[idc].sigADC.push_back(digi->GetCharge());
    fDigiClusters[idc].cluTDC = digi->GetTimeStamp();
    fDigiClusters[idc].cluPos = sensor->GetMeanChannel(iSide,fDigiClusters[idc].cluPos,fDigiClusters[idc].cluADC,digi->GetChannelNr(),digi->GetCharge());
    fDigiClusters[idc].cluADC =  fDigiClusters[idc].cluADC+digi->GetCharge();
    if ( digi->GetCharge() > fDigiClusters[idc].cluMVl ) {
      fDigiClusters[idc].cluMVl = digi->GetCharge();
      fDigiClusters[idc].cluMPs = digi->GetChannelNr();
    }

    if ( fVerbose > 1 ) 
      cout << " >>> new pos = " << fDigiClusters[idc].cluPos << endl;

    digiUsed = kTRUE;
  }
  return digiUsed;

}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void PndGemFindClusters::Finish() {
  if ( fClusters ) fClusters->Clear();

  cout << "-------------------- " << fName.Data() << " : Summary -----------------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents << endl;
  cout << " Digis:         " << setw(10) << fTNofDigis      << "    ( " << (Double_t)fTNofDigis   /((Double_t)fTNofEvents) << " per event )" << endl;
  cout << " Clusters:      " << setw(10) << fTNofClusters   << "    ( " << (Double_t)fTNofClusters/((Double_t)fTNofEvents) << " per event )" << endl;
  cout << "                       -->    ( " << (Double_t)fTNofDigis  /((Double_t)fTNofClusters) << " digis per cluster )" << endl;
  cout << "---------------------------------------------------------------------" << endl; 

}
// -------------------------------------------------------------------------


ClassImp(PndGemFindClusters)

