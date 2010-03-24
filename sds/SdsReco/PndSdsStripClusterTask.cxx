// -------------------------------------------------------------------------
// -----                PndSdsStripClusterTask source file             -----
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
#include "FairContFact.h" 

#include "PndStringSeparator.h"
#include "PndSdsStripDigiPar.h"
#include "PndSdsStripClusterTask.h"
#include "PndSdsMCPoint.h"
#include "PndSdsCalcStrip.h"
#include "PndSdsDigiStrip.h"
#include "PndSdsClusterStrip.h"
#include "PndGeoHandling.h"
#include "PndSdsContFact.h" 

#include "PndSdsSimpleStripClusterFinder.h"
#include "PndSdsStripClusterFinder.h"
#include "PndSdsChargeWeightingAlgorithms.h"

#include <map>

//TODO: Remove hardcoding!
// charge cut for cluster charge which consist only of one strip
const Double_t SingleStripChargeThreshold = 3000.;
// enum SensorSide { kTOP, kBOTTOM };

// -----   Default constructor   -------------------------------------------

PndSdsStripClusterTask::PndSdsStripClusterTask() :
  FairTask("SDS Strip Clustertisation Task")
{
  fChargeCut = 1.e8; // this ist really large and shall have no effect
  fDigiParameterList = new TList(); 
}

// -----   Destructor   ----------------------------------------------------
PndSdsStripClusterTask::~PndSdsStripClusterTask()
{
  if(0!=fGeoH)  delete fGeoH;
  if(0!=fChargeAlgos) delete fChargeAlgos;
  if(0!=fDigiParameterList) delete fDigiParameterList; 
}
// -------------------------------------------------------------------------



//
InitStatus PndSdsStripClusterTask::ReInit()
{

  InitStatus stat=kERROR;
  SetParContainers(); 
  SetCalculators(); 
  stat=kSUCCESS; 
  return stat;

}

void PndSdsStripClusterTask::SetCalculators() 
{ 
  // called at the enf of Init() 
   // TODO: Implement more clusterfinders
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
      digipar->Print(); 
    } 
    fStripCalcTop[senstype]=new PndSdsCalcStrip(digipar,kTOP); 
    fStripCalcTop[senstype]->SetVerboseLevel(fVerbose); 
    fStripCalcBot[senstype]=new PndSdsCalcStrip(digipar,kBOTTOM); 
    fStripCalcBot[senstype]->SetVerboseLevel(fVerbose); 
    Int_t ClusterMod = digipar->GetClusterMod(); 
    Int_t RadChannel = digipar->GetRadChannel(); 
    Int_t RadTime    = digipar->GetRadTime(); 
    if(0==ClusterMod) { 
      fClusterFinderList[senstype] = new PndSdsSimpleStripClusterFinder( RadChannel ); //search radius in channel no. 
    } else if(1==ClusterMod) { 
      fClusterFinderList[senstype] = new PndSdsStripClusterFinder(RadChannel, RadTime); 
    } 
 
  } 
   
} 


// -----   Public method Init   --------------------------------------------
InitStatus PndSdsStripClusterTask::Init()
{

  SetBranchNames();

  FairRootManager* ioman = FairRootManager::Instance();

  if ( ! ioman )
    {
      std::cout << "-E- PndSdsStripClusterTask::Init: "
     << "RootManager not instantiated!" << std::endl;
      return kFATAL;
    }

  // Get input array
  fDigiArray = (TClonesArray*) ioman->GetObject(fBranchName);

  if ( ! fDigiArray )
  {
      std::cout << "-W- PndSdsStripClusterTask::Init: "
     << "No SDSDigi array!" << std::endl;
      return kERROR;
  }

  // set output arrays
  fHitArray = new TClonesArray("PndSdsHit");
  ioman->Register(fHitBranchName, fFolderName, fHitArray, kTRUE);

  fClusterArray = new TClonesArray("PndSdsClusterStrip");
  ioman->Register(fClustBranchName, fFolderName, fClusterArray, kTRUE);

  // geo name handling
  fGeoH = new PndGeoHandling(gGeoManager);

  SetCalculators();

  fChargeAlgos = new PndSdsChargeWeightingAlgorithms(fDigiArray);

  std::cout << "-I- PndSdsStripClusterTask: Initialisation successfull with these parameters:" << std::endl; 
  TIter params(fDigiParameterList); 
  while(PndSdsStripDigiPar* digipar=(PndSdsStripDigiPar*)params()){ 
    if(0!=digipar) { 
      digipar->Print(); 
    } 
  }
  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndSdsStripClusterTask::Exec(Option_t* opt)
{
  if (fVerbose > 2)
    std::cout<<" **Sarting PndSdsStripClusterTask::Exec()**"<<std::endl;
  std::vector<PndSdsDigiStrip> digiStripArray;
  // Reset output array
  if ( ! fClusterArray ) Fatal("Exec", "No ClusterArray");
  fClusterArray->Delete();
  if ( ! fHitArray ) Fatal("Exec", "No HitArray");
  fHitArray->Delete();
  ResetClusterFinders();

   // a std::map is a SORTED container, it is sorted by the identifier
  TString detName;
 // Int_t nPoints = fDigiArray->GetEntriesFast();
  Int_t strip;
  SensorSide side;
  PndSdsDigiStrip* myDigi=0;
 // PndSdsCluster* myCandTop=0;
 // PndSdsCluster* myCandBot=0;

  // load the Clusterfinder
  // Sort Digi indice into the clusterfinder
  if (fDigiArray->GetEntriesFast() == 0) return;
  if(fVerbose>2) Info("Exec","adding these digis to the finders:"); 
  for (Int_t iDigi = 0; iDigi < fDigiArray->GetEntriesFast(); iDigi++)
  { // sort digis by sensor name and stripnumber
    myDigi = (PndSdsDigiStrip*)(fDigiArray->At(iDigi));
    if(fVerbose>2) {std::cout<<"Digi "<<iDigi<<" "; myDigi->Print();} 
    detName = myDigi->GetDetName().Data();
    if (kFALSE==SelectSensorParams(detName)) continue; // Invalid parameters, skip here.
    //we use the top side as "first" side
    fCurrentStripCalcTop->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side); 
    fCurrentClusterfinder->AddDigi(detName.Data(),side,myDigi->GetTimestamp(),strip,iDigi); 
 } 

  std::vector< PndSdsClusterStrip > clusters;
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
  clusters = fCurrentClusterfinder->SearchClusters();
  // fetch ids in 'clusters' to the top and bot side
  topclusters = fCurrentClusterfinder->GetTopClusterIDs();
  botclusters = fCurrentClusterfinder->GetBotClusterIDs();
  if(fVerbose > 2) {
    leftDigis = fCurrentClusterfinder->GetLeftDigiIDs();
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
  for(std::vector< PndSdsClusterStrip >::iterator clit= clusters.begin();
        clit!=clusters.end(); ++clit)
  {
    clindex = fClusterArray->GetEntriesFast();
    new((*fClusterArray)[clindex]) PndSdsClusterStrip(*clit);
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
   int clusterIndex = 0; 
  for (std::vector< Int_t>::iterator itTop = topclusters.begin();
          itTop!=topclusters.end(); ++itTop, ++clusterIndex)
  {
    Double_t topcharge = 0., meantopstrip=0.,meantoperr=0. ;
    oneclustertop = (clusters[*itTop]).GetClusterList();
    if(oneclustertop.size()<1) continue;
    PndSdsDigiStrip* atopDigi = ((PndSdsDigiStrip*)fDigiArray->At(oneclustertop[0]));
    TString detnametop = atopDigi->GetDetName();

    if (kFALSE==SelectSensorParams(detName)) continue; // Invalid parameters, skip here. 
    detID = atopDigi->GetDetID();

    CalcMeanCharge(oneclustertop,meantopstrip,meantoperr,topcharge);

    if(oneclustertop.size()==1 && topcharge < SingleStripChargeThreshold) { 
		std::cout<<"-W- PndSdsClusterTask::Exec: Single strip top charge bigger than "<<SingleStripChargeThreshold<<" e- : skiping. "<<endl; 
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
      fCurrentStripCalcTop->CalcStripPointOnLine(meantopstrip, meantopPoint);
      // loop on bottom side
      for (std::vector< Int_t>::iterator itBot = botclusters.begin();
            itBot!=botclusters.end(); ++itBot)
      {
        botIndex = *itBot;
        Double_t botcharge = 0., meanbotstrip=0., meanboterr=0.;
        oneclusterbot = (clusters[*itBot]).GetClusterList();
        if(oneclusterbot.size()<1)continue;
        PndSdsDigiStrip* abotDigi = ((PndSdsDigiStrip*)fDigiArray->At(oneclusterbot[0]));
        TString detnamebot = abotDigi->GetDetName();

        //go to the next cluster if we didn't hit the same sensor
        if(detnamebot != detnametop) continue;

        CalcMeanCharge(oneclusterbot,meanbotstrip,meanboterr,botcharge);

        if(oneclusterbot.size() == 1 && botcharge < SingleStripChargeThreshold) { 
          std::cout<<"-W- PndSdsClusterTask::Exec: Single strip bot charge bigger than "<<SingleStripChargeThreshold<<" e- : skiping. "<<endl;
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
            fCurrentStripCalcBot->CalcStripPointOnLine(meanbotstrip, meanbotPoint);
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
            new((*fHitArray)[i]) PndSdsHit(detID,detnametop.Data(),hitPos,hitErr,
                *itTop,mycharge,oneclusterbot.size()+oneclustertop.size(),mcindex);
            ((PndSdsHit*)((*fHitArray)[i]))->SetBotIndex(*itBot);
          } else
            if (fVerbose > 2) std::cout<<"Strip charge contents too differently"<<std::endl;
        }
      }// loop bot clusters
    }
  }// loop top clusters

  if (fVerbose > 1)
  std::cout << "-I- PndSdsStripClusterTask: " << fClusterArray->GetEntriesFast()
            << " Mvd Clusters and " << fHitArray->GetEntriesFast()<<" Hits calculated."
            << " out of " <<fDigiArray->GetEntriesFast()<< " Digis"<< std::endl;

  fCurrentClusterfinder->ClearDigis(); 
  return;
}

// ------------------------------------------------------------------------- 
Bool_t PndSdsStripClusterTask::SelectSensorParams(TString detname) 
{ 
  TString detpath = fGeoH->GetPath(detname); 
  if( !(detpath.Contains("Strip")) ) 
    return kFALSE; 
   
  TIter parsetiter(fDigiParameterList); 
  while ( PndSdsStripDigiPar* digipar = (PndSdsStripDigiPar*)parsetiter() )  
  { 
    const char* sensortype = digipar->GetSensType(); 
    if(detpath.Contains(sensortype))  { 
       
      // TODO: create a list of Calculators OR make calculator switch parameters on the fly 
      fCurrentStripCalcTop = fStripCalcTop[sensortype]; 
      fCurrentStripCalcBot = fStripCalcBot[sensortype]; 
      fCurrentDigiPar = digipar; 
      fChargeAlgos->SetCalcStrip(fCurrentStripCalcTop); 
      fCurrentClusterfinder = fClusterFinderList[sensortype]; 
      fChargeCut = digipar->GetChargeCut(); 
      return kTRUE; 
    } 
  } 
  // no suiting object found 
  if (fVerbose > 1) std::cout<<"detector name does not contain a valid parameter name."<<std::endl; 
  if (fVerbose > 2) std::cout<<" DetName : "<<detpath<<std::endl; 
  return kFALSE; 
} 
// ------------------------------------------------------------------------- 
 
// ------------------------------------------------------------------------- 
void PndSdsStripClusterTask::ResetClusterFinders() 
{ 
  //recursively clean the digis in the clusterfinder objects each event 
  for(std::map<const char*,PndSdsStripClusterBuilder*>::iterator CFiter = fClusterFinderList.begin(); 
      CFiter != fClusterFinderList.end(); CFiter++) 
  { 
    (CFiter->second)->ClearDigis(); 
  } 
} 
// ------------------------------------------------------------------------- 
 
// ------------------------------------------------------------------------- 
TVector2 PndSdsStripClusterTask::CalcLineCross(
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
    std::cout<<"Warning in PndSdsStripClusterTask::CalcLineCross(): M=0 setting (x,y) to 0"<<std::endl;
    x=0.;y=0.;
  }

  TVector2 result(x,y);
  return result;
}


void PndSdsStripClusterTask::Finish()
{
}


void PndSdsStripClusterTask::CalcMeanCharge(std::vector<Int_t> &onecluster, Double_t &meanstrip, Double_t &meanerr, Double_t &charge)
{

	//if (fMeanAlgo == 0)
	//{
      // Calculate mean position in position channels weighted by the charges
		  Int_t strip;
		  SensorSide side;
		  for (std::vector<Int_t>::iterator itDigi = onecluster.begin();
		        itDigi != onecluster.end(); ++itDigi)
		  { // calculate the mean charge and stripnumber
		    PndSdsDigiStrip* myDigi = (PndSdsDigiStrip*)fDigiArray->At(*itDigi);
		    fCurrentStripCalcTop->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
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
	//	if(fVerbose>1)std::cout<<"-W- PndSdsStripClusterTask::CalcMeanCharge: Using a preliminary Chargeweighting, please set fMeanAlgo = 0 ."<<std::endl;
	//	fChargeAlgos->center_of_gravity(onecluster);
	//}




}



Bool_t PndSdsStripClusterTask::Backmap( TVector2 meantopPoint, Double_t meantoperr, TVector2 meanbotPoint, Double_t meanboterr,
		TVector3 &hitPos, TVector3 &hitErr, TString &detname)
{
  // BACKMAPPING
  // get the backmapped point

  TVector3 localpos, locDpos;
  Double_t t, b;
  Double_t errZ = 2.*fGeoH->GetSensorDimensionsId(detname).Z()/TMath::Sqrt(12.0);

  TVector2 onsensorPoint = 
    CalcLineCross(meantopPoint, fCurrentStripCalcTop->GetStripDirection(), meanbotPoint, fCurrentStripCalcBot->GetStripDirection() );
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


ClassImp(PndSdsStripClusterTask);

