// -------------------------------------------------------------------------
// -----                PndSdsIdealClusterTask source file             -----
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

#include "PndSdsIdealClusterTask.h"
#include "PndSdsMCPoint.h"
// #include "PndSdsHit.h"
#include "PndSdsCalcPixel.h"
#include "PndSdsCalcFePixel.h"
#include "PndSdsDigiPixel.h"
// #include "PndSdsPixelCluster.h"
#include "PndSdsClusterPixel.h"

#include "PndSdsSimplePixelClusterFinder.h"
#include "PndSdsChargeWeightedPixelMapping.h"

// -----   Default constructor   -------------------------------------------
PndSdsIdealClusterTask::PndSdsIdealClusterTask() :
  FairTask("SDS Clustertisation Task")
{

  TGeoManager* geoMan;// = new TGeoManager("geoMan","geoMan");
  //geoMan->Import("PndSdsG4_DPM15fE+i_10000.root","FAIRGeom");
  geoMan = gGeoManager;
//  fDigiArray  = new TClonesArray("PndSdsDigiPixel");
//  fClusterArray  = new TClonesArray("PndSdsIdealClusterPixel");
}

PndSdsIdealClusterTask::PndSdsIdealClusterTask(Double_t radius, Int_t FEcolumns, Int_t FErows, TString geoFile) :
  FairTask("SDS Clustertisation Task")
{
//  fDigiArray  = new TClonesArray("PndSdsDigiPixel");
//  fClusterArray  = new TClonesArray("PndSdsClusterPixelPixel");
  TGeoManager* geoMan;// = new TGeoManager("geoMan","geoMan");
  //geoMan->Import("PndSdsG4_DPM15fE+i_10000.root","FAIRGeom");
  geoMan = gGeoManager;

  fRadius = radius;
  fFEcolumns = FEcolumns;
  fFErows    = FErows;
  fGeoFile   = geoFile;
  fParams.push_back(radius);
  fParams.push_back(FEcolumns);
  fParams.push_back(FErows);
}

// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndSdsIdealClusterTask::~PndSdsIdealClusterTask()
{
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndSdsIdealClusterTask::SetParContainers()
{
  // Get Base Container
/*
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar = (PndSdsGeoPar*)(rtdb->getContainer("PndSdsGeoPar"));
*/
}

InitStatus PndSdsIdealClusterTask::ReInit()
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
InitStatus PndSdsIdealClusterTask::Init()
{

  SetBranchNames();

  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndSdsIdealClusterTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fDigiArray = (TClonesArray*) ioman->GetObject(fBranchName);

  if ( ! fDigiArray )
    {
      std::cout << "-W- PndSdsIdealClusterTask::Init: "
     << "No SDSDigi array!" << std::endl;
      return kERROR;
  }


  fHitArray = new TClonesArray("PndSdsHit");
  ioman->Register(fHitBranchName , fFolderName, fHitArray, kTRUE);

  fClusterArray = new TClonesArray("PndSdsClusterPixel");
  ioman->Register(fClustBranchName, fFolderName, fClusterArray, kTRUE);

  std::cout << "-I- PndSdsIdealClusterTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndSdsIdealClusterTask::Exec(Option_t* opt)
{
  std::vector<PndSdsDigiPixel> DigiPixelArray;
  // Reset output array
  if ( ! fClusterArray )
    Fatal("Exec", "No ClusterArray");
  fClusterArray->Delete();

  if ( ! fHitArray )
    Fatal("Exec", "No HitArray");
  fHitArray->Delete();

  Int_t
    nPoints = fDigiArray->GetEntriesFast();

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++){
	PndSdsDigiPixel myDigi = *(PndSdsDigiPixel*)(fDigiArray->At(iPoint));
	DigiPixelArray.push_back(myDigi);
  }
  PndSdsIdealPixelClusterFinder finder(fParams, DigiPixelArray);
  finder.SetVerbose(fVerbose);
  std::vector< std::vector< Int_t> > clusters = finder.GetClusters();
  std::cout << clusters.size() << std::endl;
  for (Int_t i = 0; i < clusters.size(); i++)
  {
	new((*fClusterArray)[i]) PndSdsClusterPixel(clusters[i]);
  }

  std::vector<Double_t> mappingPar;
  mappingPar.push_back(fParams[1]);
  mappingPar.push_back(fParams[2]);
  mappingPar.push_back(0.01);
  mappingPar.push_back(0.01);
 for (Int_t i = 0; i < clusters.size(); i++){
	std::cout << clusters[i].size() << " " << std::endl;
	std::vector<PndSdsDigiPixel> clusterArray;
	for (Int_t j=0;j < clusters[i].size();j++)
		clusterArray.push_back(DigiPixelArray[clusters[i][j]]);
	PndSdsChargeWeightedPixelMapping mapping(clusterArray, mappingPar);
	mapping.SetVerbose(fVerbose);
	PndSdsHit myCluster = mapping.GetCluster();
	std::cout << "ClusterData: " << std::endl;
	myCluster.Print();
	new ((*fHitArray)[i]) PndSdsHit(myCluster);
  }
  std::cout << std::endl;

  return;

}

ClassImp(PndSdsIdealClusterTask);

