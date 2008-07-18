// -------------------------------------------------------------------------
// -----                PndMvdStripClusterTask source file             -----
// -----                  Created 10/01/06  by V. Friese               -----
// -------------------------------------------------------------------------

#include <cmath>

#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmGeoNode.h"
#include "CbmGeoVector.h"

#include "PndStringVector.h"
#include "PndMvdGeoPar.h"
#include "PndMvdStripDigiPar.h"
#include "PndMvdStripClusterTask.h"
#include "PndMvdMCPoint.h"
#include "PndMvdCalcStrip.h"
#include "PndMvdDigiStrip.h"
// #include "PndMvdStripCluster.h"
#include "PndMvdCluster.h"
#include "PndMvdGeoHandling.h"

#include "PndMvdStripClusterBuilder.h"

#include <map>

// enum SensorSide { kTOP, kBOTTOM };

// -----   Default constructor   -------------------------------------------
PndMvdStripClusterTask::PndMvdStripClusterTask() :
  CbmTask("MVD Strip Clustertisation Task")
{
  fChargeCut = 1.e6; // this ist really large and shall have no effect
  fGeoFile = "";
}
// -------------------------------------------------------------------------

// -----           constructor   -------------------------------------------
PndMvdStripClusterTask::PndMvdStripClusterTask(Double_t chargecut, TString geoFile) :
  CbmTask("MVD Strip Clustertisation Task")
{
  fChargeCut = chargecut;
  fGeoFile = geoFile;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMvdStripClusterTask::~PndMvdStripClusterTask()
{
  if(0!=fGeoH)  delete fGeoH;
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdStripClusterTask::SetParContainers()
{
  // Get Base Container

  CbmRunAna* ana = CbmRunAna::Instance();
  CbmRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));
  fDigiParRect = (PndMvdStripDigiPar*)(rtdb->getContainer("MVDStripDigiParRect"));
  fDigiParTrap = (PndMvdStripDigiPar*)(rtdb->getContainer("MVDStripDigiParTrap"));
}

InitStatus PndMvdStripClusterTask::ReInit()
{

  InitStatus stat=kERROR;
  return stat;

  /*
  CbmRunAna* ana = CbmRunAna::Instance();
  CbmRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar=(PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));

  return kSUCCESS;
  */
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdStripClusterTask::Init()
{

  CbmRootManager* ioman = CbmRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndMvdStripClusterTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fDigiArray = (TClonesArray*) ioman->GetObject("MVDStripDigis");

  if ( ! fDigiArray )
  {
      std::cout << "-W- PndMvdStripClusterTask::Init: "
     << "No MVDDigi array!" << std::endl;
      return kERROR;
  }

  // set output arrays
  fHitArray = new TClonesArray("PndMvdHit");
  ioman->Register("MVDHitsStrip", "MVD", fHitArray, kTRUE);

  fClusterArray = new TClonesArray("PndMvdCluster");
  ioman->Register("MVDStripClusterCand","MVD",fClusterArray,kTRUE);

  // geo name handling
  // This requires a connection to a simulation file!
  if (fGeoFile=="") fGeoFile = ioman->GetInFile()->GetName();
  fGeoH = new PndMvdGeoHandling(fGeoFile.Data());

//   else *infile = new TFile(fGeoFile);
//   TGeoManager *geoMan = (TGeoManager*) infile->Get("CBMGeom");
//   std::cout << "-I- geoMan in StripClusterTask is  = "<<geoMan << std::endl;
//   fGeoH = new PndMvdGeoHandling(geoMan);

  if ( ! fDigiParTrap )
  {
      std::cout << "-W- PndMvdStripClusterTask::Init: "
     << "No fDigiParTrap!" << std::endl;
      return kERROR;
  }
  if ( ! fDigiParRect )
  {
      std::cout << "-W- PndMvdStripClusterTask::Init: "
     << "No fDigiParRect!" << std::endl;
      return kERROR;
  }
  fStripCalcTopRect = new PndMvdCalcStrip(fDigiParRect, kTOP);
  fStripCalcBotRect = new PndMvdCalcStrip(fDigiParRect, kBOTTOM);
  fStripCalcTopTrap = new PndMvdCalcStrip(fDigiParTrap, kTOP);
  fStripCalcBotTrap = new PndMvdCalcStrip(fDigiParTrap, kBOTTOM);
  fStripCalcTopRect->SetVerboseLevel(fVerbose);
  fStripCalcBotRect->SetVerboseLevel(fVerbose);
  fStripCalcTopTrap->SetVerboseLevel(fVerbose);
  fStripCalcBotTrap->SetVerboseLevel(fVerbose);

  std::cout << "-I- PndMvdStripClusterTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndMvdStripClusterTask::Exec(Option_t* opt)
{
  if (fVerbose > 2)
    std::cout<<"Sarting PndMvdStripClusterTask::Exec()"<<std::endl;
  std::vector<PndMvdDigiStrip> digiStripArray;
  // Reset output array
  if ( ! fClusterArray ) Fatal("Exec", "No ClusterArray");
  fClusterArray->Clear();
  if ( ! fHitArray ) Fatal("Exec", "No HitArray");
  fHitArray->Clear();

   // a std::map is a SORTED container, it is sorted by the identifier
  TString detName;
  Int_t nPoints = fDigiArray->GetEntriesFast();
  Int_t strip;
  SensorSide side;
  PndMvdDigiStrip* myDigi=0;
  PndMvdCluster* myCandTop=0;
  PndMvdCluster* myCandBot=0;

  // load the Clusterfinder
  PndMvdStripClusterBuilder clusterbuilder;
  // Sort Digi indice into the clusterbuilder
  for (Int_t iPoint = 0; iPoint < fDigiArray->GetEntriesFast(); iPoint++)
  { // sort digis by sensor name and stripnumber
    myDigi = (PndMvdDigiStrip*)(fDigiArray->At(iPoint));
    detName = myDigi->GetDetName().Data();
    SelectSensorParams(detName);
    //we use the top side as "first" side
    fStripCalcTop->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
    clusterbuilder.AddDigi(detName.Data(),side,strip,iPoint);
  }
    //TODO
    // maybe I shall convert the vectors into PndMvdClustCand objects and fill
    // the fClusterArray



  std::vector< PndMvdCluster > clusters;
  std::vector< Int_t > topclusters;// contains index to fClusterArray
  std::vector< Int_t > botclusters;// contains index to fClusterArray
  std::vector< Int_t > oneclustertop;
  std::vector< Int_t > oneclusterbot;
  TVector2 topDirection, botDirection;
//   TGeoVolume* actVolume;
//   TGeoBBox* actBox;
//   TVector3 sensorDim, 
  TVector3 localpos, locDpos;
  Int_t detID, iDigi, clindex, topIndex, botIndex;
  Double_t mycharge;
  TVector2 meantopPoint, meanbotPoint, onsensorPoint;
  TVector3 hitPos,hitErr;
  Double_t t, b;


  // -------   SEARCH  ------
  clusters = clusterbuilder.SearchClusters();
  // fetch ids in 'clusters' to the top and bot side
  topclusters = clusterbuilder.GetTopClusterIDs();
  botclusters = clusterbuilder.GetBotClusterIDs();

  // Fill the ClonesArray for output TODO: do this better, don't copy objects around
  for(std::vector< PndMvdCluster >::iterator clit= clusters.begin();
        clit!=clusters.end(); ++clit)
  {
    clindex = fClusterArray->GetEntriesFast();
    new((*fClusterArray)[clindex]) PndMvdCluster(*clit);
  }


  // begin the hit reconstruction
  // loop structure:
  //
  //top clusters
  // |-calculate chargeweighted mean
  // |-bot clusters
  // |-|-calc chargew. mean
  // |-|-calc the crossing strip points
  // |-|-fill hit array


  //  -----  search for clusters  -----
  //printout for checking
  if(fVerbose > 2) {
    std::cout<<"Top Clusters: ";
    for (std::vector< Int_t>::iterator itTop = topclusters.begin();
      itTop!=topclusters.end(); ++itTop) {
        std::cout<<*itTop<<" | ";}
    std::cout<<std::endl;
    std::cout<<"Bot Clusters: ";
    for (std::vector< Int_t>::iterator itBot = botclusters.begin();
      itBot!=botclusters.end(); ++itBot) {
      std::cout<<*itBot<<" | ";  }
    std::cout<<std::endl;
  }

    // -----  merge top/bot clusters to hits  -----
    // loop on clusters from the top side
  for (std::vector< Int_t>::iterator itTop = topclusters.begin();
          itTop!=topclusters.end(); ++itTop)
  {
    Double_t topcharge = 0., meantopstrip=0.;
    oneclustertop = (clusters[*itTop]).GetClusterList();
    TString detnametop = ((PndMvdDigiStrip*)fDigiArray->At(oneclustertop[0]))->GetDetName();

    SelectSensorParams(detnametop);
    topDirection = fStripCalcTop->GetStripDirection();
    botDirection = fStripCalcBot->GetStripDirection();
//         detID = 2; // Strip part
    detID = ((PndMvdDigiStrip*)fDigiArray->At(oneclustertop[0]))->GetDetID();

    Double_t lastcharge=0;
    for (std::vector<Int_t>::iterator itTopDigi = oneclustertop.begin();
          itTopDigi != oneclustertop.end(); ++itTopDigi)
    {// I use the temp. variables from the top of this method again
     // calculate the mean charge and stripnumber
      myDigi = (PndMvdDigiStrip*)fDigiArray->At(*itTopDigi);
      fStripCalcTop->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
      topcharge += myDigi->GetCharge();
      meantopstrip += myDigi->GetCharge() * strip;
      if(fVerbose > 2)std::cout<<"FE no: "<<myDigi->GetFE()<<" | channel no: "
        <<myDigi->GetChannel()<<" | topstrip no: "<<strip<<" side="<<side<<std::endl;
    }

    if(topcharge>0)
    {
      meantopstrip = meantopstrip/topcharge;
      fStripCalcTop->CalcStripPointOnLine(meantopstrip, meantopPoint);
      // loop on bottom side
      for (std::vector< Int_t>::iterator itBot = botclusters.begin();
            itBot!=botclusters.end(); ++itBot)
      {
        botIndex = *itBot;
        Double_t botcharge = 0., meanbotstrip=0.;
        oneclusterbot = (clusters[*itBot]).GetClusterList();
        TString detnamebot = ((PndMvdDigiStrip*)fDigiArray->At(oneclusterbot[0]))->GetDetName();

        //go to the next cluster if we didn't hit the same sensor
        if(detnamebot != detnametop) continue;

        for (std::vector<Int_t>::iterator itBotDigi = oneclusterbot.begin();
              itBotDigi != oneclusterbot.end(); ++itBotDigi)
        {// I use the temp. variables from the top of this method again
          // calculate the mean charge and stripnumber
          // here we must use the top calculator to get the correct numbers
          myDigi = (PndMvdDigiStrip*)fDigiArray->At(*itBotDigi);
          //we use the top side as "first" side to get the strip numbers
          fStripCalcTop->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
          botcharge += myDigi->GetCharge();
          meanbotstrip += myDigi->GetCharge() * strip;
          if(fVerbose > 2)std::cout<<"FE no: "<<myDigi->GetFE()<<" | channel no: "
            <<myDigi->GetChannel()<<" | botstrip no: "<<strip<<" side="<<side<<std::endl;
        }

        if(botcharge>0)
        {
          meanbotstrip = meanbotstrip/botcharge;
          if(fVerbose > 2)  {
            std::cout<<"Charges: Ctop = "<<topcharge<<" | Cbot = "<<botcharge
                  <<" | difference bot-top = "<<botcharge-topcharge<<std::endl;
          }

          if(fabs(botcharge-topcharge)<fChargeCut)
          {// look if the charges are not too differently
            mycharge = (botcharge + topcharge) / 2.;
            fStripCalcBot->CalcStripPointOnLine(meanbotstrip, meanbotPoint);
            // get the backmapped point
            onsensorPoint = CalcLineCross(meantopPoint, topDirection,
                                          meanbotPoint, botDirection);
            if (fVerbose > 2){
              std::cout<<"Strip no. top: "<<meantopstrip
                       <<" | bot: "<<meanbotstrip
                       <<" | Mean Top side: ("<<meantopPoint.X()<<" ; "
                       <<meantopPoint.Y()<<")"<<"\n"<<"Mean Bot side: ("
                       <<meanbotPoint.X()<<" ; "<<meanbotPoint.Y()<<")"<<"\n"
                       <<"On Sensor Hit: ("<<onsensorPoint.X()<<" ; "
                       <<onsensorPoint.Y()<<")"<<std::endl;
            }
            // here we assume the sensor system to be in the _Middle_ of the volume
            localpos.SetXYZ( onsensorPoint.X(), onsensorPoint.Y(), 0.);

            //do the transformation from sensor to lab frame
            hitPos = fGeoH->LocalToMasterId(localpos,detnametop.Data());

            // calculate the errors corresponding to a skewed system!
            // TODO: how tot treat the errors correctly?
            // if(clustersize == 1) { "1/sqrt(12)" }
            t = fCurrentDigiPar->GetTopPitch()*cos(fCurrentDigiPar->GetOrient());
            b = fCurrentDigiPar->GetBotPitch()*cos(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
            locDpos.SetX( sqrt((t*t+b*b)/12.) );
            t = fCurrentDigiPar->GetTopPitch()*sin(fCurrentDigiPar->GetOrient());
            b = fCurrentDigiPar->GetBotPitch()*sin(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
            locDpos.SetY( sqrt((t*t+b*b)/12.) );
            locDpos.SetZ( 0. );

            hitErr = fGeoH->LocalToMasterErrorsId(locDpos,detnametop.Data());

            // --- add hit to list ---
            Int_t i = fHitArray->GetEntriesFast();
            new((*fHitArray)[i]) PndMvdHit(detID,detnametop.Data(),hitPos,hitErr,
                *itTop,mycharge,oneclusterbot.size()+oneclustertop.size());
            ((PndMvdHit*)((*fHitArray)[i]))->SetBotIndex(*itBot);
          } else 
            if (fVerbose > 2) std::cout<<"Strip charge contents too differently"<<std::endl;
        }
      }// loop bot clusters
    }
  }// loop top clusters

  if (fVerbose > 1)
  std::cout << "-I- PndMvdStripClusterTask: " << fClusterArray->GetEntriesFast()
            << " Mvd Clusters and " << fHitArray->GetEntriesFast()<<" Hits calculated." 
            << " out of " <<fDigiArray->GetEntriesFast()<< " Digis"<< std::endl;
  return;
}



TVector2 PndMvdStripClusterTask::CalcLineCross(
              TVector2 point1, TVector2 dir1,
              TVector2 point2, TVector2 dir2)
{
  Double_t dx, dy, s, t, M, x, y;
  dx = point2.X() - point1.X();
  dy = point2.Y() - point1.Y();

  M  = dir1.X()*dir2.Y() - dir1.Y()*dir2.X();

  if(M !=0.)
  {
    s  = dir1.Y()*dx/M - dir1.X()*dy/M;
    x  = point2.X() + dir2.X()*s;
    y  = point2.Y() + dir2.Y()*s;
  }
  else
  {
    std::cout<<"Warning in PndMvdStripClusterTask::CalcLineCross(): M=0 setting (x,y) to 0"<<std::endl;
    x=0.;y=0.;
  }

  TVector2 result(x,y);
  return result;
}

void PndMvdStripClusterTask::SelectSensorParams(TString detname)
{
  TString path = fGeoH->GetPath(detname.Data());
  if (path.Contains("Rect"))
  {
    fStripCalcTop = fStripCalcTopRect;
    fStripCalcBot = fStripCalcBotRect;
    fCurrentDigiPar = fDigiParRect;
  }
  else if (path.Contains("Trap"))
  {
    fStripCalcTop = fStripCalcTopTrap;
    fStripCalcBot = fStripCalcBotTrap;
    fCurrentDigiPar = fDigiParTrap;
  }
  else
  {
    std::cout<<"-E- PndMvdStripClusterTask::SelectSensorParams on\n"
                <<"\t"<<path.Data()<<std::endl;
  }
//   fChargeCut = fCurrentDigiPar->GetThreshold(); ??
//   fChargeCut = factor * fCurrentDigiPar->GetNoise(); ??
}


ClassImp(PndMvdStripClusterTask);

