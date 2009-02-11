// -------------------------------------------------------------------------
// -----                PndMvdRecoTask source file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>
#include <vector>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "TRandom.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

// framework includes
#include "FairRootManager.h"
#include "PndMvdRecoTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "../pndbase/PndMCTrack.h"
#include "FairHit.h"
// PndMvd includes
#include "PndMvdMCPoint.h"
#include "PndMvdDigiPixel.h"
#include "PndMvdDigiStrip.h"
#include "PndMvdHit.h"

// #include "PndMvdHitMaker.h"
// #include "PndMvdHitMakerFactory.h"
// #include "PndMvdDetectorDescr.h"


// -----   Default constructor   -------------------------------------------
PndMvdRecoTask::PndMvdRecoTask() :
  fHitCovMatrix(3,3),
  FairTask("Reconstruction task for PANDA PndMvd")
{
/*  fSigmaX=0.;
  fSigmaY=0.;
  fSigmaZ=0.;*/
	fBranchName 	= "MVDDigi";
}
// -------------------------------------------------------------------------
/*
// -----   Constructor   ---------------------------------------------------
PndMvdRecoTask::PndMvdRecoTask(Double_t sx, Double_t sy, Double_t sz) :
  fHitCovMatrix(3,3),
  FairTask("Ideal reconstruction task for PANDA PndMvd")
{
  fSigmaX=sx;
  fSigmaY=sy;
  fSigmaZ=sz;
  fBranchName   = "MVDDigi";
}
// -------------------------------------------------------------------------

*/
// -----   Constructor   ---------------------------------------------------
PndMvdRecoTask::PndMvdRecoTask(std::string type) :
  FairTask("Reconstruction task for PANDA PndMvd")
{
  fMakerType = type;
  fBranchName   = "MVDDigi";
  if(fVerbose>0)
    std::cout<<type<<" Reconstruction task for PANDA PndMvd"<<std::endl;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndMvdRecoTask::~PndMvdRecoTask()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdRecoTask::Init()
{
  if(fVerbose>1)
    std::cout<<"PndMvdRecoTask::Init() called."<<std::endl;

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    std::cout << "-E- PndMvdRecoTask::Init: "
							<< "RootManager not instantiated!" << std::endl;
    return kFATAL;  }


  // Get MCTruth collection
//   fMctruthArray=(TClonesArray*) ioman->GetObject("MCTrack");
//   if(fMctruthArray==0)  {
//     std::cout << "-W- PndMvdRecoTask::Init: No McTruth array!" << std::endl;
//     return kERROR;  }


  // Get input array
  fDigiPixelArray=(TClonesArray*) ioman->GetObject("MVDPixelDigis");
  if(fDigiPixelArray==0)  {
    std::cout << "-W- PndMvdRecoTask::Init: No fDigiPixelArray!" << std::endl;
    return kERROR;  }

  // Get input array
  fDigiStripArray=(TClonesArray*) ioman->GetObject("MVDStripDigis");
  if(fDigiStripArray==0)  {
    std::cout << "-W- PndMvdRecoTask::Init: No fDigiStripArray!" << std::endl;
    return kERROR;  }

  // Create and register output array
  fHitOutputArray = new TClonesArray("PndMvdHit");
  ioman->Register("MVDHit", "PndMvd ideal Hits",
									fHitOutputArray, kTRUE);

/*
  PndMvdDetectorDescr(
       Double_t pixelCellX, Double_t pixelCellXLong, Double_t pixelCellY,
       Double_t stripX, Double_t stripY, Double_t feY, Double_t feX,
       Double_t rpStripPitch, Double_t zstripPitch,
       unsigned int numOfRows, unsigned int numOfColumns,
       unsigned int numNormalColumns, Double_t rowOffsetEdge,
       unsigned int zFE, unsigned int rphiFE, unsigned int channelsFE);
*/
       // TODO remove hardcoded numbers & improve detector description class
       Double_t pixelCellX = 0.01;
       Double_t pixelCellXLong = 0.03;
       Double_t pixelCellY = 0.01;
       Double_t stripX = 0.004921;//pitch in sensor coordinates
       Double_t stripY = 0.0042969;//
       Double_t feY = 0.82;
       Double_t feX = 0.76;
       Double_t rpStripPitch = 0.;//0.00859375;
       Double_t zstripPitch = 0.;//0.00984375;
       unsigned int numOfRows = 0;
       unsigned int numOfColumns = 0;
       unsigned int numNormalColumns = 0;
       Double_t rowOffsetEdge = 0.;
       unsigned int zFE = 0;
       unsigned int rphiFE = 0;
       unsigned int channelsFE = 128;
  if(fVerbose>1)
    std::cout<<"PndMvdRecoTask::Init(): Making PndMvdDetectorDescr "<<std::endl;

//   PndMvdDetectorDescr* detDescr = new PndMvdDetectorDescr(pixelCellX,pixelCellXLong,
//     pixelCellY,stripX,stripY,feY,feX,rpStripPitch,zstripPitch,numOfRows,
//     numOfColumns,numNormalColumns,rowOffsetEdge,zFE,rphiFE,channelsFE);
//   fHitMaker = PndMvdHitMakerFactory::theFactory().maker(fMakerType,detDescr);

//   if (fHitMaker == 0) return kERROR;

  fevent=0;

  if(fVerbose>1)
    std::cout<<"PndMvdRecoTask::Init() done."<<std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndMvdRecoTask::SetParContainers()
{
  // Get Base Container
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void PndMvdRecoTask::Exec(Option_t* opt)
{
	// Fills PndMvdHits with the MC Truth
  // TODO filling of RecoHits, together with the sensor plane
  fevent++;
  if (fVerbose > 0) {
    std::cout<<"PndMvdRecoTask::Exec() started event no. "<<fevent<<std::endl;
  }
  if ( ! fHitOutputArray ) Fatal("Exec", "No fHitOutputArray");
  fHitOutputArray->Clear();

  if ( ! fDigiPixelArray ) Fatal("Exec", "No fDigiPixelArray");
  if ( ! fDigiStripArray ) Fatal("Exec", "No fDigiStripArray");

  // preparing arrays
  std::vector<PndMvdDigiPixel*> mvdPixelDigis;
  std::vector<PndMvdDigiStrip*> mvdStripDigis;
  std::vector<PndMvdHit*> mvdHits;

  //converting TClonesarrays of Digis to std::vector
  PndMvdDigiPixel* aDigiPixel = 0;
  for (Int_t k = 0; k < fDigiPixelArray->GetEntriesFast(); k++)
  {
    aDigiPixel = (PndMvdDigiPixel*)fDigiPixelArray->At(k);
    if (aDigiPixel != 0) {
      mvdPixelDigis.push_back(aDigiPixel);
    }
  }
  PndMvdDigiStrip* aDigiStrip = 0;
  for (Int_t h = 0; h < fDigiStripArray->GetEntriesFast(); h++)
  {
    aDigiStrip = (PndMvdDigiStrip*)fDigiStripArray->At(h);
    if (aDigiStrip != 0) {
      mvdStripDigis.push_back(aDigiStrip);
    }
  }
  if (fVerbose > 2) {
    std::cout<<"  mvdStripDigis[0] = "<<mvdStripDigis[0]<<std::endl;
    std::cout<<"PndMvdRecoTask::Exec() conversion done, start hitmaking."<<std::endl;
  }

  //  ---
//   bool success = fHitMaker->makeHits(mvdPixelDigis,mvdStripDigis,mvdHits);
  //  ---

/*  if (!success){
    std::cerr << "making PndMvd hits was not successfull" << std::endl;
  }*/
  if (fVerbose > 2) {
    std::cout<<"PndMvdRecoTask::Exec() hitmaking done, reconvert now."<<std::endl;
  }

  //converting of std::vector Hits to TClonesarray
//   Int_t size = fHitOutputArray->GetEntriesFast();
//   Int_t s=0;
//   for (std::vector<PndMvdHit*>::iterator it = mvdHits.begin();
//        it != mvdHits.end(); it++)
//   { // size++ returns size and increases it then; ++size gives the increased one
//     s++;
//     if (*it==0){
//       std::cout<<"PndMvdRecoTask::Exec(): mvdHits["<<s
//                <<"] does not exist!"<<std::endl;
//       continue;
//     }
//     new ((*fHitOutputArray)[size++]) PndMvdHit(**it);
//     delete *it;
//   }
//
//   if (fVerbose > 0) {
//     std::cout<<fHitOutputArray->GetEntriesFast() <<" Hits created out of "
//              <<fDigiPixelArray->GetEntriesFast() <<" Pixel Digis and "
//              <<fDigiStripArray->GetEntriesFast() <<" Strip Digis."<<std::endl;
//   }

}





// -------------------------------------------------------------------------
void PndMvdRecoTask::InitTransMat()
{
  gGeoManager->cd(fCurrentPndMvdMCPoint->GetDetName());
  fCurrentTransMat = gGeoManager->GetCurrentMatrix();
  if (fVerbose > 1) {
    fCurrentTransMat->Print("");
  }
}

void PndMvdRecoTask::CalcDetPlane(TVector3& oVect, TVector3& uVect,TVector3& vVect)
{
  Double_t O[3], U[3], V[3], o[3], u[3], v[3];
  O[0]=oVect.x();  O[1]=oVect.y();  O[2]=oVect.z();
  U[0]=uVect.x();  U[1]=uVect.y();  U[2]=uVect.z();
  V[0]=vVect.x();  V[1]=vVect.y();  V[2]=vVect.z();

  if (fVerbose > 1) {
    std::cout<<"PndMvdRecoTask::CalcDetPlane from Detector "
             <<fCurrentPndMvdMCPoint->GetDetName()<<std::endl;
  }
  //make transformation
  fCurrentTransMat->LocalToMaster(O,o);
  fCurrentTransMat->LocalToMaster(U,u);
  fCurrentTransMat->LocalToMaster(V,v);
  oVect.SetXYZ(o[0],o[1],o[2]);
  uVect.SetXYZ(u[0],u[1],u[2]);
  vVect.SetXYZ(v[0],v[1],v[2]);
}


ClassImp(PndMvdRecoTask)
