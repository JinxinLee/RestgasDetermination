//-------------------------------------------------------//
//     Class PndMvdCreateDefaultApvMap                   //
//                                                       //
// Creates a default Frontend number mapping file        //
//                                                       //
// Created 16.02.2010                                    //
// Author: Ralf Kliemt                                   //
// ralf.kliemt{at}hiskp.uni-bonn.de                      //
//-------------------------------------------------------//

#include "PndMvdCreateDefaultApvMap.h"

#include <iostream>
#include <fstream>

#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndMvdContFact.h"
#include "TObjString.h"
#include "TObjArray.h"

ClassImp(PndMvdCreateDefaultApvMap);

// Constructor
PndMvdCreateDefaultApvMap::PndMvdCreateDefaultApvMap()
{
  fDigiParameterList = new TList();
}

// Destructor
PndMvdCreateDefaultApvMap::~PndMvdCreateDefaultApvMap()
{
  if(fDigiParameterList) delete fDigiParameterList;
}

// Setup
Bool_t PndMvdCreateDefaultApvMap::Init()
{
  // Check for all necessary framework i/o
  
  fGeoMan = gGeoManager;
  if(!fGeoMan) {
    Error("Init","Cannot find a valid GeoManager");
    return kFALSE;
  }
  
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  PndMvdContFact* themvdcontfact = (PndMvdContFact*)rtdb->getContFactory("PndMvdContFact");
  TList* theContNames = themvdcontfact->GetDigiParNames();
  Info("Init()","The container names list contains %i entries",theContNames->GetEntries());
  TIter cfIter(theContNames);
  while (TObjString* contname = (TObjString*)cfIter()) {
    TString parsetname = contname->String();
    Info("Init()",parsetname.Data());
    if(parsetname.BeginsWith("MVDStripDigiPar")){
      PndMvdStripDigiPar* digipar = (PndMvdStripDigiPar*)(rtdb->getContainer(parsetname.Data()));
      fDigiParameterList->Add(digipar);
    }
  }
  return kTRUE; 
}

// The main function
Bool_t PndMvdCreateDefaultApvMap::CreateFile(TString outFileName)
{
  Info("CreateFile","Output file name is %s",outFileName.Data());
  Int_t feCount = 0, feSens=0;
  std::ofstream outfile(outFileName.Data());
  TString detname, detpath;
  // loop over volumes in geomanager
  TObjArray* nodes = fGeoMan->GetListOfNodes();
  for(Int_t idx=0;idx<nodes->GetEntriesFast();idx++)
  {
    TGeoNode* anode = (TGeoNode*)nodes->At(idx);
    TGeoVolume* avol = anode->GetVolume();
    // get path & volume name
    detname=avol->GetName();
    if(!detname.Contains("Strip")) continue;
    // select a parameter set
    if(!SelectSensorParams(detname)) continue;
    feSens = fCurrentDigiPar->GetNrTopFE() + fCurrentDigiPar->GetNrBotFE();
    anode->cd();
    detpath = fGeoMan->GetPath();
    
    // count frontends
    for(Int_t fe =0;fe<feSens;fe++)
    {
      // write to file
      outfile << feCount << fe << detpath << std::endl;
      feCount++;
    }
  }
  outfile.close();
  return kTRUE;
}

Bool_t PndMvdCreateDefaultApvMap::SelectSensorParams(TString detname)
{
  TIter parsetiter(fDigiParameterList);
  while ( PndMvdStripDigiPar* digipar = (PndMvdStripDigiPar*)parsetiter() ) 
  {
    const char* sensortype = digipar->GetSensType();
    if(detname.Contains(sensortype))  {
      fCurrentDigiPar = digipar;
      return kTRUE;
    }
  }
  // no suiting object found
  Error("SelectSensorParams","Detector name does not contain a valid parameter name. DetName : %s",detname.Data());
  return kFALSE;
}






