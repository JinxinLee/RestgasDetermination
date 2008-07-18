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

#include "PndMvdDigiTask.h"
#include "PndMvdMCPoint.h"
// #include "PndMvdHit.h"
#include "PndMvdCalcPixel.h"
#include "PndMvdCalcFePixel.h"

// -----   Default constructor   -------------------------------------------
PndMvdDigiTask::PndMvdDigiTask() :
  CbmTask("MVD Digitization Task")
{
  fBranchName   = "MVDPoint";
  pixelHits = 0;
  stripHits = 0;

  fPixelProducer = new PndMvdHybridHitProducer();
  fStripProducer = new PndMvdStripHitProducer();
}
// -------------------------------------------------------------------------

PndMvdDigiTask::PndMvdDigiTask(   Double_t pixelX, Double_t pixelY,
                            Double_t topPitch, Double_t botPitch,
                            Double_t ori, Double_t skew,
                            TVector2 topAnchor, TVector2 botAnchor,
                            Int_t nrTopFE, Int_t nrBotFE, Int_t nrFECh,
                            Double_t pixelfThreshold, Double_t pixelfNoise,
                            Double_t stripfThreshold, Double_t stripfNoise  )

      :  CbmTask("MVD Digitization Task")
{
  fBranchName   = "MVDPoint";
//  fHitArray  = new TClonesArray("PndMvdHit");
//  fPixelArray  = new TClonesArray("PndMvdDigiPixel");
  
  pixelHits = 0;
  stripHits = 0;

  fPixelProducer = new PndMvdHybridHitProducer( pixelX, pixelY, pixelfThreshold, pixelfNoise );
  fStripProducer = new PndMvdStripHitProducer( topPitch, botPitch, ori, skew,
                                            topAnchor, botAnchor,
                                            nrTopFE, nrBotFE, nrFECh,
                                            stripfThreshold, stripfNoise );


}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMvdDigiTask::~PndMvdDigiTask() 
{ 
  if (fPixelProducer) delete (fPixelProducer);
  if (fStripProducer) delete (fStripProducer);
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdDigiTask::SetParContainers()
{
  // Get Base Container
  if (fPixelProducer) fPixelProducer->SetParContainers();
  if (fStripProducer) fStripProducer->SetParContainers();
  
/*  
  CbmRunAna* ana = CbmRunAna::Instance();
  CbmRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));
*/
}

InitStatus PndMvdDigiTask::ReInit()
{
  
  InitStatus stat=kERROR;
  if (fPixelProducer) if (fPixelProducer->ReInit() == kSUCCESS) stat = kSUCCESS;
  if (fStripProducer) if (fStripProducer->ReInit() == kSUCCESS) stat = kSUCCESS;
  else stat = kERROR;
  return stat;
  
  /*
  CbmRunAna* ana = CbmRunAna::Instance();
  CbmRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar=(PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));
  
  return kSUCCESS;
  */
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdDigiTask::Init() 
{
  
  CbmRootManager* ioman = CbmRootManager::Instance();

  if ( ! ioman ) 
    {
      std::cout << "-E- PndMvdDigiTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }
    
/*  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject(fBranchName);

  if ( ! fPointArray ) 
    {
      std::cout << "-W- PndMvdDigiTask::Init: "
     << "No MVDPoint array!" << std::endl;
      return kERROR;
  }*/
  
  
  InitStatus stat=kERROR;
  if (fPixelProducer) if (fPixelProducer->Init() == kSUCCESS) stat = kSUCCESS;
  if (fStripProducer) if (fStripProducer->Init() == kSUCCESS) stat = kSUCCESS;
  else stat = kERROR;
  return stat;
  
  std::cout << "-I- PndMvdDigiTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndMvdDigiTask::Exec(Option_t* opt) 
{


  if (fPixelProducer) fPixelProducer->Exec(opt);
  if (fStripProducer) fStripProducer->Exec(opt);
  return;

}


ClassImp(PndMvdDigiTask);
