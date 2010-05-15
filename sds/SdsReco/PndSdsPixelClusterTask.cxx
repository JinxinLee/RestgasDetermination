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

#include "PndStringSeparator.h"

#include "PndSdsPixelClusterTask.h"
#include "PndSdsMCPoint.h"
#include "PndSdsCalcPixel.h"
#include "PndSdsCalcFePixel.h"
#include "PndSdsDigiPixel.h"
// #include "PndSdsPixelCluster.h"
#include "PndSdsClusterPixel.h"
#include "PndGeoHandling.h"

#include "PndSdsSimplePixelClusterFinder.h"
#include "PndSdsChargeWeightedPixelMapping.h"

// -----   Default constructor   -------------------------------------------
PndSdsPixelClusterTask::PndSdsPixelClusterTask() :
FairTask("SDS Clustertisation Task"), fPersistance(kTRUE)
{
  fGeoH=NULL;
}
// -------------------------------------------------------------------------

// -----   Named constructor   ---------------------------------------------
PndSdsPixelClusterTask::PndSdsPixelClusterTask(const char* name) :
FairTask(name), fPersistance(kTRUE)
{
  fGeoH=NULL;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSdsPixelClusterTask::~PndSdsPixelClusterTask()
{
	if (fBackMapping != 0) delete fBackMapping;
	if (fClusterFinder != 0) delete fClusterFinder;
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndSdsPixelClusterTask::SetParContainers()
{
  if(fVerbose>1) Info("SetParContainers","make geohandler");
	if(0==fGeoH) fGeoH = new PndGeoHandling();
  else if(fVerbose>1) Warning("SetParContainers","ooops there was already a geohandler");
  fGeoH->SetVerbose(fVerbose);
  if( ! fDigiPar) Fatal("SetParContainers","No digitiztiopn parameters specified");
  if(fVerbose>1) Info("SetParContainers","done.");
  return;
}


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
  SetBackMapping();
  SetClusterFinder();
  SetClusterType();
  
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
  ioman->Register(fHitBranchName, fFolderName, fHitArray, fPersistance);
  
  fClusterArray = new TClonesArray("PndSdsClusterPixel");
  ioman->Register(fClustBranchName, fFolderName, fClusterArray, fPersistance);
  
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
  // Retrieve the calculated clusters with the choosen clusterfinder
  std::vector< std::vector< Int_t> > clusters = fClusterFinder->GetClusters(DigiPixelArray);
  if(fVerbose>1) std::cout << " -I-  PndSdsPixelClusterTask::Exec(): We have "<<clusters.size()<<" pixel clusters" << std::endl;
  // store the list
  for (UInt_t i = 0; i < clusters.size(); i++)
  {
    new((*fClusterArray)[i]) PndSdsClusterPixel(clusters[i]);
  }
  
  // do the backmapping with charge-weight
  for (UInt_t i = 0; i < clusters.size(); i++)
  {
    //    if(fVerbose>2) std::cout << clusters[i].size() << " " << std::endl;
    std::vector<PndSdsDigiPixel> clusterArray;
    for (UInt_t j=0;j < clusters[i].size();j++)
    { // convert
      clusterArray.push_back(DigiPixelArray[clusters[i][j]]);
    }
    
    // mapping with the choosen back mapping
    PndSdsHit myHit = fBackMapping->GetCluster(clusterArray);
    myHit.SetClusterIndex(fClusterType,i);
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

