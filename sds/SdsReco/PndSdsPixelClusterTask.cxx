// -------------------------------------------------------------------------
// -----                PndSdsPixelClusterTasksource file             -----
// -------------------------------------------------------------------------


#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"

#include "PndStringSeparator.h"

#include "PndSdsPixelClusterTask.h"
#include "PndSdsMCPoint.h"
#include "PndSdsCalcPixel.h"
//#include "PndSdsCalcFePixel.h"
#include "PndSdsDigiPixel.h"
// #include "PndSdsPixelCluster.h"
#include "PndSdsClusterPixel.h"
#include "PndGeoHandling.h"

#include "PndSdsSimplePixelClusterFinder.h"
#include "PndSdsChargeWeightedPixelMapping.h"

// -----   Default constructor   -------------------------------------------
PndSdsPixelClusterTask::PndSdsPixelClusterTask() :
PndSdsTask("SDS Clustertisation Task"), fPersistance(kTRUE), fClusterType(-1)
{
  fGeoH = PndGeoHandling::Instance();
}
// -------------------------------------------------------------------------

// -----   Named constructor   ---------------------------------------------
PndSdsPixelClusterTask::PndSdsPixelClusterTask(const char* name) :
PndSdsTask(name), fPersistance(kTRUE), fClusterType(-1)
{
  fGeoH = PndGeoHandling::Instance();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSdsPixelClusterTask::~PndSdsPixelClusterTask()
{
	if (fBackMapping != 0) delete fBackMapping;
	if (fClusterFinder != 0) delete fClusterFinder;
	if (fFunctor != 0) delete fFunctor;
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndSdsPixelClusterTask::SetParContainers()
{
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
  
  FairRootManager* ioman = FairRootManager::Instance();
  
  if ( ! ioman )
  {
    std::cout << "-E- PndSdsPixelClusterTask::Init: "
    << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  
  // Get input array
//  fDigiArray = (TClonesArray*) ioman->GetObject(fInBranchName);
//
//  if ( ! fDigiArray )
//  {
//    std::cout << "-W- PndSdsPixelClusterTask::Init: "
//    << "No SDSDigi array!" << std::endl;
//    return kERROR;
//  }
  
  fClusterArray = new TClonesArray("PndSdsClusterPixel");
  ioman->Register(fClustBranchName, fFolderName, fClusterArray, fPersistance);

  fHitArray = new TClonesArray("PndSdsHit");
  ioman->Register(fOutBranchName, fFolderName, fHitArray, fPersistance);
  
  SetInBranchId();

  fFunctor = new StopTime();

  if(fVerbose>1) fDigiPar->Print();
  
  std::cout << "-I- PndSdsPixelClusterTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndSdsPixelClusterTask::Exec(Option_t* opt)
{
	Int_t eventNr = 0;
  std::vector<PndSdsDigiPixel> DigiPixelArray;
  // Reset output array
  if ( ! fClusterArray ) Fatal("Exec", "No ClusterArray");
  fClusterArray->Delete();
  fGeoH->SetVerbose(fVerbose);
  
  std::cout << "EventTime: " << FairRootManager::Instance()->GetEventTime() << std::endl;

    if (fDigiArray > 0)
  	  fDigiArray->Clear();

    if (FairRunAna::Instance()->IsTimeStamp())
  	  fDigiArray = FairRootManager::Instance()->GetData(fInBranchName, fFunctor, FairRootManager::Instance()->GetEventTime() + 10); //FairRootManager::Instance()->GetEventTime() +
    else
  	  fDigiArray = FairRootManager::Instance()->GetData(fInBranchName, 0, 0);

  if ( ! fHitArray ) Fatal("Exec", "No HitArray");
  fHitArray->Delete();
  Int_t nPoints = fDigiArray->GetEntriesFast();
  // convert from TClonesarray to a std::vector
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++){
    PndSdsDigiPixel myDigi = *(PndSdsDigiPixel*)(fDigiArray->At(iPoint));
    std::cout << eventNr << " : Time " << FairRootManager::Instance()->GetEventTime() << " " << myDigi << std::endl;
    DigiPixelArray.push_back(myDigi);
  }
  eventNr++;
  // Retrieve the calculated clusters with the chosen clusterfinder
  std::vector< std::vector< Int_t> > clusters = fClusterFinder->GetClusters(DigiPixelArray);
  if(fVerbose>1) std::cout << " -I-  PndSdsPixelClusterTask::Exec(): We have "<<clusters.size()<<" pixel clusters" << std::endl;
  // store the list
  for (UInt_t i = 0; i < clusters.size(); i++)
  {
    new((*fClusterArray)[i]) PndSdsClusterPixel(fInBranchId, clusters[i]);
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
//    if(fVerbose>1){
      std::cout << " -I-  PndSdsPixelClusterTask::Exec(): Calculated Hit: " << std::endl;
      myHit.Print();
//    }
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

