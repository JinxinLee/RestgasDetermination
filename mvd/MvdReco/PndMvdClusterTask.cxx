// -------------------------------------------------------------------------
// -----                CbmStsHitProducerIdeal source file             -----
// -----                  Created 10/01/06  by V. Friese               -----
// -------------------------------------------------------------------------


#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"

#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmGeoNode.h"
#include "CbmGeoVector.h"

#include "PndStringVector.h"

#include "PndMvdClusterTask.h"
#include "PndMvdPixelClusterTask.h"
#include "PndMvdStripClusterTask.h"
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
PndMvdClusterTask::PndMvdClusterTask() :
  CbmTask("MVD Clustertisation Task")
{
//   fBranchName   = "MVDPixelDigis";
  
//  fDigiArray  = new TClonesArray("PndMvdDigiPixel");
//  fClusterArray  = new TClonesArray("PndMvdClusterPixel");
}
// -------------------------------------------------------------------------

PndMvdClusterTask::PndMvdClusterTask(Double_t radius, Int_t FEcolumns, Int_t FErows, TString geoFile) :
  CbmTask("MVD Clustertization Task") 
{
//   fBranchName   = "MVDPixelDigis";
//  fDigiArray  = new TClonesArray("PndMvdDigiPixel");
//  fClusterArray  = new TClonesArray("PndMvdClusterPixel");

  this->Add(new PndMvdPixelClusterTask(radius,geoFile));
  this->Add(new PndMvdStripClusterTask(1e6,geoFile)); //TODO: fix cargecut hardcoded
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMvdClusterTask::~PndMvdClusterTask() 
{ 
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdClusterTask::SetParContainers()
{
  // Get Base Container
/*  
  CbmRunAna* ana = CbmRunAna::Instance();
  CbmRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));
*/
}

InitStatus PndMvdClusterTask::ReInit()
{
  
  InitStatus stat=kERROR;
  return stat;
  
  /*
  CbmRunAna* ana = CbmRunAna::Instance();
  CbmRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar=(PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));
  
  return kSUCCESS;
  */
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdClusterTask::Init() 
{
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndMvdClusterTask::Exec(Option_t* opt) 
{
  return;
}

ClassImp(PndMvdClusterTask);

