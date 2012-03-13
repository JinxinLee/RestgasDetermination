// -------------------------------------------------------------------------
// -----                FairSciTProducerIdeal source file             -----
// -----                  Created by A. Sanchez              -----
// -------------------------------------------------------------------------

#include <cmath>

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "FairRootManager.h"
#include "PndSciTHitProducerIdeal.h"
#include "PndSciTHit.h"
#include "TGeoBBox.h"
#include "PndSciTPoint.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoVector.h"
#include "TVector3.h"


// -----   Default constructor   -------------------------------------------
PndSciTHitProducerIdeal::PndSciTHitProducerIdeal() :
  FairTask("Ideal PndSciT Hit Producer") 
{
	fBranchName 	= "SciTPoint";

}
// -------------------------------------------------------------------------

// -----   Default constructor   -------------------------------------------
PndSciTHitProducerIdeal::PndSciTHitProducerIdeal(Double_t dt) :
  FairTask("Ideal PndSciT Hit Producer") 
{
	fBranchName 	= "SciTPoint";

	fdt= dt;
	
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSciTHitProducerIdeal::~PndSciTHitProducerIdeal() 
{ 
}

// -----   Public method Init   --------------------------------------------
InitStatus PndSciTHitProducerIdeal::Init() 
{
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  
  if ( ! ioman ) 
    {
      std::cout << "-E- PndSciTHitProducerIdeal::Init: "
	   << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject(fBranchName);

  if ( ! fPointArray ) 
    {
      std::cout << "-W- PndSciTHitProducerIdeal::Init: "
	   << "No SciTPoint array!" << std::endl;
      return kERROR;
  }

 

  // Create and register output array
  fHitArray = ioman->Register("SciTHit", "PndSciTHit", "SciT", kTRUE);

  std::cout << "-I- PndSciTHitProducerIdeal: Intialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndSciTHitProducerIdeal::SetParContainers()
{
  // Get Base Container
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  

}


// -----   Public method Exec   --------------------------------------------
void PndSciTHitProducerIdeal::Exec(Option_t* opt) 
{
  // Reset output array
  if ( ! fHitArray ) 
    Fatal("Exec", "No HitArray");
  
  fHitArray->Clear();
 
  
  // Declare some variables
  PndSciTPoint *point = 0;

  Int_t 
    detID = 0,       // Detector ID
    trackID = 0;     // Track index


  Double_t time = 0.;
Double_t t1 = 0;
  

  // Loop over SciTPoints
  Int_t 
    nPoints = fPointArray->GetEntriesFast();

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) 
    {
      point = (PndSciTPoint*) fPointArray->At(iPoint);
	std::cout << " Ideal Hit Producer -Point-: " << point << std::endl;
      if ( ! point) 
	continue;

      // Detector ID
      detID = point->GetVolumeID();

      // MCTrack ID
      trackID = point->GetTrackID();

      FairGeoVector posCInL, posCOut, meanPos, meanPosL;
     // Position in the middle of the sensor
      GetLocalHitPoints(point, meanPos);

      
     
      
      TVector3 dpos; 

      dpos.SetXYZ(0.,0.,0.);

  
      TVector3 position(meanPos.getX(), 
			meanPos.getY(), 
			meanPos.getZ());
     
//      std::cout<<" x "<<position.x()<<" y "<<position.y()<<" z "<<std::endl;
      
      
      time = point->GetTime();
      
      //t1 = fd;//100 ps time resolution
      smear(time,fdt);
 
      // Create new hit
      new ((*fHitArray)[iPoint]) PndSciTHit(trackID, detID, 
					   point->GetDetName(),time, fdt, 
					   position,dpos,iPoint, 
					   point->GetEnergyLoss());

      
      
      
    }   // Loop over MCPoints

 
 
  
  // Event summary
  std::cout << "-I- PndSciTHitProducerIdeal: " << nPoints << " SciTPoints, "
       << nPoints << " Hits created." << std::endl;
  

}
// -------------------------------------------------------------------------
void PndSciTHitProducerIdeal::smear(Double_t& time, Double_t& dt)
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

void PndSciTHitProducerIdeal::GetLocalHitPoints(PndSciTPoint* myPoint, 
					       FairGeoVector& myHitIn)
{
 	 
  if (fVerbose > 0)
    std::cout << "GetLocalHitPoints" << std::endl;
  TGeoHMatrix trans = GetTransformation(myPoint->GetDetName().Data());
  std::cout<<" name "<<myPoint->GetDetName().Data()<<std::endl;
  
  
  Double_t posIn[3];
  Double_t posOut[3];

  

    for (Int_t i = 0; i < 3; i++){
      posIn[i] = 0.;
      posOut[i] = 0.;
	}
     if (fVerbose > 0) trans.Print("");
  
  trans.LocalToMaster(posIn, posOut);

  
  if (fVerbose > 0) {
    for (Int_t i = 0; i < 3; i++){
      
      std::cout << "posOut "<< i << ": " << posOut[i] << std::endl;
    }
  }

  myHitIn.setVector(posOut);
  

}


 	
 	
TGeoHMatrix PndSciTHitProducerIdeal::GetTransformation(std::string detName) const
{
  gGeoManager->cd(detName.c_str());
  TGeoHMatrix* transMat = gGeoManager->GetCurrentMatrix();
  if (fVerbose > 1)
    transMat->Print("");
  return *transMat;
}

TVector3 PndSciTHitProducerIdeal::GetSensorDimensions(std::string detName) const
	{
 	  gGeoManager->cd(detName.c_str());
 	  TGeoVolume* actVolume = gGeoManager->GetCurrentVolume();
 	  TGeoBBox* actBox = (TGeoBBox*)(actVolume->GetShape());
 	  TVector3 result;
 	  result.SetX(actBox->GetDX());
 	  result.SetY(actBox->GetDY());
 	  result.SetZ(actBox->GetDZ());
 	 
 	  //result.Dump();
 	 
 	  return result;
 	}

ClassImp(PndSciTHitProducerIdeal)
