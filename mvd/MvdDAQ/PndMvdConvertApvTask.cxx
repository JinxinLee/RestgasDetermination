// -------------------------------------------------------------------------
// -----                PndMvdConvertApvTast source file               -----
// -----                  Created 13/01/09  by L.Ackermann             -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>
#include <map>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"


// framework includes
#include "FairRootManager.h"
// PndMvd includes
#include "PndMvdConvertApvTask.h"
#include "PndMvdConvertApv.h"

#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndMvdContFact.h"
#include "PndSdsStripDigiPar.h"

#include "PndSdsDigiStrip.h"
#include <vector>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndMvdConvertApvTask::PndMvdConvertApvTask(PndMvdConvertApv* Apvconvert,PndMvdMapApv* Apvmapper) : FairTask("Convert Task for PANDA PndMvd")
{
  fApvConvert=Apvconvert;
  fApvMapper=Apvmapper;
  fPersistance = kTRUE;
}
// -----   Destructor   ----------------------------------------------------
PndMvdConvertApvTask::~PndMvdConvertApvTask()
{
  if(fGeoH) delete fGeoH;
}

void PndMvdConvertApvTask::SetParContainers()
{ // in this task we even don't need the digitization info
  if(0==fGeoH) fGeoH = new PndGeoHandling();
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  PndMvdContFact* themvdcontfact = (PndMvdContFact*)rtdb->getContFactory("PndMvdContFact");
  TList* theContNames = themvdcontfact->GetDigiParNames();
  Info("SetParContainers()","The container names list contains %i entries",theContNames->GetEntries());
  TIter cfIter(theContNames);
  while (TObjString* contname = (TObjString*)cfIter()) {
    TString parsetname = contname->String();
    if(parsetname.BeginsWith("MVDStripDigiPar")){
      PndSdsStripDigiPar* digipar = (PndSdsStripDigiPar*)(rtdb->getContainer(parsetname.Data()));
      fDigiParameterList->Add(digipar);
      if(digipar->GetNrBotFE()==1)
      { // we count top side first from 0; Bot side strarts at #top fe's
        fBotSides[(TString)digipar->GetSensType()]=digipar->GetNrTopFE();
      }
      Info("SetParContainers()","Loaded container %s",parsetname.Data());
    }
  }  
  
  return;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdConvertApvTask::Init()
{
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman )
  {
    std::cout << "-E- PndMvdStripHitProducer::Init: "
    << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  
  // Create and register output array
  fStripArray = new TClonesArray("PndSdsDigiStrip");
  ioman->Register("MVDStripDigis", "MVD", fStripArray, fPersistance);
  fApvConvert->Init();
  fApvMapper->Init();
  cout<<"Init of Task"<<endl;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdConvertApvTask::Exec(Option_t* opt)
{
  // Reset output array
	fStripArray->Delete();
  
	std::map<Int_t,Double_t> singleSidedBacksideMap;
  std::map<Int_t,std::vector<Int_t> > buffIndex;
  
  Int_t rw=-1, sw=-1, botfe=-1;
  TString detpath=""; 
  Int_t detnameid;
  Int_t stripnum;
  
	std::vector<PndSdsDigiStrip> strips = fApvConvert->ReadNext();
	for (std::vector<PndSdsDigiStrip>::iterator strip=strips.begin(); strip!=strips.end(); ++strip)
	{
    rw=strip->GetFE();
    fApvMapper->DoMapping(rw,sw,detpath);
    detnameid=fGeoH->GetShortID(detpath);
    
    if(fVerbose>1) Info("Exec","Write a Digi from detector %s %i",detpath.Data(),detnameid);
    stripnum = fStripArray->GetEntriesFast();
    //cout << "stripnum: " << stripnum << endl;
		new ((*fStripArray)[stripnum]) PndSdsDigiStrip(strip->GetIndices(), strip->GetDetID(),
                                                   detnameid, sw, strip->GetChannel(), strip->GetCharge(),kUnknown ,strip->GetTimestamp());
    // collect information of fake bottom sides if singlesided
		if (IsSingleSided(detpath))
	  {    // collect information of fake bottom sides if singlesided
      singleSidedBacksideMap[detnameid]+= strip->GetCharge();
      (buffIndex[detnameid]).push_back(strip->GetIndex());
	  }
	}
  
  // writing the single sided fake backside
	for (std::map<Int_t,Double_t>::iterator it=singleSidedBacksideMap.begin();it!=singleSidedBacksideMap.end();++it)
  {
    stripnum = fStripArray->GetEntriesFast();
    botfe=CalcBotFakeFE( fGeoH->GetPath(it->first) );
    new ((*fStripArray)[stripnum]) PndSdsDigiStrip(buffIndex[it->first], kMVDHitsStrip,it->first, botfe, 0, it->second,kUnknown, 0);
  }
  
}

Bool_t PndMvdConvertApvTask::IsSingleSided(TString &detpath)
{
  if( !(detpath.Contains("Strip")) )   return kFALSE;
  TIter parsetiter(fDigiParameterList);
  while ( PndSdsStripDigiPar* digipar = (PndSdsStripDigiPar*)parsetiter() ) 
  {
    const char* sensortype = digipar->GetSensType();
    if(detpath.Contains(sensortype))  {
      return kTRUE;
    }
  }
  return kFALSE;
}

Int_t PndMvdConvertApvTask::CalcBotFakeFE(TString detpath)
{
  for(std::map<TString,Int_t>::iterator it=fBotSides.begin();it!=fBotSides.end();it++)
  {
    if( detpath.Contains(it->first) )
    {
      return it->second;
    }
  }
  return -1;
}

void PndMvdConvertApvTask::Finish()
{}

ClassImp(PndMvdConvertApvTask);
