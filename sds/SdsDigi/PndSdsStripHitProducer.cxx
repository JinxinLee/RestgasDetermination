// -------------------------------------------------------------------------
// -----                PndSdsStripHitProducer source file             -----
// -------------------------------------------------------------------------


// This Class
#include "PndSdsStripHitProducer.h"
// SDS
#include "PndSdsMCPoint.h"
#include "PndSdsCalcStrip.h"
#include "PndSdsDigiStrip.h"
//PANDA
#include "PndStringSeparator.h"
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
#include "TRandom.h"

// -----   Default constructor   -------------------------------------------
PndSdsStripHitProducer::PndSdsStripHitProducer() :
PndSdsTask("SDS Strip Digi Producer(PndSdsStripHitProducer)"), fDataBuffer(0)
{
  fOverrideParams = false;
  fDigiParameterList = new TList();
  fChargeDigiParameterList = new TList();
  fPersistance = kTRUE;
  fGeoH = PndGeoHandling::Instance();
  fTimeOrderedDigi = kFALSE;
}
// -------------------------------------------------------------------------

// -----   Default constructor   -------------------------------------------
PndSdsStripHitProducer::PndSdsStripHitProducer(const char* name) :
PndSdsTask(name), fDataBuffer(0)
{
  fOverrideParams = false;
  fDigiParameterList = new TList();
  fChargeDigiParameterList = new TList();
  fPersistance = kTRUE;
  fGeoH = PndGeoHandling::Instance();
  fTimeOrderedDigi = kFALSE;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSdsStripHitProducer::~PndSdsStripHitProducer()
{
  if (0!=fDigiParameterList) delete fDigiParameterList;
  if (0!=fChargeDigiParameterList) delete fChargeDigiParameterList;
  if (0!=fDataBuffer) delete fDataBuffer;
  // TODO: needs check: now cleared correctly?
  for( std::map<const char*,PndSdsCalcStrip*>::iterator it = fStripCalcTop.begin(); it != fStripCalcTop.end(); it++){
    if(0 != it->second) delete it->second;
    it->second = 0;
  }
  for( std::map<const char*,PndSdsCalcStrip*>::iterator it = fStripCalcBot.begin(); it != fStripCalcBot.end(); it++){
    if(0 != it->second) delete it->second;
    it->second = 0;
  }
  for(std::map<const char*,PndSdsChargeConversion*>::iterator it = fChargeConverter.begin(); it != fChargeConverter.end(); it++){
    if(0 != it->second) delete it->second;
    it->second = 0;
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
InitStatus PndSdsStripHitProducer::ReInit()
{
  SetParContainers();
  SetCalculators();
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void PndSdsStripHitProducer::SetCalculators()
{
  // After the first start if the Init() tis can be set properly.
  
  TIter params(fDigiParameterList);
  while(PndSdsStripDigiPar* digipar=(PndSdsStripDigiPar*)params()){
    if(0==digipar) {
      Error("SetCalculators()","A Digi Parameter Set does not exist properly.");
      continue;
    }
    const char* senstype = digipar->GetSensType();
    if(fVerbose>1){
      Info("SetCalculators()","Create a Parameter Set for %s sensors",senstype);
      std::cout<<senstype<<"#"<<std::endl;
    }
    if(fVerbose>2)digipar->Print();
    //TODO switch also with PndSdsCalcStripDif
    fStripCalcTop[senstype]=new PndSdsCalcStrip(digipar,kTOP);
    fStripCalcTop[senstype]->SetVerboseLevel(fVerbose);
    fStripCalcBot[senstype]=new PndSdsCalcStrip(digipar,kBOTTOM);
    fStripCalcBot[senstype]->SetVerboseLevel(fVerbose);
  }
}

// -------------------------------------------------------------------------

void PndSdsStripHitProducer::SetParContainers()
{
  if(fVerbose>1) Info("SetParContainers","done.");
	return;
}


// -----   Public method Init   --------------------------------------------
InitStatus PndSdsStripHitProducer::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();
  
  SetBranchNames();
  
  
  if ( ! ioman )
  {
    std::cout << "-E- PndSdsStripHitProducer::Init: "
    << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  
  fPointArray = (TClonesArray*) ioman->GetObject(fInBranchName);
  if ( ! fPointArray )
  {
    std::cout << "-E- PndSdsStripHitProducer::Init: "
    << "No "<<fInBranchName<<" array!" << std::endl;
    return kERROR;
  }
  
  
  //  fMcEventHeader = (FairMCEventHeader*) ioman->GetObject("MCEventHeader.");
  //  if(!fMcEventHeader)
  //  {
  //    std::cout << "-E- PndSdsStripHitProducer::Init: "
  //    << "No MCEventHeader. array!" << std::endl;
  //    return kERROR;
  //  }
  
  // Create and register output array
  // fStripArray = new TClonesArray("PndSdsDigiStrip");
  //fStripArray = ioman->Register(fOutBranchName, "PndSdsDigiStrip", fFolderName, fPersistance);
  //  fDataBuffer = new PndWriteoutBufferT<PndSdsDigiStrip>(fOutBranchName, "PndSdsDigiStrip");
  
  fDataBuffer = new PndSdsDigiStripWriteoutBuffer(fOutBranchName, fFolderName, fPersistance);
  fDataBuffer = (PndSdsDigiStripWriteoutBuffer*)ioman->RegisterWriteoutBuffer(fOutBranchName, fDataBuffer);

  fDataBuffer->ActivateBuffering(fTimeOrderedDigi);

  SetCalculators();
  
  if(fVerbose>0){
    std::cout << "-I- PndSdsStripHitProducer: Initialisation successfull with these parameters:" << std::endl;
    TIter params(fDigiParameterList);
    while(PndSdsStripDigiPar* digipar=(PndSdsStripDigiPar*)params()){
      if(0!=digipar) {
        digipar->Print();
      }
    }
  }
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndSdsStripHitProducer::Exec(Option_t* opt)
{
  // Reset output array
  fGeoH->SetVerbose(fVerbose);
  for (std::map<const char*,PndSdsChargeConversion*>::iterator it = fChargeConverter.begin(); it != fChargeConverter.end(); it++){
	  it->second->StartExecute();
  }
  // Declare some variables
  PndSdsMCPoint *point = NULL;
  
  //  Int_t detID = 0;       // Detector ID
  //     Int_t trackID = 0;     // Track index
  
  fStripArray = FairRootManager::Instance()->GetTClonesArray(fOutBranchName);
  
  //  if (fTimeOrderedDigi) {
  //		fDataBuffer->WriteOutData(FairRootManager::Instance()->GetEventTime());
  //	}
  
  
  // Loop over PndSdsMCPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  if (fVerbose > 0){
    std::cout<<" Nr of Points: "<<nPoints<<std::endl;
  }
  //  if(!fMcEventHeader) Error("Exec", "No Fair MC event header found. Why?? %p",fMcEventHeader);
  // std::cout << "MCEventHeader in StripHitProducer: " << fMcEventHeader->GetT() << std::endl;
  Double_t timestamp = 0;
  Double_t smearedCharge = 0;
  Int_t iStrip = 0;
  Bool_t selected = kFALSE;
  
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++)
  {
    point = (PndSdsMCPoint*) fPointArray->At(iPoint);
    selected = SelectSensorParams(point->GetSensorID());
    if( !selected ) { continue; }
    
    if (fVerbose > 2){
      std::cout<<"***** Strip Digi for "<<fCurrentDigiPar->GetSensType()<<" ******"<<std::endl;
      std::cout<<" DetName : "<<fGeoH->GetPath(point->GetSensorID())<<std::endl;
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
    TVector3 posInL = fGeoH->MasterToLocalShortId(point->GetPosition(),point->GetSensorID());
    TVector3 posOutL = fGeoH->MasterToLocalShortId(point->GetPositionOut(),point->GetSensorID());
    if (fVerbose > 2){
      posInL.Print();posOutL.Print();
      std::cout << "Energy: " << point->GetEnergyLoss() << std::endl;
    }
    //      detID   = point->GetDetectorID();
    
    // Top Side
    if (fVerbose > 2) std::cout  << "Top Side: " << std::endl;
    // Calculate a cluster of Strips fired
    std::vector<PndSdsStrip> topStrips =
    fCurrentStripCalcTop->GetStrips(posInL.X(),  posInL.Y(),  posInL.Z(),
                                    posOutL.X(), posOutL.Y(), posOutL.Z(),
                                    point->GetEnergyLoss());
    
    if (topStrips.size() != 0)
    {
      if (fVerbose > 1) std::cout  << "SensorStrips: " << std::endl;
      for(std::vector<PndSdsStrip>::const_iterator kit=topStrips.begin();
          kit!= topStrips.end(); ++kit)
      {   
        //        if (fTimeOrderedDigi == kFALSE){
        //    	  AddDigi(iStrip,iPoint,FairRootManager::Instance()->GetBranchId(fInBranchName),point->GetSensorID(),
        //            		fCurrentStripCalcTop->CalcFEfromStrip(kit->GetIndex()),
        //            		fCurrentStripCalcTop->CalcChannelfromStrip(kit->GetIndex()),kit->GetCharge());
        //        }
        //        else{
        std::vector<Int_t>indices;
        indices.push_back(iPoint);
        PndSdsDigiStrip* tempStrip = new PndSdsDigiStrip(indices, FairRootManager::Instance()->GetBranchId(fInBranchName), point->GetSensorID(),
                                                         fCurrentStripCalcTop->CalcFEfromStrip(kit->GetIndex()),
                                                         fCurrentStripCalcTop->CalcChannelfromStrip(kit->GetIndex()), kit->GetCharge(), FairRootManager::Instance()->GetEventTime());
        
        fDataBuffer->FillNewData(tempStrip,	FairRootManager::Instance()->GetEventTime() + 100);
        //        }
        if (fVerbose > 1) std::cout << *kit << std::endl;
      }
    }else if(fVerbose>2) std::cout<<"Top side empty"<<std::endl;
    
    // Bottom Side
    if (fVerbose > 2) std::cout  << "Bottom Side: " << std::endl;
    std::vector<PndSdsStrip> botStrips =
    fCurrentStripCalcBot->GetStrips(posInL.X(),  posInL.Y(),  posInL.Z(),
                                    posOutL.X(), posOutL.Y(), posOutL.Z(),
                                    point->GetEnergyLoss());
    if (botStrips.size() != 0)
    {
      if (fVerbose > 2) std::cout  << " SensorStrips: " << std::endl;
      for(std::vector<PndSdsStrip>::const_iterator kit=botStrips.begin();
          kit!= botStrips.end();
          ++kit)
      {
        //    	  if (fTimeOrderedDigi == kFALSE) {
        //    		  AddDigi(iStrip, iPoint, FairRootManager::Instance()->GetBranchId(fInBranchName),
        //							point->GetSensorID(), fCurrentStripCalcBot->CalcFEfromStrip(kit->GetIndex())+ fCurrentDigiPar->GetNrTopFE(),
        //							fCurrentStripCalcBot->CalcChannelfromStrip(kit->GetIndex()), kit->GetCharge());
        //    	  } else{
        std::vector<Int_t>indices;
        indices.push_back(iPoint);
        // FairMCEventHeader* MCevtHeader = (FairMCEventHeader*)FairRootManager::Instance()->GetObject("MCEventHeader.");
        
        PndSdsDigiStrip* tempStrip = new PndSdsDigiStrip(indices, FairRootManager::Instance()->GetBranchId(fInBranchName),
                                                         point->GetSensorID(), fCurrentStripCalcBot->CalcFEfromStrip(kit->GetIndex())+ fCurrentDigiPar->GetNrTopFE(),
                                                         fCurrentStripCalcBot->CalcChannelfromStrip(kit->GetIndex()), kit->GetCharge(), FairRootManager::Instance()->GetEventTime());
			  fDataBuffer->FillNewData(tempStrip,	FairRootManager::Instance()->GetEventTime() + 100);
        //    	  }
        if (fVerbose > 2) std::cout << *kit << std::endl;
      }
    } else if(fVerbose>2) std::cout<<"Bottom side empty"<<std::endl;
    
  } // Loop over MCPoints
  
  Int_t indexnum=0;
  Double_t tempstamp=0;
  // Loop over PndSdsDigis and convert charge to digi value
  for (Int_t i = 0; i<fStripArray->GetEntriesFast(); i++){
	  PndSdsDigiStrip* finDigi = (PndSdsDigiStrip*) fStripArray->At(i);
	  SelectSensorParams(finDigi->GetSensorID());
    smearedCharge = finDigi->GetCharge(); // Gaussian noise smearing done in PndCalcStrip
                                          //FIXME: This is not elegant and error prone, for Tasks afterwards will not know how we digitized!
	  finDigi->SetCharge(fCurrentChargeConverter->ChargeToDigiValue(smearedCharge));
    if(fVerbose>4)Info("Exec","Loop over all digis to convert cahrge: Q=%f; D=%f",smearedCharge,finDigi->GetCharge());
    //    indexnum = finDigi->GetNIndices();
    //    for(Int_t ind=0;ind<indexnum;ind++)
    //    {
    //      point = (PndSdsMCPoint*) fPointArray->At(ind);
    //      tempstamp = DigitizeTime(point->GetTime(),smearedCharge);
    //      if(tempstamp < timestamp) timestamp = tempstamp;
    //    }
    //    finDigi->SetTimeStamp(timestamp); //todo check timestamp
  }
  
  for (std::map<const char*,PndSdsChargeConversion*>::iterator it = fChargeConverter.begin(); it != fChargeConverter.end(); it++){
    it->second->EndExecute();
  }
  
  // Event summary
  if(fVerbose > 1) std::cout << "-I- PndSdsStripHitProducer: " << nPoints << " PndSdsMCPoints, "
    << iStrip << " Digis created."<< std::endl;
}
// -------------------------------------------------------------------------

void PndSdsStripHitProducer::AddDigi(Int_t &iStrip, Int_t iPoint, Int_t detID, Int_t sensorID, Int_t fe, Int_t chan, Double_t charge)
{
  if(fVerbose>3)Info("AddDigi","called");
  PndSdsDigiStrip* aDigi = 0;
  for(Int_t kstr = 0; kstr < iStrip ; kstr++)
  {
    // search if that channel fired already
    aDigi = (PndSdsDigiStrip*)fStripArray->At(kstr);
    if ( aDigi->GetDetID() == detID &&
        aDigi->GetSensorID() == sensorID &&
        aDigi->GetFE() == fe &&
        aDigi->GetChannel() == chan )
    {
      aDigi->AddCharge(charge);
      aDigi->AddIndex(iPoint);
      return;
    }
  }
  // we're here when this channel didn't fire
  std::vector<Int_t>indices;
  indices.push_back(iPoint);
  
  PndSdsDigiStrip* tempStrip = new ((*fStripArray)[iStrip]) PndSdsDigiStrip(indices,detID,sensorID,fe,chan,charge, FairRootManager::Instance()->GetEventTime());
  tempStrip->SetEntryNr(FairLink(FairRootManager::Instance()->GetBranchId(fOutBranchName), iStrip));
  
  iStrip++;
  return;
}
// -------------------------------------------------------------------------


Bool_t PndSdsStripHitProducer::SelectSensorParams(Int_t sensorID)
{
  fCurrentDigiPar = NULL;
  fCurrentStripCalcTop = NULL;
  fCurrentStripCalcBot = NULL;
  fCurrentChargeConverter = NULL;
  
  TString detpath = fGeoH->GetPath(sensorID);
  if( !(detpath.Contains("Strip")) )
  { // filter from pixel points
    return kFALSE;
  }
  
  TIter parsetiter(fDigiParameterList);
  while ( PndSdsStripDigiPar* digipar = (PndSdsStripDigiPar*)parsetiter() ) 
  {
    const char* sensortype = digipar->GetSensType();
    if(detpath.Contains(sensortype))  {
      fCurrentStripCalcTop = fStripCalcTop[sensortype];
      fCurrentStripCalcBot = fStripCalcBot[sensortype];
      fCurrentChargeConverter = fChargeConverter[sensortype];
      fCurrentDigiPar = digipar;
      return kTRUE;
    }
  }
  // no suiting object found
  //if (fVerbose > 1) 
  if(fVerbose>1) Info("SelectSensorParams()","No valid sensor parameters selected, skipping this point.");
  std::cout<<"detector name does not contain a valid parameter name."<<std::endl;
  //if (fVerbose > 2)
  std::cout<<" DetName : "<<detpath<<std::endl;
  return kFALSE;
}

Int_t PndSdsStripHitProducer::DigitizeTime(Double_t time, Double_t charge)
{ // time [ns]
  Double_t eventTime = FairRootManager::Instance()->GetEventTime();
  return fCurrentChargeConverter->GetTimeStamp(time,charge,eventTime);
}

//______________________________________________________________________________
// -------------------------------------------------------------------------

void PndSdsStripHitProducer::FinishEvent()
{
  // called after all Tasks did their Exex() and the data is copied to the file
  //  fStripArray->Delete();
  FinishEvents();
}
// -------------------------------------------------------------------------

void PndSdsStripHitProducer::FinishTask()
{
  // called after all Tasks did their Exex() and the data is copied to the file
  // fPixelArray->Delete();
  // FinishEvents();
  
  //	if (fTimeOrderedDigi){
  //		fDataBuffer->WriteOutAllData();
  //	}
  
  
}

ClassImp(PndSdsStripHitProducer);

