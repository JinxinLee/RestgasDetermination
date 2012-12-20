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
#include "FairEventHeader.h"

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
#include "PndSdsFESimple.h"

// -----   Default constructor   -------------------------------------------
PndSdsPixelClusterTask::PndSdsPixelClusterTask() :
PndSdsTask("SDS Clustertisation Task"), fPersistance(kTRUE), fClusterType(-1), fEventNr(0)
{
  fFEModel = NULL;

  fDigiArray = NULL;
  fDigiPar = NULL;
  fChargeConverter = NULL;
  fGeoH = NULL;
  
  fClustBranchName = "";
  /** Output array of PndSdsHits **/
  fClusterArray = NULL;
  fHitArray = NULL;
  
  fFunctor = NULL;
  fStartFunctor = NULL;
  
  fClusterFinder = NULL;
  fBackMapping = NULL;
}
// -------------------------------------------------------------------------

// -----   Named constructor   ---------------------------------------------
PndSdsPixelClusterTask::PndSdsPixelClusterTask(const char* name) :
PndSdsTask(name), fPersistance(kTRUE), fClusterType(-1), fEventNr(0)
{
  fFEModel = NULL;

  fDigiArray = NULL;
  fDigiPar = NULL;
  fChargeConverter = NULL;
  fGeoH = NULL;
  
  fClustBranchName = "";
  /** Output array of PndSdsHits **/
  fClusterArray = NULL;
  fHitArray = NULL;
  
  fFunctor = NULL;
  fStartFunctor = NULL;
  
  fClusterFinder = NULL;
  fBackMapping = NULL;
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
  if ( fGeoH == NULL ) {
    fGeoH = PndGeoHandling::Instance();
  }
  fGeoH->SetParContainers();
  if( ! fDigiPar) Fatal("SetParContainers","No digitization parameters specified");
  if(fVerbose>1) Info("SetParContainers","done.");
  return;
}


// -----   Initialization  of Parameter Containers -------------------------

InitStatus PndSdsPixelClusterTask::ReInit()
{
  
  /*
   FairRun* ana = FairRun::Instance();
   FairRuntimeDb* rtdb=ana->GetRuntimeDb();
   fGeoPar=(PndSdsGeoPar*)(rtdb->getContainer("PndSdsGeoPar"));
   
   return kSUCCESS;
   */
  
  return kSUCCESS; // TODO: Fill in sense here...
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
  fDigiArray = (TClonesArray*) ioman->GetObject(fInBranchName);
//
  if ( ! fDigiArray )
  {
    std::cout << "-E- PndSdsPixelClusterTask::Init: "
    << "No SDSDigi array!" << std::endl;
    return kERROR;
  }
  
  fClusterArray = ioman->Register(fClustBranchName, "PndSdsClusterPixel", fFolderName, fPersistance);

  fHitArray =  ioman->Register(fOutBranchName, "PndSdsHit", fFolderName, fPersistance);
  
  SetInBranchId();

  fFunctor = new TimeGap();
  fStartFunctor = new StopTime();
//  fFEModel = new PndSdsFESimple();


  if(fVerbose>1) fDigiPar->Print();
  
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

  fGeoH->SetVerbose(fVerbose);

  Double_t EventTime = FairRootManager::Instance()->GetEventTime();

   if(fVerbose>0) std::cout << "-I- PndSdsPixelClusterTask::Exec EventTime: " << EventTime << std::endl;

    if (FairRunAna::Instance()->IsTimeStamp()){
    	std::cout << "TimeStepPixel: " << fDigiPar->GetTimeStep();
    	fDigiArray = FairRootManager::Instance()->GetData(fInBranchName, fFunctor, fDigiPar->GetTimeStep() * 2);
    }

  if ( ! fHitArray ) Fatal("Exec", "No HitArray");

  Int_t nPoints = fDigiArray->GetEntriesFast();
  //std::cout << "Points in DigiArray: " << nPoints << std::endl;
  // convert from TClonesArray to a std::vector
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++){
    PndSdsDigiPixel myDigi = *(PndSdsDigiPixel*)(fDigiArray->At(iPoint));
    DigiPixelArray.push_back(myDigi);
  }
  // Retrieve the calculated clusters with the chosen clusterfinder
  std::vector< std::vector< Int_t> > clusters = fClusterFinder->GetClusters(DigiPixelArray);
  if(fVerbose>1) std::cout << " -I-  PndSdsPixelClusterTask::Exec(): We have "<<clusters.size()<<" pixel clusters" << std::endl;
  // store the list
  for (UInt_t i = 0; i < clusters.size(); i++)
  {
    PndSdsClusterPixel* tempCluster = new((*fClusterArray)[i]) PndSdsClusterPixel(fInBranchId, clusters[i]);

    if (FairRunAna::Instance()->IsTimeStamp()){
//		std::cout << "TempCluster: " << *tempCluster << std::endl;
		tempCluster->ResetLinks();
		for (UInt_t j = 0; j < clusters[i].size(); j++){
			PndSdsDigiPixel* tempDigi = (PndSdsDigiPixel*)fDigiArray->At(clusters[i][j]);
//			std::cout << "TempDigi: " << *tempDigi << std::endl;
//			std::cout << "EntryNr: ";
//			tempDigi->GetEntryNr().Print();
//			std::cout << std::endl;
			tempCluster->AddLink(FairLink(tempDigi->GetEntryNr()));
//			std::cout << "Links: " << (FairMultiLinkedData)(*tempCluster) << std::endl;
		}
    }
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
    myHit.SetClusterIndex(fClusterType,i, -1, FairRootManager::Instance()->GetEntryNr());

 //   myHit.SetCharge(myHit.GetCharge());
    if(fVerbose>0){
      std::cout << " -I-  PndSdsPixelClusterTask::Exec(): Calculated Hit: " << std::endl;
      myHit.Print();
      ((FairMultiLinkedData)(myHit)).Print();
    }
    new ((*fHitArray)[i]) PndSdsHit(myHit);
  }
  if(fVerbose>1)std::cout << std::endl;

  if(fVerbose>1){
    std::cout << "-I- PndSdsPixelClusterTask: " << fClusterArray->GetEntriesFast()
    << " Sds Clusters and " << fHitArray->GetEntriesFast()<<" Hits calculated." << std::endl;
  }
  fEventNr++;
  fHitArray->Sort();
  return;
  
}

void PndSdsPixelClusterTask::FinishEvent(){

	if (fDigiArray > 0) 	fDigiArray->Delete();
	if (fHitArray > 0) 		fHitArray->Delete();
	if (fClusterArray > 0) 	fClusterArray->Delete();
}

ClassImp(PndSdsPixelClusterTask);

