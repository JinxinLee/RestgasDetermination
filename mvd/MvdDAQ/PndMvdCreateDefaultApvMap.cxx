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
  fFeCount = 0;
}

// Destructor
PndMvdCreateDefaultApvMap::~PndMvdCreateDefaultApvMap()
{
  if(fDigiParameterList) delete fDigiParameterList;
  if(fGeoH) delete fGeoH;
}


// Setup
void PndMvdCreateDefaultApvMap::SetParContainers()
{
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  PndMvdContFact* themvdcontfact = (PndMvdContFact*)rtdb->getContFactory("PndMvdContFact");
  TList* theContNames = themvdcontfact->GetDigiParNames();
  Info("SetParContainers()","The container names list contains %i entries",theContNames->GetEntries());
  TIter cfIter(theContNames);
  while (TObjString* contname = (TObjString*)cfIter()) {
    TString parsetname = contname->String();
    if(parsetname.BeginsWith("MVDStripDigiPar")){
      PndMvdStripDigiPar* digipar = (PndMvdStripDigiPar*)(rtdb->getContainer(parsetname.Data()));
      fDigiParameterList->Add(digipar);
      Info("SetParContainers()","Add container %s",parsetname.Data());
    }
  }  
}

InitStatus PndMvdCreateDefaultApvMap::Init()
{
  // Check for all necessary framework i/o
  if(!gGeoManager) {
    Error("Init","Cannot find a valid GeoManager");
    return kERROR;
  }
  fGeoH = new PndMvdGeoHandling(gGeoManager);
  if(!fGeoH) {
    Error("Init","Cannot find a valid MvdGeoHandling");
    return kERROR;
  }
  
  return kSUCCESS; 
}

// The main function
Bool_t PndMvdCreateDefaultApvMap::CreateFile(TString outFileName)
{
  Info("CreateFile","Output file name is %s",outFileName.Data());
  std::ofstream outfile(outFileName.Data(),ofstream::out);
  
  const char* oldpath = gGeoManager->GetPath();
  gGeoManager->CdTop();
  WriteExpandNode(outfile);
  gGeoManager->cd(oldpath);
  
  outfile.close();
  return kTRUE;
}

void PndMvdCreateDefaultApvMap::WriteExpandNode(std::ofstream& outfile)
{
  // this volume should be an assambly
  TGeoNode* node = gGeoManager->GetCurrentNode();
  TGeoVolume* vol = node->GetVolume();
  if(node->GetNdaughters() > 0)
  { // iterate to daughters
    for (Int_t Nod=0; Nod<node->GetNdaughters();Nod++) {
      gGeoManager->CdDown(Nod);
      WriteExpandNode(outfile);
      gGeoManager->CdUp();
    }
  } else { // no daughter nodes
    TString detpath=gGeoManager->GetPath();
    if(!detpath.Contains("Strip")) return;
    if(!SelectSensorParams(detpath)) return;
    Int_t feSens = fCurrentDigiPar->GetNrTopFE() + fCurrentDigiPar->GetNrBotFE();
    TString pathid = fGeoH->GetID(detpath);
    for(Int_t fe =0;fe<feSens;fe++)
    { // write to file
      outfile << fFeCount <<" " << fe <<" " << pathid.Data() << std::endl;
      fFeCount++;
    }
  }
  return;
}



Bool_t PndMvdCreateDefaultApvMap::SelectSensorParams(TString detpath)
{
  TIter parsetiter(fDigiParameterList);
  while ( PndMvdStripDigiPar* digipar = (PndMvdStripDigiPar*)parsetiter() ) 
  {
    const char* sensortype = digipar->GetSensType();
    if(detpath.Contains(sensortype))  {
      fCurrentDigiPar = digipar;
      Info("SelectSensorParams","Found a parameter set for %s", detpath.Data());
      return kTRUE;
    }
  }
  // no suiting object found
  Error("SelectSensorParams","Detector name does not contain a valid parameter name. DetName : %s",detpath.Data());
  return kFALSE;
}


//TString PndMvdCreateDefaultApvMap::FindNodePath(TGeoNode* node)
//{
//  // Find a nodes full path by going there in the gGeoManager
//  // With many volumes this becomes surely slow.
//  const char* oldpath = gGeoManager->GetPath();
//  gGeoManager->CdTop(); // dive down from top node
//  DiveDownToNode(node);
//  TString pathname = gGeoManager->GetPath();
//  gGeoManager->cd(oldpath);
//  return pathname;
//}
//  
//void PndMvdCreateDefaultApvMap::DiveDownToNode(TGeoNode* node)
//{
//  // cd gGeoManager from the current node to a given node
//  TGeoNode *currentNode = gGeoManager->GetCurrentNode();
//  if (currentNode == node) return;
//  for (Int_t iNod=0; iNod<currentNode->GetNdaughters();iNod++)
//  {
//    gGeoManager->CdDown(iNod);
//    DiveDownToNode(node);
//    if (gGeoManager->GetCurrentNode() == node) return;
//    gGeoManager->CdUp();
//  }
//}

  



