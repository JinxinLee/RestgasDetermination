// -------------------------------------------------------------------------
// -----                PndDrcHitFinder source file             -----
// -------------------------------------------------------------------------


#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "PndDrcHitFinder.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"
#include "FairRunAna.h"
#include "FairEventHeader.h"

//#include "PndDrcDigi.h"

#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndDrcHitFinder::PndDrcHitFinder() :
FairTask("SDS Hybrid Hit Producer", 1)
{
  
  fPixelHits = 0;
  fEventNr = 0;
   
  if(fVerbose>0) Info("PndDrcHitFinder","DRC Hit Finder created, Parameters will be taken from RTDB");  
  fGeoH=NULL;
  fGeo = new PndGeoDrc();
  fDigiArray   = NULL;
  fPdHitArray   = NULL;
 
  fMCEventHeader = NULL;  

  fPixelSize = fGeo->PixelSize();	//pixel size;    
  fNpix = fGeo->Npixels(); //pixel columns
  fMcpActiveArea = fGeo->McpActiveArea();
  fPixelGap      =  (fMcpActiveArea - (Double_t)fNpix*fPixelSize) / ((Double_t)fNpix - 1.);
  fPixelStep     =  fMcpActiveArea/fNpix;//fPixelSize + 0.5*fPixelGap;

}
// -------------------------------------------------------------------------

PndDrcHitFinder::PndDrcHitFinder(Int_t iVerbose) :
  FairTask("DrcHitFinder", iVerbose)
{
  fPixelHits = 0;
  fEventNr = 0;    
  fGeoH = NULL;
  fGeo = new PndGeoDrc();
  fDigiPixelMCInfo = kFALSE;  
  if(fVerbose>0) Info("PndDrcHitFinder","DrcHitFinder created, Parameters will be taken from RTDB");

  fDigiArray   = NULL;
  fPdHitArray   = NULL;
  
  fMCEventHeader = NULL;  

  fPixelSize = fGeo->PixelSize();	//pixel size;  
  fNpix = fGeo->Npixels();              //pixel rows in one FE
  fMcpActiveArea = fGeo->McpActiveArea();
  fPixelGap      =  (fMcpActiveArea - (Double_t)fNpix*fPixelSize) / ((Double_t)fNpix - 1.);
  fPixelStep     =  fPixelSize + 0.5*fPixelGap;
}

PndDrcHitFinder::PndDrcHitFinder(const char* name, Int_t iVerbose) :
FairTask(name, iVerbose)
{
  fPixelHits = 0;
  fEventNr = 0;    
  fGeoH = NULL;
  fGeo = new PndGeoDrc();
  fDigiPixelMCInfo = kFALSE;  
  if(fVerbose>0) Info("PndDrcHitFinder","%s created, Parameters will be taken from RTDB",name);

  fDigiArray   = NULL;
  fPdHitArray   = NULL;
  
  fMCEventHeader = NULL;  

  fPixelSize = fGeo->PixelSize();	//pixel size;  
  fNpix = fGeo->Npixels();              //pixel rows in one FE
  fMcpActiveArea = fGeo->McpActiveArea();
  fPixelGap      =  (fMcpActiveArea - (Double_t)fNpix*fPixelSize) / ((Double_t)fNpix - 1.);
  fPixelStep     =  fPixelSize + 0.5*fPixelGap;
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndDrcHitFinder::~PndDrcHitFinder()
{
 if (fGeo) delete fGeo;
 
 if (fGeoH) delete fGeoH;	
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndDrcHitFinder::SetParContainers()
{
  if ( fGeoH == NULL )
    fGeoH = PndGeoHandling::Instance();

  fGeoH->SetParContainers(); 
  if(fVerbose>1) Info("SetParContainers","done.");
  return;
}

InitStatus PndDrcHitFinder::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndDrcHitFinder::Init()
{
    
  FairRun* ana = FairRun::Instance();
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman )
  {
    std::cout << "-E- PndDrcHitFinder::Init: "
    << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  
  // Get input array
  fDigiArray = (TClonesArray*) ioman->GetObject("DrcDigi");  
  if ( ! fDigiArray )
  {
    std::cout << "-W- PndDrcHitFinder::Init: "
    << "No DrcDigi array!" << std::endl;
    return kERROR;
  }
  
  // Create and register output array
  fPdHitArray	= new TClonesArray("PndDrcPDHit");    
  ioman->Register("DrcPDHit", "Drc", fPdHitArray, kTRUE);
  
  
 
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndDrcHitFinder::Exec(Option_t* opt)
{
  if(fVerbose>3) Info("Exec","Start");
  
  if ( ! fPdHitArray ) Fatal("Exec", "No PdHitArray");
  fPdHitArray->Delete(); 
  
  Double_t EventTime = FairRootManager::Instance()->GetEventTime();
  
  if(fVerbose>0) std::cout << "-I- PndDrcHitFinder::Exec EventTime: " << EventTime << std::endl;  
  
  //fPDHitList.clear();
  fGeoH->SetVerbose(fVerbose);  
  
  // Declare some variables
  PndDrcDigi *digi = NULL;
  Int_t detID = 0;
  Int_t sensorID = 0;
  Int_t pixelID = 0;
  TVector3 HitPosGlobal;
  TVector3 HitPosLocal;
  TVector3 dPosHit;
  Double_t hitTime = 0.;
    
  // Loop over PndSdsMCPoints
  Int_t nDigis = fDigiArray->GetEntriesFast();
  
  if(fVerbose>0) std::cout << "-I- PndDrcHitFinder::Exec event number: " << fEventNr <<" has "<<nDigis<<" digis."<< std::endl;
  
  if(fVerbose>3) Info("Exec","Begin loop for %i points",nDigis);
  for (Int_t iDigi = 0; iDigi < nDigis; iDigi++)
  {
    digi = (PndDrcDigi*) fDigiArray->At(iDigi); 
    detID = digi->GetSensorID();    
    pixelID = detID - 100*(Int_t)TMath::Floor((Double_t)detID/100.);
    sensorID = detID/100;
    hitTime = digi->GetTime();
        
    // the pixel number shows local coordinates of the hit:
    HitPosLocal.SetXYZ(fPixelStep*((Double_t)(pixelID % fNpix) - (Double_t)(fNpix/2) + 0.5),fPixelStep*(TMath::Floor(((Double_t)pixelID)/((Double_t)fNpix)) - (Double_t)(fNpix/2) + 0.5), 0.);
    
    // local coordinates of the hit on the MCP are translated into global ones as the following:
    HitPosGlobal = fGeoH->LocalToMasterShortId(HitPosLocal, sensorID);   
    dPosHit.SetXYZ(fPixelSize/2., fPixelSize/2., 0.);   
       	
    PndDrcPDHit*  aNewPDHit = AddDrcPDHit(detID, HitPosGlobal, dPosHit, hitTime, 0., iDigi);     
  }   // Loop over MCPoints
  
 
  fEventNr++;
  if(fVerbose>3) Info("Exec","Loop MC points");
}
//----------------------------------------------------------------------------

// -----   Add Photon Detector Hit to HitCollection   --------------------------------------
PndDrcPDHit* PndDrcHitFinder::AddDrcPDHit(Int_t iDetectorId, TVector3 posHit, TVector3 dPosHit, Double_t time, Double_t timeThreshold, Int_t index)
{
  TClonesArray& clrefPD = *fPdHitArray;
  Int_t size = clrefPD.GetEntriesFast();
  return new(clrefPD[size]) PndDrcPDHit(iDetectorId, posHit, dPosHit, time, timeThreshold, index);
}
//______________________________________________________________________________

// -------------------------------------------------------------------------

void PndDrcHitFinder::FinishEvent()
{
  // called after all Tasks did their Exex() and the data is copied to the file
  
  //	  fPdHitArray->Delete();
  FinishEvents();
}
// -------------------------------------------------------------------------

void PndDrcHitFinder::FinishTask()
{
  // called after all Tasks did their Exex() and the data is copied to the file
   
}

ClassImp(PndDrcHitFinder);
