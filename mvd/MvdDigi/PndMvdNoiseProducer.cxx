// -------------------------------------------------------------------------
// -----                CbmStsHitProducerIdeal source file             -----
// -----                  Created 10/01/06  by V. Friese               -----
// -------------------------------------------------------------------------


#include "TClonesArray.h"

#include "CbmRootManager.h"
#include "CbmGeoVolume.h"
#include "PndMvdNoiseProducer.h"
#include "PndMvdHitInfo.h"
#include "PndMvdMCPoint.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"

// -----   Default constructor   -------------------------------------------
PndMvdNoiseProducer::PndMvdNoiseProducer() :
  CbmTask("Charge Noise Producer") 
{
	fBranchName 	= "MVDStripDigis";
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndMvdNoiseProducer::~PndMvdNoiseProducer() 
{ 
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdNoiseProducer::Init() 
{
  // Get RootManager
  CbmRootManager* ioman = CbmRootManager::Instance();
  
  if ( ! ioman ) 
    {
      std::cout << "-E- PndMvdNoiseProducer::Init: RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fDigiStripArray = (TClonesArray*) ioman->GetObject("MVDStripDigis");
  if ( ! fDigiStripArray )  {
      std::cout << "-W- PndMvdNoiseProducer::Init: No MVDStripDigis array!" << std::endl;
      return kERROR;
  }
  ioman->Register("MVDStripDigis", "MVD", fDigiStripArray, kTRUE); // reregister for output
    // TODO Check if this is done this way!

  fDigiPixelArray = (TClonesArray*) ioman->GetObject("MVDPixelDigis");
  if ( ! fDigiPixelArray )     {
      std::cout << "-W- PndMvdNoiseProducer::Init: No MVDPixelDigis array!" << std::endl;
      return kERROR;
  }
  ioman->Register("MVDPixelDigis", "MVD", fDigiPixelArray, kTRUE); // reregister for output

  std::cout << "-I- PndMvdNoiseProducer: Intialisation successfull" << std::endl;

  // Retrieve a map between the active geometry nodes and their interpretation
  TObjArray* activeSensors = fGeoPar->GetGeoSensitiveNodes();
  for(Int_t k = 0;k < activeSensors->GetEntriesFast(); k++)
  {
    CbmGeoVolume* vol = (CbmGeoVolume*)activeSensors->At(k);
    TString volname = vol->getName();
    if(volname.Contains("StripRect")) fStripRectIds.push_back(k);
    if(volname.Contains("StripTrap")) fStripTrapIds.push_back(k);
    if(volname.Contains("Pixel"))     fPixelIds.push_back(k);
  }

  return kSUCCESS;

}
// -------------------------------------------------------------------------
void PndMvdNoiseProducer::SetParContainers()
{
  // Get Base Container
  CbmRunAna* ana = CbmRunAna::Instance();
  CbmRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));
  fDigiParRect = (PndMvdStripDigiPar*)(rtdb->getContainer("MVDStripDigiParRect"));
  fDigiParTrap = (PndMvdStripDigiPar*)(rtdb->getContainer("MVDStripDigiParTrap"));
  fDigiParPix  = (PndMvdPixelDigiPar*)(rtdb->getContainer("MVDPixelDigiPar"));
}


// -----   Public method Exec   --------------------------------------------
void PndMvdNoiseProducer::Exec(Option_t* opt) 
{
  TObjArray* activeSensors = fGeoPar->GetGeoSensitiveNodes();
  // Do NOT Reset output array!
  // get through all 3 sensor types:

  // Strip Rect 

  // how many channels left?
  Int_t nrCh = fDigiParRect->GetNrFECh();
  Int_t nrFE = fDigiParRect->GetNrBotFE() + fDigiParRect->GetNrTopFE();
  Int_t nrSensors = fStripRectIds.size();
  Int_t chanmax = nrCh * nrFE * nrSensors;
  Int_t chanleft = chanmax - fDigiStripArray->GetEntriesFast();
  // Get Number of Channels fired from noise
  Int_t chanwhite = CalcChanWhite(chanleft,
                                  1000.,
//                                   fDigiParRect->GetAbsNoiseLevel(),
                                  fDigiParRect->GetNoise(),
                                  fDigiParRect->GetThreshold());
  for(Int_t i = 0;i < chanwhite;i++)
  {
    // randomize the channel numbers & sensors
    Int_t rnd = gRandom->Integer(chanmax);
    Int_t sens = rnd/nrSensors;
    rnd = rnd % nrSensors;
    Int_t fe = rnd/nrFE; //will populate 
    Int_t chan = rnd % nrFE;
      // calculate a charge deposit above threshold
    
      // add this digi
//       TString detname = activeSensors->At(fStripRectIds[sens]);
//       addDigi... ( chan,fe,... fGeoH->GetID(detname) ...);
  }

  // Strips

}
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
Int_t PndMvdNoiseProducer::CalcChanWhite(Int_t chanleft, Double_t spread, 
                                         Double_t level, Double_t threshold)
{
  Double_t result=0., x=0.;
  // mean fraction of fireing digis
  x = TMath::Erfc( (threshold-level) / spread );
  // random number of firing digis around the mean fraction
  result = gRandom->Gaus(x,1./sqrt(chanleft));
  result *= chanleft;
  // TODO  we collect different ammounts of noise since we read each clock 
  // cycle, but the here we're in an eventloop:
//   Double_t evtTime = ... ???
//   Double_t clkTime = ... ???
//   result *= evtTime/clkTime;
  return (Int_t)result;
}
// -------------------------------------------------------------------------

ClassImp(PndMvdNoiseProducer)
