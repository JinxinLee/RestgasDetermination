// -------------------------------------------------------------------------
// -----                PndMvdStripHitProducer source file             -----
// -----                Ralf Kliemt                                    -----
// -------------------------------------------------------------------------

//This Class
#include "PndMvdStripHitProducer.h"
//MVD
#include "PndMvdCalcStrip.h"
#include "PndMvdDigiStrip.h"
#include "PndMvdMCPoint.h"
#include "PndMvdContFact.h"
//PANDA
#include "PndStringVector.h"
#include "PndDetectorList.h"
//FAIR
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairContFact.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"
//ROOT
#include "TClonesArray.h"
#include "TArrayD.h"
#include "TVector2.h"
#include "TString.h"
#include "TObjString.h"
#include "TGeoManager.h"
#include "TList.h"

// -----   Default constructor   -------------------------------------------
PndMvdStripHitProducer::PndMvdStripHitProducer() :
  FairTask("MVD Strip Digi Producer(PndMvdStripHitProducer)")
{
  fBranchName   = "MVDPoint";
  fOverrideParams = false;
  fDigiParameterList = new TList();
}
// -------------------------------------------------------------------------

//PndMvdStripHitProducer::PndMvdStripHitProducer(Double_t topPitch, Double_t botPitch,
//                                         Double_t ori, Double_t skew,
//                                         TVector2 topAnchor, TVector2 botAnchor,
//                                         Int_t nrTopFE, Int_t nrBotFE, Int_t nrFECh,
//                                         Double_t threshold, Double_t noise,
//                                         TString sensorType, TString feType) :
//  FairTask("MVD Strip Digi Producer")
//{
//  // This constructor is not needed anymore, since the parameters are
//  // read in via an ascii file!
//  fDigiParameterList = new TList();
//  Warning("PndMvdStripHitProducer::PndMvdStripHitProducer()",
//          "Obsolete constructor for PndMvdStripHitProducer called. \nMvd strip sensors in barrel and disk are set to the SAME type.");
//
//  fBranchName   = "MVDPoint";
//  fOverrideParams = true;
//  SetParamSet(topPitch,botPitch,ori,skew,topAnchor,botAnchor,nrTopFE,nrBotFE,
//              nrFECh,threshold,noise,"Rect",feType);
//  SetParamSet(topPitch,botPitch,ori,skew,topAnchor,botAnchor,nrTopFE,nrBotFE,
//              nrFECh,threshold,noise,"Trap",feType);
//  std::cout << "MVD Strip Digi Producer initiated" << std::endl;
//  
//}
//
//void PndMvdStripHitProducer::SetParamSet(Double_t topPitch, Double_t botPitch,
//                                         Double_t ori, Double_t skew,
//                                         TVector2 topAnchor, TVector2 botAnchor,
//                                         Int_t nrTopFE, Int_t nrBotFE, Int_t nrFECh,
//                                         Double_t threshold, Double_t noise,
//                                         TString sensorType, TString feType)
//{
//  if ( 0==fDigiParRect || 0==fDigiParTrap ) SetParContainers();
//  if (fOverrideParams){
//    if (sensorType.Contains("Rect")) fCurrentDigiPar = fDigiParRect;
//    else if (sensorType.Contains("Trap")) fCurrentDigiPar = fDigiParTrap;
//    fCurrentDigiPar->SetTopPitch(topPitch);
//    fCurrentDigiPar->SetBotPitch(botPitch);
//    fCurrentDigiPar->SetSkew(skew);
//    fCurrentDigiPar->SetOrient(ori);
//    fCurrentDigiPar->SetTopAnchor(topAnchor);
//    fCurrentDigiPar->SetBotAnchor(botAnchor);
//    fCurrentDigiPar->SetNrFECh(nrFECh);
//    fCurrentDigiPar->SetNrTopFE(nrTopFE);
//    fCurrentDigiPar->SetNrBotFE(nrBotFE);
//    fCurrentDigiPar->SetThreshold(threshold);
//    fCurrentDigiPar->SetNoise(noise);
//    fCurrentDigiPar->SetSensType(sensorType);
//    fCurrentDigiPar->SetFeType(feType);
//    fCurrentDigiPar->setChanged();
//    fCurrentDigiPar->setInputVersion(FairRun::Instance()->GetRunId(),1);
//  }
//
//
//}
//// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMvdStripHitProducer::~PndMvdStripHitProducer()
{
  if (0!=fGeoH) delete fGeoH;
  if (0!=fDigiParameterList) delete fDigiParameterList;
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdStripHitProducer::SetParContainers()
{
  // called from the FairRun::Init()
  // Caution: The Parameter Set is not filled from the DB IO, yet. 
  // This will be done just before this Tasks Init() is called.
  
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  PndMvdContFact* themvdcontfact = (PndMvdContFact*)rtdb->getContFactory("PndMvdContFact");
  TList* theContNames = themvdcontfact->GetDigiParNames();
  Info("SetParContainers()","The container names list contains %i entries",theContNames->GetEntries());
  TIter cfIter(theContNames);
  while (TObjString* contname = (TObjString*)cfIter()) {
    TString parsetname = contname->String();
    Info("SetParContainers()",parsetname.Data());
    if(parsetname.BeginsWith("MVDStripDigiPar")){
      PndMvdStripDigiPar* digipar = (PndMvdStripDigiPar*)(rtdb->getContainer(parsetname.Data()));
      digipar->Print();
      fDigiParameterList->Add(digipar);
    }
  }
  
//  fDigiParRect = (PndMvdStripDigiPar*)(rtdb->getContainer("MVDStripDigiParRect"));
//  fDigiParTrap = (PndMvdStripDigiPar*)(rtdb->getContainer("MVDStripDigiParTrap"));
}

InitStatus PndMvdStripHitProducer::ReInit()
{
  SetParContainers();
  SetCalculators();
  return kSUCCESS;
}

void PndMvdStripHitProducer::SetCalculators()
{
  // After the first start if the Init() tis can be set properly.
  
  TIter params(fDigiParameterList);
  while(PndMvdStripDigiPar* digipar=(PndMvdStripDigiPar*)params()){
    if(0==digipar) {
      Error("SetCalculators()","A Digi Parameter Set does not exist properly.");
      continue;
    }
    const char* senstype = digipar->GetSensType();
    if(fVerbose>1){
      Info("SetCalculators()","Create a Parameter Set for %s sensors",senstype);
      std::cout<<senstype<<"#"<<std::endl;
    }
    if(fVerbose>0)digipar->Print();
    fStripCalcTop[senstype]=new PndMvdCalcStrip(digipar,kTOP);
    fStripCalcTop[senstype]->SetVerboseLevel(fVerbose);
    fStripCalcBot[senstype]=new PndMvdCalcStrip(digipar,kBOTTOM);
    fStripCalcBot[senstype]->SetVerboseLevel(fVerbose);
  }
    
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdStripHitProducer::Init()
{
 // FairRun* ana = FairRun::Instance();
  FairRootManager* ioman = FairRootManager::Instance();

  fGeoH = new PndMvdGeoHandling(gGeoManager);

  //std::cout << "-I- PndMvdStripHitProucer::Init() " << fGeoH->GetPath("1_1/212_0/") << std::endl;

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
  
  SetCalculators();

  std::cout << "-I- PndMvdStripHitProducer: Initialisation successfull" << std::endl;


//  if (!fDigiParRect){
//     std::cout<<"-E- PndMvdStripHitProducer: DigiPar Rect Container does not exist!"<<std::endl;
//     return kERROR;
//  }
//  if (!fDigiParTrap){
//     std::cout<<"-E- PndMvdStripHitProducer: DigiPar Trap Container does not exist!"<<std::endl;
//     return kERROR;
//  }


//  if(fVerbose>0) fDigiParRect->Print();
//  if(fVerbose>0) fDigiParTrap->Print();

  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndMvdStripHitProducer::Exec(Option_t* opt)
{
  // Reset output array
  fStripArray->Delete();

  // Declare some variables
  PndMvdMCPoint *point = NULL;

//  Int_t detID = 0;       // Detector ID
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
      if( kFALSE == SelectSensorParams(point->GetDetName()) )
      {
        if(fVerbose>0) Error("Exec()","No valid sensor parameters selected, skipping this point.");
        continue;
      }

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
//      detID   = point->GetDetectorID();

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
            AddDigi(iStrip,iPoint,kMVDHitsStrip,point->GetDetName(),
            		fCurrentStripCalcTop->CalcFEfromStrip(kit->GetIndex()),
            		fCurrentStripCalcTop->CalcChannelfromStrip(kit->GetIndex()),kit->GetCharge());
            if (fVerbose > 1) std::cout << *kit << std::endl;
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
            AddDigi(iStrip,iPoint,kMVDHitsStrip,point->GetDetName(),
                    fCurrentStripCalcBot->CalcFEfromStrip(kit->GetIndex()) + fCurrentDigiPar->GetNrTopFE(),
                    fCurrentStripCalcBot->CalcChannelfromStrip(kit->GetIndex()),kit->GetCharge());
            if (fVerbose > 2) std::cout << *kit << std::endl;
        }
      } else if(fVerbose>2) std::cout<<"Bottom side empty"<<std::endl;

  } // Loop over MCPoints

  // Event summary
  if(fVerbose > 1) std::cout << "-I- PndMvdStripHitProducer: " << nPoints << " PndMvdMCPoints, "
       << iStrip << " Digis created."<< std::endl;
}
// -------------------------------------------------------------------------

void PndMvdStripHitProducer::AddDigi(Int_t &iStrip, Int_t iPoint, Int_t detID, TString detname, Int_t fe, Int_t chan, Double_t charge)
{
  Bool_t found = kFALSE;
  PndMvdDigiStrip* aDigi = 0;
  for(Int_t kstr = 0; kstr < iStrip && found==kFALSE ; kstr++)
  {
	aDigi = (PndMvdDigiStrip*)fStripArray->At(kstr);
	if ( aDigi->GetDetID() == detID &&
		 aDigi->GetDetName() == detname &&
		 aDigi->GetFE() == fe &&
		 aDigi->GetChannel() == chan )
	{
		aDigi->AddCharge(charge);
		aDigi->AddIndex(iPoint);
		found = kTRUE;
//		((PndMvdDigiStrip*)(*fStripArray)[kstr])->AddCarge(charge);
//		((PndMvdDigiStrip*)(*fStripArray)[kstr])->AddIndex(iPoint);
//		return;
	}
  }
  if(found == kFALSE){//TODO: Simulate a timestamp
    new ((*fStripArray)[iStrip]) PndMvdDigiStrip(iPoint,detID,detname,fe,chan,0,charge) ;
    iStrip++;
  }
}

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
  TString detpath = fGeoH->GetPath(detname);
  if( !(detpath.Contains("Strip")) )
    return kFALSE;

  TIter parsetiter(fDigiParameterList);
  while ( PndMvdStripDigiPar* digipar = (PndMvdStripDigiPar*)parsetiter() ) 
  {
    const char* sensortype = digipar->GetSensType();
    if(detpath.Contains(sensortype))  {
      
      // TODO: create a list of Calculators OR make calculator switch parameters on the fly
      fCurrentStripCalcTop = fStripCalcTop[sensortype];
      fCurrentStripCalcBot = fStripCalcBot[sensortype];
      fCurrentDigiPar = digipar;
      return kTRUE;
    }
  }
  // no suiting object found
  if (fVerbose > 1) std::cout<<"detector name does not contain a valid parameter name."<<std::endl;
  if (fVerbose > 2) std::cout<<" DetName : "<<detpath<<std::endl;
  return kFALSE;
}

// -------------------------------------------------------------------------


ClassImp(PndMvdStripHitProducer);

