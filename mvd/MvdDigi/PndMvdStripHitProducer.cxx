// -------------------------------------------------------------------------
// -----                CbmStsHitProducerIdeal source file             -----
// -----                  Created 10/01/06  by V. Friese               -----
// -------------------------------------------------------------------------


#include "TClonesArray.h"
#include "TArrayD.h"
#include "TVector2.h"
#include "TGeoManager.h"

#include "CbmRootManager.h"
#include "PndMvdStripHitProducer.h"
#include "PndMvdMCPoint.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmGeoNode.h"
#include "CbmRuntimeDb.h"
#include "CbmGeoNode.h"
#include "CbmGeoVector.h"
#include "PndStringVector.h"
#include "PndMvdCalcStrip.h"
#include "PndMvdDigiStrip.h"

// -----   Default constructor   -------------------------------------------
PndMvdStripHitProducer::PndMvdStripHitProducer() :
  CbmTask("MVD Strip Digi Producer(PndMvdStripHitProducer)")
{
  fBranchName   = "MVDPoint";
//  stripHits = 0;

/*  fTopPitch = 0.;
  fBotPitch = 0.;
  fOrient = 0.;
  fSkew = 0.;
  fTopAnchor = TVector2(0,0);
  fBotAnchor = TVector2(0,0);
  fNrTopFE = 0;
  fNrBotFE = 0;
  fNrFECh = 0;
  fThreshold = 0.;
  fNoise = 0.;*/
  fOverrideParams = false;
//  fHitArray  = new TClonesArray("PndMvdHit");
//	fStripArray	= new TClonesArray("PndMvdStripHit");
}
// -------------------------------------------------------------------------

PndMvdStripHitProducer::PndMvdStripHitProducer(Double_t topPitch, Double_t botPitch,
                                         Double_t ori, Double_t skew,
                                         TVector2 topAnchor, TVector2 botAnchor,
                                         Int_t nrTopFE, Int_t nrBotFE, Int_t nrFECh,
                                         Double_t threshold, Double_t noise,
                                         TString sensorType, TString feType) :
  CbmTask("MVD Strip Digi Producer")
{
  // This constructor is probably not needed anymore, since the parameters are 
  // read in via an ascii file.
  std::cout <<" -W- Obsolete constructor for PndMvdStripHitProducer called."
            <<"Mvd strip sensors in barrel and disk are set to the SAME."<< std::endl;

  fBranchName   = "MVDPoint";
  fOverrideParams = true;
  SetParamSet(topPitch,botPitch,ori,skew,topAnchor,botAnchor,nrTopFE,nrBotFE,
              nrFECh,threshold,noise,"Rect",feType);
  SetParamSet(topPitch,botPitch,ori,skew,topAnchor,botAnchor,nrTopFE,nrBotFE,
              nrFECh,threshold,noise,"Trap",feType);
  std::cout << "MVD Strip Digi Producer initiated" << std::endl;
}

void PndMvdStripHitProducer::SetParamSet(Double_t topPitch, Double_t botPitch,
                                         Double_t ori, Double_t skew,
                                         TVector2 topAnchor, TVector2 botAnchor,
                                         Int_t nrTopFE, Int_t nrBotFE, Int_t nrFECh,
                                         Double_t threshold, Double_t noise,
                                         TString sensorType, TString feType)
{
  CbmRunAna* ana = CbmRunAna::Instance();
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( 0==fDigiParRect || 0==fDigiParTrap ) SetParContainers();
  if (fOverrideParams){
    if (sensorType.Contains("Rect")) fCurrentDigiPar = fDigiParRect;
    else if (sensorType.Contains("Trap")) fCurrentDigiPar = fDigiParTrap;
    fCurrentDigiPar->SetTopPitch(topPitch);
    fCurrentDigiPar->SetBotPitch(botPitch);
    fCurrentDigiPar->SetSkew(skew);
    fCurrentDigiPar->SetOrient(ori);
    fCurrentDigiPar->SetTopAnchor(topAnchor);
    fCurrentDigiPar->SetBotAnchor(botAnchor);
    fCurrentDigiPar->SetNrFECh(nrFECh);
    fCurrentDigiPar->SetNrTopFE(nrTopFE);
    fCurrentDigiPar->SetNrBotFE(nrBotFE);
    fCurrentDigiPar->SetThreshold(threshold);
    fCurrentDigiPar->SetNoise(noise);
    fCurrentDigiPar->SetSensType(sensorType);
    fCurrentDigiPar->SetFeType(feType);
    fCurrentDigiPar->setChanged();
    fCurrentDigiPar->setInputVersion(ana->GetRunId(),1);
  }


}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMvdStripHitProducer::~PndMvdStripHitProducer()
{
  delete fGeoH;
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdStripHitProducer::SetParContainers()
{
  // called from the CbmRunAna::Init()
  // Get Base Container
  CbmRunAna* ana = CbmRunAna::Instance();
  CbmRuntimeDb* rtdb=ana->GetRuntimeDb();
  fDigiParRect = (PndMvdStripDigiPar*)(rtdb->getContainer("MVDStripDigiParRect"));
  fDigiParTrap = (PndMvdStripDigiPar*)(rtdb->getContainer("MVDStripDigiParTrap"));
}

InitStatus PndMvdStripHitProducer::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}


// -----   Public method Init   --------------------------------------------
InitStatus PndMvdStripHitProducer::Init()
{
  CbmRunAna* ana = CbmRunAna::Instance();
  CbmRootManager* ioman = CbmRootManager::Instance();

  fGeoH = new PndMvdGeoHandling(gGeoManager);

  if ( ! ioman ) 
    {
      std::cout << "-E- PndMvdStripHitProducer::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  fPointArray = (TClonesArray*) ioman->GetObject(fBranchName);
  if ( ! fPointArray )
    {
      std::cout << "-W- PndMvdStripHitProducer::Init: "
     << "No MVDPoint array!" << std::endl;
      return kERROR;
  }

  // Create and register output array
//  fHitArray = new TClonesArray("PndMvdHit");
//  ioman->Register("MVDHit", "MVD", fHitArray, kTRUE);

  // Create and register output array
  fStripArray = new TClonesArray("PndMvdDigiStrip");
  ioman->Register("MVDStripDigis", "MVD", fStripArray, kTRUE);
  
  // Create and register parameter array
//  fStripArray = new TClonesArray("PndMvdDigiPar");
//  ioman->Register("MVDDigiParam", "MVD", fDigiParRect, kTRUE);
  
  std::cout << "-I- PndMvdStripHitProducer: Initialisation successfull" << std::endl;


  if (!fDigiParRect){
     std::cout<<"-E- PndMvdStripHitProducer: DigiPar Rect Container does not exist!"<<std::endl;
     return kERROR;
  }
  if (!fDigiParTrap){
     std::cout<<"-E- PndMvdStripHitProducer: DigiPar Trap Container does not exist!"<<std::endl;
     return kERROR;
  }
  
  
  if(fVerbose>0) fDigiParRect->Print();
  if(fVerbose>0) fDigiParTrap->Print();

  fStripCalcTopRect = new PndMvdCalcStrip(fDigiParRect, kTOP);
  fStripCalcBotRect = new PndMvdCalcStrip(fDigiParRect, kBOTTOM);
  fStripCalcTopTrap = new PndMvdCalcStrip(fDigiParTrap, kTOP);
  fStripCalcBotTrap = new PndMvdCalcStrip(fDigiParTrap, kBOTTOM);
  fStripCalcTopRect->SetVerboseLevel(fVerbose);
  fStripCalcBotRect->SetVerboseLevel(fVerbose);
  fStripCalcTopTrap->SetVerboseLevel(fVerbose);
  fStripCalcBotTrap->SetVerboseLevel(fVerbose);

  return kSUCCESS;
}    
// -------------------------------------------------------------------------
     


// -----   Public method Exec   --------------------------------------------
void PndMvdStripHitProducer::Exec(Option_t* opt)
{
  // Reset output array
  fStripArray->Clear();
  
  // Declare some variables
  PndMvdMCPoint *point = NULL;

  Int_t detID = 0;       // Detector ID
//     Int_t trackID = 0;     // Track index

  // Loop over PndMvdMCPoints
  Int_t 
    nPoints = fPointArray->GetEntriesFast();
  if (fVerbose > 0){
    std::cout<<" Nr of Points: "<<nPoints<<std::endl;
  }
  
  Int_t iStrip = 0;

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) 
  {
      point = (PndMvdMCPoint*) fPointArray->At(iPoint);
      if( kFALSE == SelectSensorParams(point->GetDetName()) ) continue;
      
      if (fVerbose > 2){
        std::cout<<"***** Strip Digi for "<<fCurrentDigiPar->GetSensType()<<" ******"<<std::endl;
        std::cout<<" DetName : "<<fGeoH->GetPath(point->GetDetName())<<std::endl;
      }
      if ( ! point){
        std::cout<< "No Point!" << std::endl;
         continue;
      }
      if (fVerbose > 2){
        std::cout << "****Global Point: " << std::endl;
        point->Print("");
      }

      // transform to local sensor system... (mc point has the ID not the path to the volume)
      TVector3 posInL = fGeoH->MasterToLocalId(point->GetPosition(),point->GetDetName());
      TVector3 posOutL = fGeoH->MasterToLocalId(point->GetPositionOut(),point->GetDetName());
     
      if (fVerbose > 2){
        posInL.Print();posOutL.Print();
        std::cout << "Energy: " << point->GetEnergyLoss() << std::endl;
      }
      detID   = point->GetDetectorID();

      // Top Side
      if (fVerbose > 2) std::cout  << "Top Side: " << std::endl;
      // Calculate a cluster of Strips fired
      std::vector<PndMvdStrip> topStrips =
        fCurrentStripCalcTop->GetStrips(posInL.X(),  posInL.Y(),  posInL.Z(),
                                        posOutL.X(), posOutL.Y(), posOutL.Z(),
                                        point->GetEnergyLoss());
      if (topStrips.size() != 0)
      {
        if (fVerbose > 1) std::cout  << "SensorStrips: " << std::endl;
        for(std::vector<PndMvdStrip>::const_iterator kit=topStrips.begin();
            kit!= topStrips.end(); ++kit)
        {
            new ((*fStripArray)[iStrip])
                PndMvdDigiStrip(iPoint,detID,point->GetDetName(),
                    fCurrentStripCalcTop->CalcFEfromStrip(kit->GetIndex()),
                    fCurrentStripCalcTop->CalcChannelfromStrip(kit->GetIndex()),kit->GetCharge());
            if (fVerbose > 1) std::cout << *kit << std::endl;
            iStrip++;
        }
      }else if(fVerbose>2) std::cout<<"Top side empty"<<std::endl;
      
      // Bottom Side
      if (fVerbose > 2) std::cout  << "Bottom Side: " << std::endl;
      std::vector<PndMvdStrip> botStrips =
        fCurrentStripCalcBot->GetStrips(posInL.X(),  posInL.Y(),  posInL.Z(),
                                        posOutL.X(), posOutL.Y(), posOutL.Z(),
                                        point->GetEnergyLoss());
      if (botStrips.size() != 0)
      {
        if (fVerbose > 2) std::cout  << " SensorStrips: " << std::endl;
        for(std::vector<PndMvdStrip>::const_iterator kit=botStrips.begin();
            kit!= botStrips.end();
            ++kit)
        {
            new ((*fStripArray)[iStrip])
                PndMvdDigiStrip(iPoint,detID,point->GetDetName(),
                    fCurrentStripCalcBot->CalcFEfromStrip(kit->GetIndex()) + fCurrentDigiPar->GetNrTopFE(),
                    fCurrentStripCalcBot->CalcChannelfromStrip(kit->GetIndex()),kit->GetCharge());
            if (fVerbose > 2) std::cout << *kit << std::endl;
            iStrip++;
        }
      } else if(fVerbose>2) std::cout<<"Bottom side empty"<<std::endl;

  } // Loop over MCPoints

  // Event summary
  if(fVerbose > 1) std::cout << "-I- PndMvdStripHitProducer: " << nPoints << " PndMvdMCPoints, "
       << iStrip << " Digis created."<< std::endl;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

// void PndMvdStripHitProducer::GetLocalHitPoints(PndMvdMCPoint* myPoint, TVector3& myHitIn, TVector3& myHitOut)
// {
//   
//   if (fVerbose > 1)
//     std::cout << "GetLocalHitPoints" << std::endl;
//   TGeoHMatrix trans = GetTransformation(fGeoH->GetPath(myPoint->GetDetName()).Data());
//   
//   Double_t posIn[3];
//   Double_t posOut[3];
//   Double_t posInLocal[3];
//   Double_t posOutLocal[3];
//   
//   posIn[0] = myPoint->GetX();
//   posIn[1] = myPoint->GetY();
//   posIn[2] = myPoint->GetZ();
//   
//   posOut[0] = myPoint->GetXOut();
//   posOut[1] = myPoint->GetYOut();
//   posOut[2] = myPoint->GetZOut();
//   
//   if (fVerbose > 1){
//     for (Int_t i = 0; i < 3; i++)
//       std::cout << "posIn "<< i << ": " << posIn[i] << std::endl;
//   
//     trans.Print("");
//   }
//   
//   trans.MasterToLocal(posIn, posInLocal);
//   trans.MasterToLocal(posOut, posOutLocal);
//   
//   if (fVerbose > 1) {
//     for (Int_t i = 0; i < 3; i++){
//       std::cout << "posInLocal "<< i << ": " << posInLocal[i] << std::endl;
//       std::cout << "posOutLocal "<< i << ": " << posOutLocal[i] << std::endl;
//     }
//   }
//   
//   //posIn/OutLocal have the center of the coordinate system in the center of the shape
//   //typically sensors have their coordinate system centered at the lower left corner
//   
// //   TVector3 offset = GetSensorDimensions(fGeoH->GetPath(myPoint->GetDetName()).Data());
//   
// //   posInLocal[0] += 0.5*offset.x();
// //   posInLocal[1] += 0.5*offset.y();
// //   //posInLocal[2] += offset.z();
// //   
// //   posOutLocal[0] += 0.5*offset.x();
// //   posOutLocal[1] += 0.5*offset.y();
// //   //posOutLocal[2] += offset.z();
//   
//   myHitIn.setVector(posInLocal);
//   myHitOut.setVector(posOutLocal);
//     
// }
// -------------------------------------------------------------------------




// TGeoHMatrix PndMvdStripHitProducer::GetTransformation(std::string detName) const
// {
//   gGeoManager->cd(detName.c_str());
//   TGeoHMatrix* transMat = gGeoManager->GetCurrentMatrix();
//   if (fVerbose > 1)
//   transMat->Print("");
//   return *transMat;
// }
// -------------------------------------------------------------------------
// 
// TVector3 PndMvdStripHitProducer::GetSensorDimensions(std::string detName) const
// {
//   gGeoManager->cd(detName.c_str());
//   TGeoVolume* actVolume = gGeoManager->GetCurrentVolume();
//   TGeoBBox* actBox = (TGeoBBox*)(actVolume->GetShape());
//   TVector3 result;
//   result.SetX(actBox->GetDX());
//   result.SetY(actBox->GetDY());
//   result.SetZ(actBox->GetDZ());
//   
//   //result.Dump();
//   
//   return result;
// }
// -------------------------------------------------------------------------


Bool_t PndMvdStripHitProducer::SelectSensorParams(TString detname)
{
      /// TODO change this to a switch on DetID==2
  TString detpath = fGeoH->GetPath(detname);
  if( !(detpath.Contains("Strip")) )
    return kFALSE;

  if(detpath.Contains(fDigiParRect->GetSensType()))  {
    fCurrentStripCalcTop = fStripCalcTopRect;
    fCurrentStripCalcBot = fStripCalcBotRect;
    fCurrentDigiPar = fDigiParRect;
  }else if(detpath.Contains(fDigiParTrap->GetSensType()))  {
    fCurrentStripCalcTop = fStripCalcTopTrap;
    fCurrentStripCalcBot = fStripCalcBotTrap;
    fCurrentDigiPar = fDigiParTrap;
  }else{
    if (fVerbose > 1) std::cout<<"detector name does not contain 'Rect' or 'Trap'"<<std::endl;
    if (fVerbose > 2) std::cout<<" DetName : "<<detpath<<std::endl;
    return kFALSE;
  }
  return kTRUE;
}

// -------------------------------------------------------------------------


ClassImp(PndMvdStripHitProducer);

