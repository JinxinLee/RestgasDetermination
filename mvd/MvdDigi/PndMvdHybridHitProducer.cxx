// -------------------------------------------------------------------------
// -----                CbmStsHitProducerIdeal source file             -----
// -----                  Created 10/01/06  by V. Friese               -----
// -------------------------------------------------------------------------


#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "PndMvdHybridHitProducer.h"
#include "PndMvdMCPoint.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"
#include "PndStringVector.h"
#include "PndMvdCalcPixel.h"
#include "PndMvdCalcFePixel.h"
#include "PndMvdDigiPixel.h"

//TODO this include is for the enumeration
#include "PndMvdDetector.h"

// -----   Default constructor   -------------------------------------------
PndMvdHybridHitProducer::PndMvdHybridHitProducer() :
  FairTask("MVD Hybrid Hit Producer")
{
  fBranchName   = "MVDPoint";
  fPixelHits = 0;
  fEventNr = 0;
  fOverwriteParams = kFALSE;
 // fGeoH = new PndMvdGeoHandling(gGeoManager);
//  fHitArray  = new TClonesArray("PndMvdHit");
//	fPixelArray	= new TClonesArray("PndMvdPixelHit");
}
// -------------------------------------------------------------------------

PndMvdHybridHitProducer::PndMvdHybridHitProducer(Double_t lx, Double_t ly, Double_t threshold, Double_t noise) :
  FairTask("MVD Hybrid Digi Producer (PndMvdHybridHitProducer)")
{
  fBranchName   = "MVDPoint";
//  fHitArray  = new TClonesArray("PndMvdHit");
//	fPixelArray	= new TClonesArray("PndMvdPixelHit");
  flx = lx;
  fly = ly;
  fthreshold = threshold;
  fnoise = noise;
  fPixelHits = 0;
  fEventNr = 0;
  fcols = 104;
  frows = 104;
  fOverwriteParams = kTRUE;
	std::cout << "MVD Hybrid Digi Producer initiated" << std::endl;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMvdHybridHitProducer::~PndMvdHybridHitProducer()
{
	delete fGeoH;
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdHybridHitProducer::SetParContainers()
{
  // called before Init()
  // Get Base Container
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fDigiPar = (PndMvdPixelDigiPar*)(rtdb->getContainer("MVDPixelDigiPar"));
}

InitStatus PndMvdHybridHitProducer::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdHybridHitProducer::Init()
{
    FairRun* ana = FairRun::Instance();
  FairRootManager* ioman = FairRootManager::Instance();
    fGeoH = new PndMvdGeoHandling(gGeoManager);
  if ( ! ioman )
    {
      std::cout << "-E- PndMvdHybridHitProducer::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }
  fPointArray = (TClonesArray*) ioman->GetObject(fBranchName);

  if ( ! fPointArray )
    {
      std::cout << "-W- PndMvdHybridHitProducer::Init: "
     << "No MVDPoint array!" << std::endl;
      return kERROR;
  }

  // Create and register output array
//  fHitArray = new TClonesArray("PndMvdHit");
//  ioman->Register("MVDHit", "MVD", fHitArray, kTRUE);

  // Create and register output array
  fPixelArray = new TClonesArray("PndMvdDigiPixel");
  ioman->Register("MVDPixelDigis", "MVD", fPixelArray, kTRUE);



  if(fOverwriteParams==kTRUE){
    fDigiPar->SetXPitch(flx);
    fDigiPar->SetYPitch(fly);
    fDigiPar->SetThreshold(fthreshold);
    fDigiPar->SetNoise(fnoise);
    fDigiPar->SetFECols(fcols);
    fDigiPar->SetFERows(frows);
    fDigiPar->setInputVersion(ana->GetRunId(),1);
    fDigiPar->setChanged();
  }
  fDigiPar->Print();

    flx = fDigiPar->GetXPitch();
    fly = fDigiPar->GetYPitch();
    fthreshold = fDigiPar->GetThreshold();
    fnoise = fDigiPar->GetNoise();
    fcols = fDigiPar->GetFECols();
    frows = fDigiPar->GetFERows();

   std::cout << "-I- PndMvdHybridHitProducer: Intialisation successfull" << std::endl;

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMvdHybridHitProducer::Exec(Option_t* opt)
{
  // Reset output array
  if ( ! fPixelArray )
    Fatal("Exec", "No PixelArray");
  //   fHitArray->Clear();
  fPixelArray->Clear();
  fPixelList.clear();
//   fFePixelArray->Clear();

  // Declare some variables
  PndMvdMCPoint *point = NULL;

//  Int_t detID = 0;      // Detector ID


  // Loop over PndMvdMCPoints
  Int_t
    nPoints = fPointArray->GetEntriesFast();
  Int_t iPixel = 0;
  Int_t iFePixel = 0;
  fPixelHits = 0;

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++)
  {
      point = (PndMvdMCPoint*) fPointArray->At(iPoint);
      if ( ! point){
        std::cout<< "No Point!" << std::endl;
         continue;
      }
      if (fVerbose > 1){
        std::cout << "****Global Point: " << std::endl;
        point->Print("");
      }
      FairGeoVector posInL, posOutL, meanPos, meanPosL;
      GetLocalHitPoints(point, posInL, posOutL);

      if (fVerbose > 1){
        std::cout << "posOutL: " << std::endl;
        std::cout << posInL.X() << " " << posInL.Y() << " " << posInL.Z() << std::endl;
        std::cout << posOutL.X() << " " << posOutL.Y() << " " << posOutL.Z() << std::endl;
      }
      meanPosL = posInL + posOutL;
      meanPosL /= 2;
      if (fVerbose > 1){
        meanPosL.Print();
        std::cout << "Energy: " << point->GetEnergyLoss() << std::endl;
        std::cout << point->GetDetName() << std::endl;
        std::cout << fGeoH->GetPath(point->GetDetName()) << std::endl;
      }
      std::vector<PndMvdPixel> myFePixels;
      if (   fGeoH->GetPath(point->GetDetName()).Contains("Pixel")
           ||
           ( fGeoH->GetPath(point->GetDetName()).Contains("Sensor") &&
            !fGeoH->GetPath(point->GetDetName()).Contains("Strip")     )
         )
      {
        // Define sensor by pixelsizes threshold and noise from macro outside
        PndMvdCalcPixel PixelCalc(flx, fly, fthreshold, fnoise);
        // Calculate a cluster of Pixels fired (in sensor system)
        std::vector<PndMvdPixel> myPixels = PixelCalc.GetPixels (posInL.getX(), posInL.getY(), posInL.getZ(),
                                            posOutL.getX(), posOutL.getY(), posOutL.getZ(),
                                            point->GetEnergyLoss());
        if (myPixels.size() == 0){
          if (fVerbose > 1) std::cout << "Deposited charge below threshold" << std::endl;
        } else {
          fPixelHits += myPixels.size();
          if (fVerbose > 1) std::cout  << "SensorPixels: " << std::endl;
          for(UInt_t i = 0; i < myPixels.size(); i++)
          {
            myPixels[i].SetDetName(point->GetDetName().Data());
            if (fVerbose > 1) std::cout << myPixels[i] << std::endl;
          }
          // TODO read Parameters from Database
          // Calculate channel numbers
          PndMvdCalcFePixel feCalc(fcols, frows, 10);
          myFePixels = feCalc.CalcFEHits(myPixels);
          if (fVerbose > 1){
            std::cout << "FePixels: " << myFePixels.size() << std::endl;
            for(UInt_t i = 0; i < myFePixels.size(); i++){
               std::cout << myFePixels[i] << std::endl;
            }
          }
          AddHits(&myFePixels, iPoint);

//          detID   = point->GetDetectorID();

//          for (UInt_t iPix = 0; iPix < myFePixels.size(); iPix++){
//            new ((*fPixelArray)[iFePixel++])
//                  PndMvdDigiPixel( iPoint, detID,   point->GetDetName(),myFePixels[iPix].GetFE(),
//                               myFePixels[iPix].GetCol(), myFePixels[iPix].GetRow(),
//                               myFePixels[iPix].GetCharge());
//          }
        }//endif myPixels.size

      }//endif pixel


    }   // Loop over MCPoints
  for (int iPix = 0; iPix < fPixelList.size(); iPix++){
    if (fPixelList[iPix].GetCharge()<=fthreshold) continue;
	  if (fVerbose > 1)  std::cout << fPixelList[iPix] << std::endl;
	    new ((*fPixelArray)[iFePixel++])
	          PndMvdDigiPixel( fPixelList[iPix].GetFirstMCIndex(), kMVDHitsPixel, fPixelList[iPix].GetDetName() ,fPixelList[iPix].GetFE(),
	                       fPixelList[iPix].GetCol(), fPixelList[iPix].GetRow(),
	                       fPixelList[iPix].GetCharge());
//	    new ((*fPixelArray)[iFePixel++])
//	          PndMvdDigiPixel( fPixelList[iPix].GetFirstMCIndex(), detID, fPixelList[iPix].GetDetName() ,fPixelList[iPix].GetFE(),
//	                       fPixelList[iPix].GetCol(), fPixelList[iPix].GetRow(),
//	                       fPixelList[iPix].GetCharge());
  }

  // Event summary

  if (fVerbose > 0){
    std::cout << "-I- PndMvdHybridHitProducer: " << nPoints << " PndMvdMCPoints, "
              << fPixelHits << " Digi created." << " " << iFePixel
              << "  (event "<<fEventNr++ <<")"<< std::endl;
  }
}

void PndMvdHybridHitProducer::GetLocalHitPoints(PndMvdMCPoint* myPoint, FairGeoVector& myHitIn, FairGeoVector& myHitOut)
{

  if (fVerbose > 1)
    std::cout << "GetLocalHitPoints" << std::endl;
  TGeoHMatrix trans = GetTransformation(myPoint->GetDetName().Data());

  Double_t posIn[3];
  Double_t posOut[3];
  Double_t posInLocal[3];
  Double_t posOutLocal[3];

  posIn[0] = myPoint->GetX();
  posIn[1] = myPoint->GetY();
  posIn[2] = myPoint->GetZ();

  posOut[0] = myPoint->GetXOut();
  posOut[1] = myPoint->GetYOut();
  posOut[2] = myPoint->GetZOut();

  if (fVerbose > 1){
    for (Int_t i = 0; i < 3; i++)
      std::cout << "posIn "<< i << ": " << posIn[i] << std::endl;

    trans.Print("");
  }

  trans.MasterToLocal(posIn, posInLocal);
  trans.MasterToLocal(posOut, posOutLocal);

  if (fVerbose > 1) {
    for (Int_t i = 0; i < 3; i++){
      std::cout << "posInLocal "<< i << ": " << posInLocal[i] << std::endl;
      std::cout << "posOutLocal "<< i << ": " << posOutLocal[i] << std::endl;
    }
  }


  //posIn/OutLocal have the center of the coordinate system in the center of the shape
  //typically sensors have their coordinate system centered at the lower left corner

  TVector3 offset = GetSensorDimensions(myPoint->GetDetName().Data());

  if (fVerbose > 1){
	  std::cout << "SensorDimension for: " << myPoint->GetDetName().Data() << std::endl;
	  std::cout << offset.X() << " " << offset.Y() << " " << offset.Z() << std::endl;
  }
  
  posInLocal[0] += offset.x();
  posInLocal[1] += offset.y();
  //posInLocal[2] += offset.z();

  posOutLocal[0] += offset.x();
  posOutLocal[1] += offset.y();
  //posOutLocal[2] += offset.z();


  myHitIn.setVector(posInLocal);
  myHitOut.setVector(posOutLocal);

}

TGeoHMatrix PndMvdHybridHitProducer::GetTransformation(std::string detName)
{
  //PndMvdGeoHandling GeoH(gGeoManager);
  gGeoManager->cd(fGeoH->GetPath(detName.c_str()));
  TGeoHMatrix* transMat = gGeoManager->GetCurrentMatrix();
  if (fVerbose > 1)
  transMat->Print("");
  return *transMat;
}

TVector3 PndMvdHybridHitProducer::GetSensorDimensions(std::string detName)
{
  //PndMvdGeoHandling GeoH(gGeoManager);
  gGeoManager->cd(fGeoH->GetPath(detName.c_str()));
  TGeoVolume* actVolume = gGeoManager->GetCurrentVolume();
  TGeoBBox* actBox = (TGeoBBox*)(actVolume->GetShape());
  TVector3 result;
  result.SetX(actBox->GetDX());
  result.SetY(actBox->GetDY());
  result.SetZ(actBox->GetDZ());

  //result.Dump();

  return result;
}

void PndMvdHybridHitProducer::AddHits(std::vector<PndMvdPixel>* hits, int mcIndex)
{
	for (UInt_t i = 0; i < hits->size(); i++){
		AddHit(hits->at(i), mcIndex);
	}
	if (fVerbose > 1) std::cout << "Size of fPixelList: " << fPixelList.size() << std::endl;
}

void PndMvdHybridHitProducer::AddHit(PndMvdPixel& hit, int mcIndex)
{
	bool found = false;
	if(fVerbose > 1) std::cout << "Adding Hits to PixelList with mcIndex: " << mcIndex << std::endl;
	for (UInt_t i = 0; i < fPixelList.size() && found == false; i++){
		if ( fPixelList[i].GetDetName() == hit.GetDetName() &&
			 fPixelList[i].GetFE() == hit.GetFE() &&
			 fPixelList[i].GetCol() == hit.GetCol() &&
			 fPixelList[i].GetRow() == hit.GetRow() )
		{
			if (fVerbose > 1) std::cout << "Pixel " << hit.GetDetName()
										<< " FE/col/row " << hit.GetFE()
										<< "/" << hit.GetCol()
										<< "/" << hit.GetRow() << " already hit!"<< std::endl;
			fPixelList[i].AddCharge(hit.GetCharge());
			fPixelList[i].AddMCIndex(mcIndex);
			found = true;
		}
	}
	if (found == false){
		hit.AddMCIndex(mcIndex);
		fPixelList.push_back(hit);
	}
}
// -------------------------------------------------------------------------


ClassImp(PndMvdHybridHitProducer);
