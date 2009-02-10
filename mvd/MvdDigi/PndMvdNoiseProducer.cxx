// -------------------------------------------------------------------------
// -----                PndMvdNoiseProducer source file                -----
// -----                  Created 01.07.08  by R.Kliemt                -----
// -------------------------------------------------------------------------


#include "TClonesArray.h"
#include "TGeoNode.h"

#include "CbmRootManager.h"
#include "CbmGeoVolume.h"
#include "CbmRun.h"
#include "CbmRuntimeDb.h"
#include "CbmGeoNode.h"

#include "PndMvdNoiseProducer.h"
#include "PndMvdHitInfo.h"
#include "PndMvdMCPoint.h"
#include "PndMvdDigiStrip.h"
#include "PndMvdDigiPixel.h"

//TODO this include is for the enumeration
#include "PndMvdDetector.h"

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
      std::cout << "    Create a new one." << std::endl;
      fDigiStripArray = new TClonesArray("PndMvdDigiStrip");
  }
//   ioman->Register("MVDStripDigis", "MVD", fDigiStripArray, kTRUE); // reregister for output
    // TODO Check if this is done this way!

  fDigiPixelArray = (TClonesArray*) ioman->GetObject("MVDPixelDigis");
  if ( ! fDigiPixelArray )     {
      std::cout << "-W- PndMvdNoiseProducer::Init: No MVDPixelDigis array!" << std::endl;
      std::cout << "    Create a new one." << std::endl;
      fDigiPixelArray = new TClonesArray("PndMvdDigiPixel");
  }
//   ioman->Register("MVDPixelDigis", "MVD", fDigiPixelArray, kTRUE); // reregister for output

  fGeoH = new PndMvdGeoHandling(gGeoManager);
  // Retrieve a map between the active geometry nodes and their interpretation
  TGeoNode* topnode = gGeoManager->GetTopNode();
  for (Int_t n=0; n<topnode->GetNdaughters();n++)
  {
    gGeoManager->CdDown(n);
    TGeoNode* node = gGeoManager->GetCurrentNode();
    TString nodeName = node->GetName();
    if(nodeName.BeginsWith("MVD"))
    {
      DiveDownNode(node);
      break;
    }
    gGeoManager->CdUp();
  }
  if(fVerbose>1)
  {
    std::cout <<"-I- PndMvdNoiseProducer: Registered Sensors: "
              <<fStripRectIds.size()<<"xStripRect "
              <<fStripTrapIds.size()<<"xStripTrap "
              <<fPixelIds4.size()<<"xPixel"
               <<std::endl;
  }
  std::cout << "-I- PndMvdNoiseProducer: Intialisation successfull" << std::endl;


  return kSUCCESS;

}

void PndMvdNoiseProducer::DiveDownNode(TGeoNode *nodeMother){
  for (Int_t Nod=0; Nod<nodeMother->GetNdaughters();Nod++)
  {
    gGeoManager->CdDown(Nod);
    TGeoNode *aNode = gGeoManager->GetCurrentNode();
    if(aNode->GetNdaughters()>0) DiveDownNode(aNode);
    TString volname = gGeoManager->GetPath();
    if(volname.Contains("Active"))
    {
      if(volname.Contains("Rect")) fStripRectIds.push_back(fGeoH->GetID(volname));
      if(volname.Contains("Trap")) fStripTrapIds.push_back(fGeoH->GetID(volname));
      if(volname.Contains("Pixel"))
      {
        if(volname.Contains("4x1")) fPixelIds4.push_back(fGeoH->GetID(volname));
        if(volname.Contains("6x1")) fPixelIds6.push_back(fGeoH->GetID(volname));
        if(volname.Contains("4x2")) fPixelIds8.push_back(fGeoH->GetID(volname));
        if(volname.Contains("6x2")) fPixelIds12.push_back(fGeoH->GetID(volname));
      }
    }
    gGeoManager->CdUp();
  }
}

// -------------------------------------------------------------------------
void PndMvdNoiseProducer::SetParContainers()
{
  // Get Base Container
  CbmRun* ana = CbmRun::Instance();
  CbmRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));
  fDigiParRect = (PndMvdStripDigiPar*)(rtdb->getContainer("MVDStripDigiParRect"));
  fDigiParTrap = (PndMvdStripDigiPar*)(rtdb->getContainer("MVDStripDigiParTrap"));
  fDigiParPix  = (PndMvdPixelDigiPar*)(rtdb->getContainer("MVDPixelDigiPar"));
}


// -----   Public method Exec   --------------------------------------------
void PndMvdNoiseProducer::Exec(Option_t* opt)
{
//   TObjArray* activeSensors = fGeoPar->GetGeoSensitiveNodes();
  Int_t nrCh=0,rnd=0,
        nrFE=0,sens=0,
        nrSensors=0,fe=0,
        chanmax=0,chan=0,
        col=0,row=0,
        iStrip=0,
        chanwhite=0,iPix=0,
        charge=0,
        nNoisyStripRects=0,
        nNoisyStripTraps=0,
        nNoisyPixels=0;
  Double_t xfrac=0.;
  TString did;

  // *** Strip Rect ***
  // how many channels left?
  nrCh = fDigiParRect->GetNrFECh();
  nrFE = fDigiParRect->GetNrBotFE() + fDigiParRect->GetNrTopFE();
  nrSensors = fStripRectIds.size();
  chanmax = nrCh * nrFE * nrSensors;
  // Get Number of Channels fired from noise
  xfrac = CalcDistFraction(fDigiParRect->GetNoise(),fDigiParRect->GetThreshold());
  chanwhite = gRandom->Poisson(xfrac*chanmax);
  if(fVerbose>1) std::cout << "-I- PndMvdNoiseProducer: RECT xfrac = " << xfrac
            << " leading to " << chanwhite << " noisy digis of " << chanmax
            << " total channels" << std::endl;
  for(Int_t i = 0;i < chanwhite;i++)
  {
    // randomize the channel numbers & sensors
    rnd = gRandom->Integer(chanmax);
    sens = rnd/(nrFE*nrCh);
    rnd = rnd % (nrFE*nrCh);
    fe = rnd/nrCh; //will populate
    chan = rnd % nrCh;
    // calculate a charge deposit above threshold
    charge = CalcChargeAboveThreshold(fDigiParRect->GetNoise(),fDigiParRect->GetThreshold());
    did = fStripRectIds.at(sens);
    AddDigiStrip(nNoisyStripRects,-1,did,fe,chan,charge);
  }

  // *** Strip Trapezoids ***
  nrCh = fDigiParTrap->GetNrFECh();
  nrFE = fDigiParTrap->GetNrBotFE() + fDigiParTrap->GetNrTopFE();
  nrSensors = fStripTrapIds.size();
  chanmax = nrCh * nrFE * nrSensors;
  xfrac = CalcDistFraction(fDigiParTrap->GetNoise(),fDigiParTrap->GetThreshold());
  chanwhite = gRandom->Poisson(xfrac*chanmax);
  if(fVerbose>1) std::cout << "-I- PndMvdNoiseProducer: TRAP xfrac = " << xfrac
            << " leading to " << chanwhite << " noisy digis of " << chanmax
            << " total channels" << std::endl;
  for(Int_t i = 0;i < chanwhite;i++)
  {
    rnd = gRandom->Integer(chanmax);
    sens = rnd/(nrFE*nrCh);
    rnd = rnd % (nrFE*nrCh);
    fe = rnd/nrCh;
    chan = rnd % nrCh;
    charge = CalcChargeAboveThreshold(fDigiParTrap->GetNoise(),fDigiParTrap->GetThreshold());
    did = fStripTrapIds.at(sens);
    AddDigiStrip(nNoisyStripTraps,-1,did,fe,chan,charge);
  }

  // *** Pixel Sensors ***
  nrCh = fDigiParPix->GetFECols()*fDigiParPix->GetFERows();
  nrFE = 4*fPixelIds4.size() + 6*fPixelIds6.size() + 8*fPixelIds8.size() + 12*fPixelIds12.size();
  chanmax = nrCh * nrFE;
  xfrac = CalcDistFraction(fDigiParPix->GetNoise(),fDigiParPix->GetThreshold());
  chanwhite = gRandom->Poisson(xfrac*chanmax);
  if(fVerbose>1) std::cout << "-I- PndMvdNoiseProducer: PIXEL xfrac = " << xfrac
            << " leading to " << chanwhite << " noisy digis of " << chanmax
            << " total channels" << std::endl;
  for(Int_t i = 0;i < chanwhite;i++)
  {
    charge = CalcChargeAboveThreshold(fDigiParPix->GetNoise(),fDigiParPix->GetThreshold());
    rnd = gRandom->Integer(chanmax);
    chan = rnd%nrCh;
    col = chan%fDigiParPix->GetFECols();
    row = chan/fDigiParPix->GetFECols();
    fe  = rnd/nrCh;
    if(fe >= (4*fPixelIds4.size() + 6*fPixelIds6.size() + 8*fPixelIds8.size()) )
    {
      fe = fe - 4*fPixelIds4.size() - 6*fPixelIds6.size() - 8*fPixelIds8.size();
      sens = fe/12;
      did = fPixelIds12.at(sens);
      fe = fe%12;
      if(fe>6) fe=fe-6+10; //0-9 one row of FE, 10-19 2nd row of FE
    } else if( fe >= (4*fPixelIds4.size() + 6*fPixelIds6.size()) )
    {
      fe = fe - 4*fPixelIds4.size() - 6*fPixelIds6.size();
      sens = fe/8;
      did = fPixelIds8.at(sens);
      fe = fe%8;
      if(fe>4) fe=fe-4+10; //0-9 one row of FE, 10-19 2nd row of FE
    } else if( fe >= 4*fPixelIds4.size() )
    {
      fe = fe -4*fPixelIds4.size();
      sens = fe/6;
      did = fPixelIds6.at(sens);
      fe = fe%6;
    } else
    {
      sens = fe/4;
      did = fPixelIds4.at(sens);
      fe = fe%4;
    }

    AddDigiPixel(nNoisyPixels,-1,did,fe,col,row,charge);
  }

  // *** The End ***
  if(fVerbose>0)
  {
    std::cout <<"-I- PndMvdNoiseProducer: Noise produced\t"
              <<nNoisyStripRects <<"xStripRect\t"
              <<nNoisyStripTraps <<"xStripTrap\t"
              <<nNoisyPixels <<"xPixels"<<std::endl;
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t PndMvdNoiseProducer::CalcDistFraction(Double_t spread, Double_t threshold)
{
  // mean fraction of fireing digis
  return 0.5*TMath::Erfc( threshold / (TMath::Sqrt2()*spread) );
}

// Int_t PndMvdNoiseProducer::CalcChanWhite(Int_t chanleft, Double_t frac)
// {
//   Int_t temp=0;
//   // random number of firing digis around the mean fraction
//   temp = gRandom->Poisson(frac*chanleft);
//   return (Int_t)temp;
// }

Int_t PndMvdNoiseProducer::CalcChargeAboveThreshold(Double_t spread,Double_t threshold)
{
  // only the charge ABOVE the threshold counts
  // get maximum y value of gauss tail
  // calculate a random charge according to that tails distribution
  Double_t temp=0.;
  temp = TMath::Gaus(threshold,0,spread);
  temp = gRandom->Uniform(0.,temp);
  temp = -2.*spread*spread*log(temp);
  temp = sqrt(temp);
  return (Int_t)temp;
}
// -------------------------------------------------------------------------
void PndMvdNoiseProducer::AddDigiStrip(Int_t &noisies, Int_t iPoint, TString detname, Int_t fe, Int_t chan, Double_t charge)
{
  Bool_t found = kFALSE;
  Int_t detID = kMVDHitsStrip;
  Int_t iStrip = fDigiStripArray->GetEntriesFast();
  PndMvdDigiStrip* aDigi = 0;
  for(Int_t kstr = 0; kstr < iStrip && found == kFALSE; kstr++)
  {
	aDigi = (PndMvdDigiStrip*)fDigiStripArray->At(kstr);
	if ( aDigi->GetDetID() == detID,
		 aDigi->GetDetName() == detname,
		 aDigi->GetFE() == fe,
		 aDigi->GetChannel() == chan )
	{
		aDigi->AddCharge(charge);
		aDigi->AddIndex(iPoint);
		found = kTRUE;
	}
  }
  if(found == kFALSE){
	  //TODO: get a reasonable timestamp fake for the noise
    new ((*fDigiStripArray)[iStrip]) PndMvdDigiStrip(iPoint,detID,detname,fe,chan,0,charge) ;
    noisies++;
    if(fVerbose>2) std::cout
      << " -I- PndMvdNoiseProducer: Added StripTrap Digi at: FE=" << fe
      << ", channel=" << chan << ", charge=" << charge<< " e"
      << ", in sensor \n" << detname <<std::endl;
  }
}
// -------------------------------------------------------------------------
void PndMvdNoiseProducer::AddDigiPixel(Int_t &noisies, Int_t iPoint, TString detname, Int_t fe, Int_t col, Int_t row, Double_t charge)
{
  Bool_t found = kFALSE;
  Int_t detID = kMVDHitsPixel;
  Int_t iPix = fDigiPixelArray->GetEntriesFast();
  PndMvdDigiPixel* aDigi = 0;
  for(Int_t kstr = 0; kstr < iPix && found == kFALSE; kstr++)
  {
	aDigi = (PndMvdDigiPixel*)fDigiPixelArray->At(kstr);
	if ( aDigi->GetDetID() == detID,
		 aDigi->GetDetName() == detname,
		 aDigi->GetFE() == fe,
		 aDigi->GetPixelColumn() == col,
		 aDigi->GetPixelRow() == row )
	{
		aDigi->AddCharge(charge);
		aDigi->AddIndex(iPoint);
		found = kTRUE;
	}
  }
  if(found == kFALSE){
    new ((*fDigiPixelArray)[iPix]) PndMvdDigiPixel(iPoint,detID,detname,fe,col,row,charge) ;
    noisies++;
    if(fVerbose>2) std::cout
      << " -I- PndMvdNoiseProducer: Added Pixel Digi at: FE=" << fe
      << ", col|row = ("<<col<<"|"<<row<< "), charge=" << charge<< " e"
      << ", in sensor \n" << detname.Data() <<std::endl;

  }
}
// -------------------------------------------------------------------------


ClassImp(PndMvdNoiseProducer)
