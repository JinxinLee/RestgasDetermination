// -------------------------------------------------------------------------
// PndSciTDigiTask source file 
//           
//  created by D. Steinschaden
//  last update  06.2015
// -------------------------------------------------------------------------

#include "PndSciTDigiTask.h"
#include "PndSciTHit.h"
#include "PndSciTPoint.h"
#include "PndSciTHitSorterTask.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoVector.h"

#include "TVector3.h"
#include "TGeoBBox.h"
#include "TClonesArray.h"
#include "TGeoManager.h"

#include <cmath>

// -----   Default constructor   -------------------------------------------
PndSciTDigiTask::PndSciTDigiTask() :
  FairTask("PndSciT Hit Producer")
{
	fInBranchName 	= "SciTPoint";
	fOutBranchName 	= "SciTHit";
	fSortedOutBranchName = "SciTSortedHit";

	fTimeOrderedDigi = kFALSE;
	fPersistance = kTRUE;

	fGeoH =  NULL;

	fdt = 0.1;  //auto time resolution 0.1 ns
	fDeadtime = 1000.0; // Tile dead time after a hit in ns

}
// -------------------------------------------------------------------------

// -----   Default constructor   -------------------------------------------
PndSciTDigiTask::PndSciTDigiTask(Double_t dt, Double_t deadtime) :
  FairTask("Ideal PndSciT Hit Producer")
{
	fInBranchName 	= "SciTPoint";
	fOutBranchName 	= "SciTHit";
	fSortedOutBranchName = "SciTSortedHit";

	fTimeOrderedDigi = kFALSE;
	fPersistance = kTRUE;

	fGeoH =  NULL;

	fdt = dt;
	fDeadtime = deadtime;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSciTDigiTask::~PndSciTDigiTask() 
{ 
}

// -----   Public method Init   --------------------------------------------
InitStatus PndSciTDigiTask::Init() 
{
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  
  if ( ! ioman ) 
    {
      std::cout << "-E- PndSciTDigiTask::Init: "
	   << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject(fInBranchName);

  if ( ! fPointArray ) 
    {
      std::cout << "-W- PndSciTDigiTask::Init: "
	   << "No SciTPoint array!" << std::endl;
      return kERROR;
  }
  //eventbased
  // Create and register output array
  //fHitArray = ioman->Register(OutBranchName, ClassName, FolderName, kTRUE);
  //fHitArray = ioman->Register("SciTHit", "PndSciTHit", "SciT", kTRUE);

  //time and event based simulation:
  // fDataBuffer = new PndSciTHitWriteoutBuffer(fOutBranchName, fFolderName", fPersistance);
  fDataBuffer = new PndSciTHitWriteoutBuffer(fOutBranchName, "SciT", fPersistance);
  fDataBuffer = (PndSciTHitWriteoutBuffer*)ioman->RegisterWriteoutBuffer(fOutBranchName, fDataBuffer);
  fDataBuffer->ActivateBuffering(fTimeOrderedDigi);

  std::cout << "-I- PndSciTDigiTask: Intialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------

void PndSciTDigiTask::RunTimeBased(){

  fTimeOrderedDigi = kTRUE;
  this->Add(new PndSciTHitSorterTask(2000, 1, fOutBranchName, fSortedOutBranchName, "SciT"));
}




// ----------------------------------------------------------------------------
void PndSciTDigiTask::SetParContainers()
{
  // Get Base Container
  // FairRun* ana = FairRun::Instance();
  //FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  if ( fGeoH == NULL ){
    std::cout << "ScitTil fGeoH is loading" << std::endl;
    fGeoH = PndGeoHandling::Instance();
  } 
  else std::cout << "ScitTil fGeoH is already defind but shouldn't" << std::endl;
  if ( fGeoH == NULL ){
    std::cout << "ScitTil fGeoH was loaded but is still NULL" << std::endl;
  }
  fGeoH->SetParContainers();

  return;
}


// -----   Public method Exec   --------------------------------------------
void PndSciTDigiTask::Exec(Option_t* opt) 
{
  // Reset output array
  if ( ! fHitArray ) 
    Fatal("Exec", "No HitArray");

  //fHitArray->Clear();
 
  
  // Declare some variables

  PndSciTPoint *point = NULL;

  Int_t detectorID;    // Detector ID /shortID
  TString detectorName;
  Double_t mcTime,eventTime,hitTime,detectingTime;

  TVector3 zeroVector(0,0,0);

  TVector3 detectorPosition;
  TVector3 hitPosition;
  TVector3 sensorDim; // Sensor dimension always in half the lenghts in root!
  TVector3 dHitPosition;

  // Loop over SciTPoints
  Int_t 
    nPoints = fPointArray->GetEntriesFast();

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) 
    {
      point = (PndSciTPoint*) fPointArray->At(iPoint);
      if (fVerbose>0) std::cout << " Ideal Hit Producer -Point-: " << point << std::endl;
      if ( ! point) 
	continue;

      // Detector ID
      detectorID = point->GetDetectorID();
      detectorName = point->GetDetName();
  
      // HitPosition in the middle of the sensor = Detector Position

      detectorPosition = fGeoH->LocalToMasterShortId(zeroVector, detectorID);
      hitPosition = detectorPosition;

      // Get the range for the Hit position

      // sensor Dimensions equivalent to the potential error of the hitPosition in the center of the Tile. Attention,in real its no Gaussian shaped distribution but an rectangual!!
    
      sensorDim = fGeoH->GetSensorDimensionsShortId(detectorID);
      dHitPosition = sensorDim;


     // produce realistic timestamp

      mcTime = point->GetTime();//Get MCTime
      eventTime = FairRootManager::Instance()->GetEventTime();
      hitTime = mcTime + eventTime;
      detectingTime = hitTime;
      smear(detectingTime,fdt);// smear with fdt to  creat realistic Time

      // Create new hit
      // new ((*fHitArray)[iPoint]) PndSciTHit(detectorID, detectorName, 
      // 			            time+FairRootManager::Instance()->GetEventTime(), fdt,
      //				    hitPosition,dHitPosition,
      //				    iPoint, 
      //				    point->GetEnergyLoss()); 

      PndSciTHit *tempHit = new PndSciTHit(detectorID, detectorName, 
					   detectingTime, fdt,
					   hitPosition,dHitPosition,
					   iPoint, 
					   point->GetEnergyLoss());

      fDataBuffer->FillNewData(tempHit, 
			       hitTime, 
			       hitTime+fDeadtime);

      delete tempHit;

    }   // Loop over MCPoints

  //fHitArray->Sort();
  // Event summary
  if (fVerbose>1) std::cout << "-I- PndSciTDigiTask: " << nPoints << " SciTPoints, "
       << nPoints << " Hits created." << std::endl;


}
// -------------------------------------------------------------------------


void PndSciTDigiTask::smear(Double_t& time, Double_t& dt)
{
/// smear a 3d vector

  Double_t t = time;
  //std::cout<<" time "<<time<<std::endl;
  Double_t sigt;
  
  sigt=gRandom->Gaus(0,dt);
  t += sigt;
  time = t;
  return;
}


ClassImp(PndSciTDigiTask)
