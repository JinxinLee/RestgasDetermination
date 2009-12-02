// -------------------------------------------------------------------------
// -----                PndMvdIdealClusterTask source file             -----
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

#include "PndMvdIdealClusterTask.h"
#include "PndMvdMCPoint.h"
// #include "PndMvdHit.h"
#include "PndMvdCalcPixel.h"
#include "PndMvdCalcFePixel.h"
#include "PndMvdDigiPixel.h"
// #include "PndMvdPixelCluster.h"
#include "PndMvdCluster.h"

#include "PndMvdSimplePixelClusterFinder.h"
#include "PndMvdChargeWeightedPixelMapping.h"

// -----   Default constructor   -------------------------------------------
PndMvdIdealClusterTask::PndMvdIdealClusterTask() :
  FairTask("MVD Clustertisation Task")
{
  fBranchName   = "MVDPixelDigis";

  TGeoManager* geoMan;// = new TGeoManager("geoMan","geoMan");
  //geoMan->Import("PndMvdG4_DPM15fE+i_10000.root","FAIRGeom");
  geoMan = gGeoManager;
//  fDigiArray  = new TClonesArray("PndMvdDigiPixel");
//  fClusterArray  = new TClonesArray("PndMvdIdealClusterPixel");
}

PndMvdIdealClusterTask::PndMvdIdealClusterTask(Double_t radius, Int_t FEcolumns, Int_t FErows, TString geoFile) :
  FairTask("MVD Clustertisation Task")
{
  fBranchName   = "MVDPixelDigis";
//  fDigiArray  = new TClonesArray("PndMvdDigiPixel");
//  fClusterArray  = new TClonesArray("PndMvdClusterPixel");
  TGeoManager* geoMan;// = new TGeoManager("geoMan","geoMan");
  //geoMan->Import("PndMvdG4_DPM15fE+i_10000.root","FAIRGeom");
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
PndMvdIdealClusterTask::~PndMvdIdealClusterTask()
{
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdIdealClusterTask::SetParContainers()
{
  // Get Base Container
/*
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));
*/
}

InitStatus PndMvdIdealClusterTask::ReInit()
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
InitStatus PndMvdIdealClusterTask::Init()
{

  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndMvdIdealClusterTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fDigiArray = (TClonesArray*) ioman->GetObject(fBranchName);

  if ( ! fDigiArray )
    {
      std::cout << "-W- PndMvdIdealClusterTask::Init: "
     << "No MVDDigi array!" << std::endl;
      return kERROR;
  }


  fHitArray = new TClonesArray("PndMvdHit");
  ioman->Register("PndMvdIdealClusterHit", "MVD", fHitArray, kTRUE);

  fClusterArray = new TClonesArray("PndMvdCluster");
  ioman->Register("PndMvdCluster","MVD",fClusterArray,kTRUE);

  std::cout << "-I- PndMvdIdealClusterTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndMvdIdealClusterTask::Exec(Option_t* opt)
{
  std::vector<PndMvdDigiPixel> DigiPixelArray;
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
	PndMvdDigiPixel myDigi = *(PndMvdDigiPixel*)(fDigiArray->At(iPoint));
	DigiPixelArray.push_back(myDigi);
  }
  PndMvdIdealPixelClusterFinder finder(fParams, DigiPixelArray);
  finder.SetVerbose(fVerbose);
  std::vector< std::vector< Int_t> > clusters = finder.GetClusters();
  std::cout << clusters.size() << std::endl;
  for (Int_t i = 0; i < clusters.size(); i++)
  {
	new((*fClusterArray)[i]) PndMvdCluster(clusters[i]);
  }

  std::vector<Double_t> mappingPar;
  mappingPar.push_back(fParams[1]);
  mappingPar.push_back(fParams[2]);
  mappingPar.push_back(0.01);
  mappingPar.push_back(0.01);
 for (Int_t i = 0; i < clusters.size(); i++){
	std::cout << clusters[i].size() << " " << std::endl;
	std::vector<PndMvdDigiPixel> clusterArray;
	for (Int_t j=0;j < clusters[i].size();j++)
		clusterArray.push_back(DigiPixelArray[clusters[i][j]]);
	PndMvdChargeWeightedPixelMapping mapping(clusterArray, mappingPar);
	mapping.SetVerbose(fVerbose);
	PndMvdHit myCluster = mapping.GetCluster();
	std::cout << "ClusterData: " << std::endl;
	myCluster.Print();
	new ((*fHitArray)[i]) PndMvdHit(myCluster);
  }
  std::cout << std::endl;

  return;

}

ClassImp(PndMvdIdealClusterTask);

