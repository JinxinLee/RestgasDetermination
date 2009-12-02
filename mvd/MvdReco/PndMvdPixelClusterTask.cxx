// -------------------------------------------------------------------------
// -----                PndMvdPixelClusterTasksource file             -----
// -------------------------------------------------------------------------


#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"

#include "PndStringVector.h"

#include "PndMvdPixelClusterTask.h"
#include "PndMvdMCPoint.h"
#include "PndMvdCalcPixel.h"
#include "PndMvdCalcFePixel.h"
#include "PndMvdDigiPixel.h"
// #include "PndMvdPixelCluster.h"
#include "PndMvdCluster.h"
#include "PndMvdGeoHandling.h"

#include "PndMvdSimplePixelClusterFinder.h"
#include "PndMvdChargeWeightedPixelMapping.h"

// -----   Default constructor   -------------------------------------------
PndMvdPixelClusterTask::PndMvdPixelClusterTask() :
  FairTask("MVD Clustertisation Task")
{
  fBranchName   = "MVDPixelDigis";

//  fDigiArray  = new TClonesArray("PndMvdDigiPixel");
//  fClusterArray  = new TClonesArray("PndMvdClusterPixel");
}
// -------------------------------------------------------------------------

PndMvdPixelClusterTask::PndMvdPixelClusterTask(Double_t radius, TString geoFile) :
  FairTask("MVD Pixel Clustertization Task")
{
  fBranchName   = "MVDPixelDigis";
//  fDigiArray  = new TClonesArray("PndMvdDigiPixel");
//  fClusterArray  = new TClonesArray("PndMvdClusterPixel");
  fRadius = radius;
  fParams.push_back(radius);
  fGeoFile = geoFile;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdPixelClusterTask::~PndMvdPixelClusterTask()
{
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdPixelClusterTask::SetParContainers()
{
  // Get Base Container
	FairRun* ana = FairRun::Instance();
	FairRuntimeDb* rtdb=ana->GetRuntimeDb();
	fDigiPar = (PndMvdPixelDigiPar*)(rtdb->getContainer("MVDPixelDigiPar"));
}

InitStatus PndMvdPixelClusterTask::ReInit()
{

  InitStatus stat=kERROR;
  return stat;

  /*
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar=(PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));

  return kSUCCESS;
  */
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdPixelClusterTask::Init()
{

  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndMvdPixelClusterTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fDigiArray = (TClonesArray*) ioman->GetObject(fBranchName);

  if ( ! fDigiArray )
    {
      std::cout << "-W- PndMvdPixelClusterTask::Init: "
     << "No MVDDigi array!" << std::endl;
      return kERROR;
  }

  fHitArray = new TClonesArray("PndMvdHit");
  ioman->Register("MVDHitsPixel", "MVD", fHitArray, kTRUE);

  fClusterArray = new TClonesArray("PndMvdCluster");
  ioman->Register("MVDClusterCand","MVD",fClusterArray,kTRUE);

  fParams.push_back(fDigiPar->GetFECols());
  fParams.push_back(fDigiPar->GetFERows());

  fDigiPar->Print();

  std::cout << "-I- PndMvdPixelClusterTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndMvdPixelClusterTask::Exec(Option_t* opt)
{
  std::vector<PndMvdDigiPixel> DigiPixelArray;
  // Reset output array
  if ( ! fClusterArray ) Fatal("Exec", "No ClusterArray");
  fClusterArray->Delete();

  if ( ! fHitArray ) Fatal("Exec", "No HitArray");
  fHitArray->Delete();

  Int_t nPoints = fDigiArray->GetEntriesFast();

  // convert from TClonesarray to a std::vector
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++){
    PndMvdDigiPixel myDigi = *(PndMvdDigiPixel*)(fDigiArray->At(iPoint));
    DigiPixelArray.push_back(myDigi);
  }

  // load the Clusterfinder
  PndMvdSimplePixelClusterFinder finder(fParams, DigiPixelArray);
  finder.SetVerbose(fVerbose);
  // Retrieve the calculated clusters
  std::vector< std::vector< Int_t> > clusters = finder.GetClusters();
  if(fVerbose>1) std::cout << " -I-  PndMvdPixelClusterTask::Exec(): We have "<<clusters.size()<<" pixel clusters" << std::endl;
  // store the list
  for (Int_t i = 0; i < clusters.size(); i++)
  {
    new((*fClusterArray)[i]) PndMvdCluster(clusters[i]);
  }

  std::vector<Double_t> mappingPar;
  mappingPar.push_back(fParams[1]);
  mappingPar.push_back(fParams[2]);
//   mappingPar.push_back(0.01);
//   mappingPar.push_back(0.01);
  mappingPar.push_back(fDigiPar->GetXPitch());
  mappingPar.push_back(fDigiPar->GetYPitch());

  // do the backmapping with charge-weight
  for (Int_t i = 0; i < clusters.size(); i++)
  {
//    if(fVerbose>2) std::cout << clusters[i].size() << " " << std::endl;
    std::vector<PndMvdDigiPixel> clusterArray;
    for (Int_t j=0;j < clusters[i].size();j++)
    { // convert
      clusterArray.push_back(DigiPixelArray[clusters[i][j]]);
    }
    // mapping
    PndMvdChargeWeightedPixelMapping mapping(clusterArray, mappingPar);
    mapping.SetVerbose(fVerbose);
    PndMvdHit myHit = mapping.GetCluster();
    myHit.SetClusterIndex(i);
    if(fVerbose>1){
      std::cout << " -I-  PndMvdPixelClusterTask::Exec(): Calculated Hit: " << std::endl;
      myHit.Print();
    }
    new ((*fHitArray)[i]) PndMvdHit(myHit);
  }
  if(fVerbose>1)std::cout << std::endl;
  if(fVerbose>0){
    std::cout << "-I- PndMvdPixelClusterTask: " << fClusterArray->GetEntriesFast()
              << " Mvd Clusters and " << fHitArray->GetEntriesFast()<<" Hits calculated." << std::endl;
  }
  return;

}

ClassImp(PndMvdPixelClusterTask);

