// -------------------------------------------------------------------------
// -----                PndSdsPixelClusterTasksource file             -----
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

#include "PndSdsPixelClusterTask.h"
#include "PndSdsMCPoint.h"
#include "PndSdsCalcPixel.h"
#include "PndSdsCalcFePixel.h"
#include "PndSdsDigiPixel.h"
// #include "PndSdsPixelCluster.h"
#include "PndSdsClusterPixel.h"
#include "PndSdsGeoHandling.h"

#include "PndSdsSimplePixelClusterFinder.h"
#include "PndSdsChargeWeightedPixelMapping.h"
#include "PndMCList.h" 

// -----   Default constructor   -------------------------------------------
PndSdsPixelClusterTask::PndSdsPixelClusterTask() :
  FairTask("SDS Clustertisation Task")
{

}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndSdsPixelClusterTask::~PndSdsPixelClusterTask()
{
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------

InitStatus PndSdsPixelClusterTask::ReInit()
{

  InitStatus stat=kERROR;
  return stat;

  /*
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar=(PndSdsGeoPar*)(rtdb->getContainer("PndSdsGeoPar"));

  return kSUCCESS;
  */
}

// -----   Public method Init   --------------------------------------------
InitStatus PndSdsPixelClusterTask::Init()
{
  SetBranchNames();

  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndSdsPixelClusterTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fDigiArray = (TClonesArray*) ioman->GetObject(fBranchName);

  if ( ! fDigiArray )
    {
      std::cout << "-W- PndSdsPixelClusterTask::Init: "
     << "No SDSDigi array!" << std::endl;
      return kERROR;
  }

  fHitArray = new TClonesArray("PndSdsHit");
  ioman->Register(fHitBranchName, fFolderName, fHitArray, kTRUE);

  fClusterArray = new TClonesArray("PndSdsClusterPixel");
  ioman->Register(fClustBranchName, fFolderName, fClusterArray, kTRUE);

  fParams.push_back(fDigiPar->GetFECols());
  fParams.push_back(fDigiPar->GetFERows());
  fParams.push_back(fDigiPar->GetClustRadius()); 
  
  fDigiPar->Print();

  std::cout << "-I- PndSdsPixelClusterTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndSdsPixelClusterTask::Exec(Option_t* opt)
{
  std::vector<PndSdsDigiPixel> DigiPixelArray;
  // Reset output array
  if ( ! fClusterArray ) Fatal("Exec", "No ClusterArray");
  fClusterArray->Delete();

  if ( ! fHitArray ) Fatal("Exec", "No HitArray");
  fHitArray->Delete();

  Int_t nPoints = fDigiArray->GetEntriesFast();

  // convert from TClonesarray to a std::vector
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++){
    PndSdsDigiPixel myDigi = *(PndSdsDigiPixel*)(fDigiArray->At(iPoint));
    DigiPixelArray.push_back(myDigi);
  }

  // load the Clusterfinder
  PndSdsSimplePixelClusterFinder finder(fParams, DigiPixelArray);
  finder.SetVerbose(fVerbose);
  // Retrieve the calculated clusters
  std::vector< std::vector< Int_t> > clusters = finder.GetClusters();
  if(fVerbose>1) std::cout << " -I-  PndSdsPixelClusterTask::Exec(): We have "<<clusters.size()<<" pixel clusters" << std::endl;
  // store the list
  for (Int_t i = 0; i < clusters.size(); i++)
  {
    new((*fClusterArray)[i]) PndSdsClusterPixel(clusters[i]);
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
    std::vector<PndSdsDigiPixel> clusterArray;
    for (Int_t j=0;j < clusters[i].size();j++)
    { // convert
      clusterArray.push_back(DigiPixelArray[clusters[i][j]]);
    }
    // mapping
    PndSdsChargeWeightedPixelMapping mapping(clusterArray, mappingPar);
    mapping.SetVerbose(fVerbose);
    PndSdsHit myHit = mapping.GetCluster();
    myHit.SetClusterIndex(kMVDClusterPixel,i);
    if(fVerbose>1){
      std::cout << " -I-  PndSdsPixelClusterTask::Exec(): Calculated Hit: " << std::endl;
      myHit.Print();
    }
    new ((*fHitArray)[i]) PndSdsHit(myHit);
  }
  if(fVerbose>1)std::cout << std::endl;
  if(fVerbose>0){
    std::cout << "-I- PndSdsPixelClusterTask: " << fClusterArray->GetEntriesFast()
              << " Sds Clusters and " << fHitArray->GetEntriesFast()<<" Hits calculated." << std::endl;
  }
  return;

}

ClassImp(PndSdsPixelClusterTask);

