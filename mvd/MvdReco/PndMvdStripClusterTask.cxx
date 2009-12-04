// -------------------------------------------------------------------------
// -----                PndMvdStripClusterTask source file             -----
// -------------------------------------------------------------------------

#include <cmath>

#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TCanvas.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"

#include "PndStringVector.h"
//#include "PndMvdGeoPar.h"
#include "PndMvdStripDigiPar.h"
#include "PndMvdStripClusterTask.h"
#include "PndMvdMCPoint.h"
#include "PndMvdCalcStrip.h"
#include "PndMvdDigiStrip.h"
// #include "PndMvdStripCluster.h"
#include "PndMvdCluster.h"
#include "PndMvdGeoHandling.h"

#include "PndMvdSimpleStripClusterFinder.h"
#include "PndMvdStripClusterFinder.h"
#include "PndMvdChargeWeightingAlgorithms.h"

#include <map>

// enum SensorSide { kTOP, kBOTTOM };

// -----   Default constructor   -------------------------------------------

PndMvdStripClusterTask::PndMvdStripClusterTask(Int_t ClusterMod, Int_t RadChannel, Int_t RadTime) :
  FairTask("MVD Strip Clustertisation Task")
{
  fChargeCut = 1.e8; // this ist really large and shall have no effect
  fGeoFile = "";
  fClusterMod=ClusterMod;
  if(fClusterMod>1 || fClusterMod<0) fClusterMod=0;
  fRadChannel=RadChannel;
  fRadTime=RadTime;
}

// -------------------------------------------------------------------------


PndMvdStripClusterTask::PndMvdStripClusterTask(Double_t chargecut, TString geofile, Int_t meanalgo, Int_t clustermod, Int_t RadChannel, Int_t RadTime) :
  FairTask("MVD Strip Clustertisation Task")
{
  fChargeCut = chargecut;
  fGeoFile = geofile;
  fMeanAlgo=meanalgo;
  fClusterMod=clustermod;
  if(fClusterMod>1 || fClusterMod<0) fClusterMod=0;
  fRadChannel=RadChannel;
  fRadTime=RadTime;
}

// -----   Destructor   ----------------------------------------------------
PndMvdStripClusterTask::~PndMvdStripClusterTask()
{
  if(0!=fGeoH)  delete fGeoH;
  if(0!=fChargeAlgos) delete fChargeAlgos;
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdStripClusterTask::SetParContainers()
{
  // Get Base Container

  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
 // fGeoPar = (PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));
  fDigiParRect = (PndMvdStripDigiPar*)(rtdb->getContainer("MVDStripDigiParRect"));
  fDigiParTrap = (PndMvdStripDigiPar*)(rtdb->getContainer("MVDStripDigiParTrap"));
}

InitStatus PndMvdStripClusterTask::ReInit()
{

  InitStatus stat=kERROR;
  return stat;

  /*
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar=(PndMvdGeoPar*)(rtdb->getContainer("PndMvdGeoPar"));

  return kSUCCESS;
  */
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdStripClusterTask::Init()
{

  FairRootManager* ioman = FairRootManager::Instance();

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
  //if (fGeoFile=="") fGeoFile = ioman->GetInFile()->GetName();
  //fGeoH = new PndMvdGeoHandling(fGeoFile.Data());
  fGeoH = new PndMvdGeoHandling(gGeoManager);

//   else *infile = new TFile(fGeoFile);
//   TGeoManager *geoMan = (TGeoManager*) infile->Get("FAIRGeom");
//   std::cout << "-I- geoMan in StripClusterTask is  = "<<geoMan << std::endl;
//   fGeoH = new PndMvdGeoHandling(geoMan);
SetParContainers();
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

  fChargeAlgos = new PndMvdChargeWeightingAlgorithms(fDigiArray);

  std::cout << "-I- PndMvdStripClusterTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndMvdStripClusterTask::Exec(Option_t* opt)
{
  if (fVerbose > 2)
    std::cout<<" **Sarting PndMvdStripClusterTask::Exec()**"<<std::endl;
  std::vector<PndMvdDigiStrip> digiStripArray;
  // Reset output array
  if ( ! fClusterArray ) Fatal("Exec", "No ClusterArray");
  fClusterArray->Delete();
  if ( ! fHitArray ) Fatal("Exec", "No HitArray");
  fHitArray->Delete();

   // a std::map is a SORTED container, it is sorted by the identifier
  TString detName;
 // Int_t nPoints = fDigiArray->GetEntriesFast();
  Int_t strip;
  SensorSide side;
  PndMvdDigiStrip* myDigi=0;
 // PndMvdCluster* myCandTop=0;
 // PndMvdCluster* myCandBot=0;

  // load the Clusterfinder
  if(0==fClusterMod) { fClusterfinder = new PndMvdSimpleStripClusterFinder( fRadChannel ); //search radius in channel no.
  }else if(1==fClusterMod) {fClusterfinder = new PndMvdStripClusterFinder(fRadChannel, fRadTime);}
  // Sort Digi indice into the clusterfinder
  for (Int_t iPoint = 0; iPoint < fDigiArray->GetEntriesFast(); iPoint++)
  { // sort digis by sensor name and stripnumber
    myDigi = (PndMvdDigiStrip*)(fDigiArray->At(iPoint));
    detName = myDigi->GetDetName().Data();
    SelectSensorParams(detName);
    //we use the top side as "first" side
    fStripCalcTop->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
    fClusterfinder->AddDigi(detName.Data(),side,myDigi->GetTimestamp(),strip,iPoint);
  }

  std::vector< PndMvdCluster > clusters;
  std::vector< Int_t > topclusters;// contains index to fClusterArray
  std::vector< Int_t > botclusters;// contains index to fClusterArray
  std::vector< Int_t > oneclustertop;
  std::vector< Int_t > oneclusterbot;
  std::vector< Int_t > leftDigis;
  Int_t detID, mcindex, clindex, botIndex;
  Double_t mycharge;
  TVector2 meantopPoint, meanbotPoint, onsensorPoint;
  TVector3 hitPos,hitErr;


  // -------   SEARCH  ------
  clusters = fClusterfinder->SearchClusters();
  // fetch ids in 'clusters' to the top and bot side
  topclusters = fClusterfinder->GetTopClusterIDs();
  botclusters = fClusterfinder->GetBotClusterIDs();
  if(fVerbose > 2) {
    leftDigis = fClusterfinder->GetLeftDigiIDs();
    if (0<leftDigis.size()){
      std::cout << "There are "<<leftDigis.size()<<" Digis not assigned to"
                << " clusters:\n";
      for(unsigned int s=0;s<leftDigis.size();s++)
      {
        std::cout<<leftDigis[s]<<"|";
      }
      std::cout<<std::endl;
    } else std::cout<<"All Digis assigned to clusters"<<std::endl;
  }
  // Fill the ClonesArray for output TODO: do this better, don't copy objects around
  for(std::vector< PndMvdCluster >::iterator clit= clusters.begin();
        clit!=clusters.end(); ++clit)
  {
    clindex = fClusterArray->GetEntriesFast();
    new((*fClusterArray)[clindex]) PndMvdCluster(*clit);
  }

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


  // begin the hit reconstruction
  // loop structure:
  //
  //top clusters
  // |-calculate chargeweighted mean
  // |-bot clusters
  // |-|-calc chargew. mean
  // |-|-calc the crossing strip points
  // |-|-fill hit array

    // -----  merge top/bot clusters to hits  -----
    // loop on clusters from the top side
  mcindex = -1;
  for (std::vector< Int_t>::iterator itTop = topclusters.begin();
          itTop!=topclusters.end(); ++itTop)
  {
    Double_t topcharge = 0., meantopstrip=0.,meantoperr=0. ;
    oneclustertop = (clusters[*itTop]).GetClusterList();
    if(oneclustertop.size()<1) continue;
    PndMvdDigiStrip* atopDigi = ((PndMvdDigiStrip*)fDigiArray->At(oneclustertop[0]));
    TString detnametop = atopDigi->GetDetName();

    SelectSensorParams(detnametop);
    detID = atopDigi->GetDetID();

    CalcMeanCharge(oneclustertop,meantopstrip,meantoperr,topcharge);

    if(oneclustertop.size()==1 && topcharge < 5200) { 
		std::cout<<"-W- PndMvdClusterTask::Exec: Single strip top charge bigger than 5200 e- : skiping. "<<endl; 
	    continue; 
	  }
    if(topcharge>0)
    {
      if(mcindex < 0) {//look for the first digi from a MC point
        for(Int_t mcI = 0; mcI<atopDigi->GetNIndices();mcI++){ 
          if (atopDigi->GetIndex(mcI) > -1) {
            mcindex = atopDigi->GetIndex(mcI);
            break;
          }
        }
      }
      fStripCalcTop->CalcStripPointOnLine(meantopstrip, meantopPoint);
      // loop on bottom side
      for (std::vector< Int_t>::iterator itBot = botclusters.begin();
            itBot!=botclusters.end(); ++itBot)
      {
        botIndex = *itBot;
        Double_t botcharge = 0., meanbotstrip=0., meanboterr=0.;
        oneclusterbot = (clusters[*itBot]).GetClusterList();
        if(oneclusterbot.size()<1)continue;
        PndMvdDigiStrip* abotDigi = ((PndMvdDigiStrip*)fDigiArray->At(oneclusterbot[0]));
        TString detnamebot = abotDigi->GetDetName();

        //go to the next cluster if we didn't hit the same sensor
        if(detnamebot != detnametop) continue;

        CalcMeanCharge(oneclusterbot,meanbotstrip,meanboterr,botcharge);

        if(oneclusterbot.size() == 1 && botcharge < 5200) { 
          std::cout<<"-W- PndMvdClusterTask::Exec: Single strip bot charge bigger than 5200 e- : skiping. "<<endl;
		      continue; 
		    }
        if(botcharge>0)
        {
          if(fVerbose > 2)  {
            std::cout<<"Charges: Ctop = "<<topcharge<<" | Cbot = "<<botcharge
                  <<" | difference bot-top = "<<botcharge-topcharge<<std::endl;
          }

          if(fabs(botcharge-topcharge)<fChargeCut)
          {// look if the charges are not too differently
            mycharge = (botcharge + topcharge) / 2.;
            fStripCalcBot->CalcStripPointOnLine(meanbotstrip, meanbotPoint);
            if(mcindex < 0) {//look for the first digi from a MC point
              for(Int_t mcI = 0; mcI<abotDigi->GetNIndices();mcI++){ 
                if (abotDigi->GetIndex(mcI) > -1) {
                  mcindex = abotDigi->GetIndex(mcI);
                  break;
                }
              }
            }
            Bool_t test = Backmap(meantopPoint, meantoperr, meanbotPoint, meanboterr, hitPos, hitErr,detnametop);
            if (kFALSE==test) continue;

            // --- add hit to list ---
            Int_t i = fHitArray->GetEntriesFast();
            new((*fHitArray)[i]) PndMvdHit(detID,detnametop.Data(),hitPos,hitErr,
                *itTop,mycharge,oneclusterbot.size()+oneclustertop.size(),mcindex);
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

  if(0!=fClusterfinder) delete fClusterfinder;
  return;
}

TVector2 PndMvdStripClusterTask::CalcLineCross(
              TVector2 point1, TVector2 dir1,
              TVector2 point2, TVector2 dir2) const
{
  Double_t dx, dy, s, M, x, y;
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
  fChargeAlgos->SetCalcStrip(fStripCalcTop);
//   fChargeCut = fCurrentDigiPar->GetThreshold(); ??
//   fChargeCut = factor * fCurrentDigiPar->GetNoise(); ?? TODO
}

void PndMvdStripClusterTask::Finish()
{
}


void PndMvdStripClusterTask::CalcMeanCharge(std::vector<Int_t> &onecluster, Double_t &meanstrip, Double_t &meanerr, Double_t &charge)
{

	//if (fMeanAlgo == 0)
	//{
      // Calculate mean position in position channels weighted by the charges
		  Int_t strip;
		  SensorSide side;
		  for (std::vector<Int_t>::iterator itDigi = onecluster.begin();
		        itDigi != onecluster.end(); ++itDigi)
		  { // calculate the mean charge and stripnumber
		    PndMvdDigiStrip* myDigi = (PndMvdDigiStrip*)fDigiArray->At(*itDigi);
		    fStripCalcTop->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
		    charge += myDigi->GetCharge();
		    meanstrip += myDigi->GetCharge() * strip;
		    meanerr += myDigi->GetCharge()*myDigi->GetCharge(); 
		  }
		  meanstrip = meanstrip/charge;
		  // this error treatment is: dx = dpitch * sqrt(weigthsquares)
  		    meanerr = sqrt(meanerr/(charge*charge));
		  if(side==kTOP){
		    meanerr = meanerr * (fCurrentDigiPar->GetTopPitch()/sqrt(12.));
		  }else{
			meanerr = meanerr * (fCurrentDigiPar->GetBotPitch()/sqrt(12.));
		  }
	//} else {
	//	//TODO: Apply other clusterfinder mean & error algorithms
	//	if(fVerbose>1)std::cout<<"-W- PndMvdStripClusterTask::CalcMeanCharge: Using a preliminary Chargeweighting, please set fMeanAlgo = 0 ."<<std::endl;
	//	fChargeAlgos->center_of_gravity(onecluster);
	//}




}



Bool_t PndMvdStripClusterTask::Backmap( TVector2 meantopPoint, Double_t meantoperr, TVector2 meanbotPoint, Double_t meanboterr,
		TVector3 &hitPos, TVector3 &hitErr, TString &detname)
{
  // BACKMAPPING
  // get the backmapped point

  TVector3 localpos, locDpos;
  Double_t t, b;
  Double_t errZ = 2.*fGeoH->GetSensorDimensionsId(detname).Z()/TMath::Sqrt(12.0);

  TVector2 onsensorPoint = 
    CalcLineCross(meantopPoint, fStripCalcTop->GetStripDirection(), meanbotPoint, fStripCalcBot->GetStripDirection() );
  // here we assume the sensor system to be in the _Middle_ of the volume
  localpos.SetXYZ( onsensorPoint.X(), onsensorPoint.Y(), 0.);

  // let's see if we're still on the sensor (cut combinations with noise off)
  if(fabs(localpos.X()) > fabs(fCurrentDigiPar->GetTopAnchor().X())) return kFALSE;
  if(fabs(localpos.Y()) > fabs(fCurrentDigiPar->GetTopAnchor().Y())) return kFALSE;

  //do the transformation from sensor to lab frame
  hitPos = fGeoH->LocalToMasterId(localpos,detname.Data());

  // calculate the errors corresponding to a skewed system!
  t = meantoperr*fCurrentDigiPar->GetTopPitch()*cos(fCurrentDigiPar->GetOrient());
  b = meanboterr*fCurrentDigiPar->GetBotPitch()*cos(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
  locDpos.SetX( sqrt(t*t+b*b) );
  t = meantoperr*fCurrentDigiPar->GetTopPitch()*sin(fCurrentDigiPar->GetOrient());
  b = meanboterr*fCurrentDigiPar->GetBotPitch()*sin(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
  locDpos.SetY( sqrt(t*t+b*b) );
  locDpos.SetZ( errZ );
  
  //do the transformation from sensor to lab frame
  hitErr = fGeoH->LocalToMasterErrorsId(locDpos,detname.Data());

  return kTRUE;
}


ClassImp(PndMvdStripClusterTask);

