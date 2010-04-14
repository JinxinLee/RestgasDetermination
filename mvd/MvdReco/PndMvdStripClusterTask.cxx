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
#include "FairContFact.h"

#include "PndMvdStripDigiPar.h"
#include "PndMvdStripClusterTask.h"
#include "PndMvdMCPoint.h"
#include "PndMvdCalcStrip.h"
#include "PndMvdDigiStrip.h"
// #include "PndMvdStripCluster.h"
#include "PndMvdClusterStrip.h"
#include "PndGeoHandling.h"
#include "PndMvdContFact.h"

#include "PndMvdSimpleStripClusterFinder.h"
#include "PndMvdStripClusterFinder.h"
#include "PndMvdChargeWeightingAlgorithms.h"

#include <map>


//TODO: Remove hardcoding!
// charge cut for cluster charge which consist only of one strip
const Double_t SingleStripChargeThreshold = 3000.;
// enum SensorSide { kTOP, kBOTTOM };

// -----   Default constructor   -------------------------------------------

PndMvdStripClusterTask::PndMvdStripClusterTask() :
  FairTask("MVD Strip Clusterisation Task")
{
  fChargeCut = 1.e8; // this ist really large and shall have no effect
  fDigiParameterList = new TList();
  fDigiParameterList->SetOwner();
  fPersistance = kTRUE;
}

// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndMvdStripClusterTask::~PndMvdStripClusterTask()
{
  if(0!=fGeoH) delete fGeoH;
  if(0!=fDigiParameterList) delete fDigiParameterList;
  for(std::map<const char*,PndMvdStripClusterBuilder*>::iterator it = fClusterFinderList.begin();
        it != fClusterFinderList.end(); it++){
	  if(0 != it->second) delete it->second;
	  it->second = 0;
  }
  for( std::map<const char*,PndMvdCalcStrip*>::iterator it = fStripCalcTop.begin();
          it != fStripCalcTop.end(); it++){
  	  if(0 != it->second) delete it->second;
  	  it->second = 0;
  }
  for( std::map<const char*,PndMvdCalcStrip*>::iterator it = fStripCalcBot.begin();
           it != fStripCalcBot.end(); it++){
   	  if(0 != it->second) delete it->second;
   	  it->second = 0;
   }
  for(std::map<const char*,PndMvdRecoCharge*>::iterator it = fChargeCalc.begin();
          it != fChargeCalc.end(); it++){
	  if(0 != it->second) delete it->second;
  	  it->second = 0;
  }
  for(std::map<const char*,PndMvdChargeWeightingAlgorithms*>::iterator it = fChargeAlgos.begin();
          it != fChargeAlgos.end(); it++){
	  if(0 != it->second) delete it->second;
  	  it->second = 0;
  }
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdStripClusterTask::SetParContainers()
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
      fDigiParameterList->Add(digipar);
    }
  }
}

InitStatus PndMvdStripClusterTask::ReInit()
{
  SetParContainers();
  SetCalculators();
  InitStatus stat=kSUCCESS;
  return stat;
  
}

void PndMvdStripClusterTask::SetCalculators()
{
  // called at the enf of Init()
  
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
      digipar->Print();
    }
    fStripCalcTop[senstype]=new PndMvdCalcStrip(digipar,kTOP);
    fStripCalcTop[senstype]->SetVerboseLevel(fVerbose);
    fStripCalcBot[senstype]=new PndMvdCalcStrip(digipar,kBOTTOM);
    fStripCalcBot[senstype]->SetVerboseLevel(fVerbose);
    fChargeCalc[senstype]=new PndMvdRecoCharge(digipar->GetRaisingTime(),digipar->GetFallingRatio(),digipar->GetThreshold());
    fChargeAlgos[senstype] = new PndMvdChargeWeightingAlgorithms(fDigiArray, digipar->GetRaisingTime(),digipar->GetFallingRatio(),digipar->GetThreshold());
    Int_t ClusterMod = digipar->GetClusterMod();
    Int_t RadChannel = digipar->GetRadChannel();
    Int_t RadTime    = digipar->GetRadTime();
    if(0==ClusterMod) {
      fClusterFinderList[senstype] = new PndMvdSimpleStripClusterFinder( RadChannel ); //search radius in channel no.
    } else if(1==ClusterMod) {
      fClusterFinderList[senstype] = new PndMvdStripClusterFinder(RadChannel, RadTime);
    }

  }
  
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
  ioman->Register("MVDHitsStrip", "MVD", fHitArray, fPersistance);

  fClusterArray = new TClonesArray("PndMvdClusterStrip");
  ioman->Register("MVDStripClusterCand","MVD",fClusterArray,fPersistance);

  // geo name handling
  fGeoH = new PndGeoHandling(gGeoManager);

  SetCalculators();

  std::cout << "-I- PndMvdStripClusterTask: Initialisation successfull with these parameters:" << std::endl;
  TIter params(fDigiParameterList);
  while(PndMvdStripDigiPar* digipar=(PndMvdStripDigiPar*)params()){
    if(0!=digipar) {
      digipar->Print();
    }
  }

  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndMvdStripClusterTask::Exec(Option_t* opt)
{
  if (fVerbose > 2)
    std::cout<<" **Starting PndMvdStripClusterTask::Exec()**"<<std::endl;
  std::vector<PndMvdDigiStrip> digiStripArray;
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
  PndMvdDigiStrip* myDigi=0;
 // PndMvdCluster* myCandTop=0;
 // PndMvdCluster* myCandBot=0;

  // load the Clusterfinder
  // Sort Digi indice into the clusterfinder
  if (fDigiArray->GetEntriesFast() == 0) return;
  if(fVerbose>2) Info("Exec","adding these digis to the finders:");
  for (Int_t iDigi= 0; iDigi < fDigiArray->GetEntriesFast(); iDigi++)
  { // sort digis by sensor name and stripnumber
    myDigi = (PndMvdDigiStrip*)(fDigiArray->At(iDigi));
    if(fVerbose>2) {std::cout<<"Digi "<<iDigi<<" "; myDigi->Print();}
    detName = myDigi->GetDetName().Data();
    if (kFALSE==SelectSensorParams(detName)) continue; // Invalid parameters, skip here.
    //we use the top side as "first" side
    fCurrentStripCalcTop->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
    fCurrentClusterfinder->AddDigi(detName.Data(),side,myDigi->GetTimestamp(),strip,iDigi);
  }

  std::vector< PndMvdClusterStrip > clusters;
  std::vector< Int_t > topclusters;// contains index to fClusterArray
  std::vector< Int_t > botclusters;// contains index to fClusterArray
  std::vector< Int_t > oneclustertop;
  std::vector< Int_t > oneclusterbot;
  std::vector< Int_t > leftDigis;
  Int_t detID, mcindex, clindex, botIndex, topIndex;
  Double_t mycharge;
  TVector2 meantopPoint, meanbotPoint, onsensorPoint;
  TVector3 hitPos,hitErr;
  Int_t oldClusterOffset = 0, newClusterOffset = 0;

  // -------   SEARCH  ------
  TIter parsetiter(fDigiParameterList);
  while ( PndMvdStripDigiPar* digipar = (PndMvdStripDigiPar*)parsetiter() ) 
  { // loop over all parameter sets and their filled finders (representing sensor types)
    SetCurrentCalculators(digipar);
    
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
      } else std::cout<<"All Digis assigned to clusters for sensortype "<<digipar->GetSensType()<<std::endl;
    }
    // Fill the ClonesArray for output TODO: do this better, don't copy objects around
    for(std::vector< PndMvdClusterStrip >::iterator clit= clusters.begin();
        clit!=clusters.end(); ++clit)
    {
      clindex = fClusterArray->GetEntriesFast();
      new((*fClusterArray)[clindex]) PndMvdClusterStrip(*clit);
    }
    newClusterOffset = fClusterArray->GetEntriesFast();
    
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
      topIndex=*itTop;
      Double_t topcharge = 0., meantopstrip=0.,meantoperr=0. ;
      oneclustertop = (clusters[*itTop]).GetClusterList();
      if(oneclustertop.size()<1) continue;
      PndMvdDigiStrip* atopDigi = ((PndMvdDigiStrip*)fDigiArray->At(oneclustertop[0]));
      TString detnametop = atopDigi->GetDetName();
      
      //if (kFALSE==SelectSensorParams(detName)) continue; // Invalid parameters, skip here.
      detID = atopDigi->GetDetID();
      
      CalcMeanCharge(oneclustertop,meantopstrip,meantoperr,topcharge);

      if(oneclustertop.size()==1 && topcharge < SingleStripChargeThreshold) { 
        std::cout<<"-W- PndMvdClusterTask::Exec: Single strip charge falls below the threshold of "<<SingleStripChargeThreshold<<"e- : skipping. "<<endl; 
        continue; 
      }
      if(topcharge <= 0) { // not a sane charge
        Error("Exec() - Hit combination","Not a sane charge calculated, skip cluster %i", topIndex);
        continue;
      }
      if(topcharge < 0) { // not a sane strip number
        Error("Exec() - Hit combination","Not a sane mean calculated, skip cluster %i", topIndex);
        continue;
      }
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
        PndMvdDigiStrip* abotDigi = ((PndMvdDigiStrip*)fDigiArray->At(oneclusterbot[0]));
        TString detnamebot = abotDigi->GetDetName();
        
        //go to the next cluster if we didn't hit the same sensor
        if(detnamebot != detnametop) continue;
        
        CalcMeanCharge(oneclusterbot,meanbotstrip,meanboterr,botcharge);

        if(oneclusterbot.size() == 1 && botcharge < SingleStripChargeThreshold) { 
          std::cout<<"-W- PndMvdClusterTask::Exec: Single strip charge falls below the threshold of "<<SingleStripChargeThreshold<<"e- : skipping. "<<endl; 
          continue; 
        }
        if(botcharge <= 0) { // not a sane charge
          Error("Exec() - Hit combination","Not a sane charge calculated, skip cluster %i", botIndex);
          continue;
        }
        if(meanbotstrip < 0) { // not a sane strip number
          Error("Exec() - Hit combination","Not a sane mean calculated, skip cluster %i", botIndex);
          continue;
        }
        
        if(fVerbose > 2)  {
          std::cout<<"Charges: Ctop = "<<topcharge<<" | Cbot = "<<botcharge
          <<" | difference bot-top = "<<botcharge-topcharge
          <<" | Cut at "<<fChargeCut<<std::endl;
        }
        
        if(fChargeCut>0 && fabs(botcharge-topcharge)<fChargeCut)
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
          new((*fHitArray)[i]) PndMvdHit(detID,detnametop.Data(),hitPos,hitErr,
                                         topIndex,mycharge,oneclusterbot.size()+oneclustertop.size(),mcindex);
          ((PndMvdHit*)((*fHitArray)[i]))->SetBotIndex(botIndex);
          ((PndMvdHit*)((*fHitArray)[i]))->SetLink(FairLink(kMVDClusterStrip, topIndex+oldClusterOffset));
          ((PndMvdHit*)((*fHitArray)[i]))->AddLink(FairLink(kMVDClusterStrip, botIndex+oldClusterOffset));
          
        } else {
          if (fVerbose > 2) std::cout<<"Cluster charge contents too different"<<std::endl;
        }
      }// loop bot clusters
    }// loop top clusters
    oldClusterOffset = newClusterOffset;
  }//loop finders
  if (fVerbose > 1)
  std::cout << "-I- PndMvdStripClusterTask: " << fClusterArray->GetEntriesFast()
            << " Mvd Clusters and " << fHitArray->GetEntriesFast()<<" Hits calculated."
            << " out of " <<fDigiArray->GetEntriesFast()<< " Digis"<< std::endl;
  fCurrentClusterfinder->ClearDigis();
  return;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
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
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Bool_t PndMvdStripClusterTask::SelectSensorParams(TString detname)
{
  TString detpath = fGeoH->GetPath(detname);
  if( !(detpath.Contains("Strip")) )
    return kFALSE;
  
  TIter parsetiter(fDigiParameterList);
  while ( PndMvdStripDigiPar* digipar = (PndMvdStripDigiPar*)parsetiter() ) 
  {
    const char* sensortype = digipar->GetSensType();
    if(detpath.Contains(sensortype))  {
      SetCurrentCalculators(digipar);
      return kTRUE;
    }
  }
  // no suiting object found
  if (fVerbose > 1) std::cout<<"detector name does not contain a valid parameter name."<<std::endl;
  if (fVerbose > 2) std::cout<<" DetName : "<<detpath<<std::endl;
  return kFALSE;
}
// -------------------------------------------------------------------------
void PndMvdStripClusterTask::SetCurrentCalculators(PndMvdStripDigiPar* digipar)
{
  const char* sensortype = digipar->GetSensType();
  fCurrentStripCalcTop = fStripCalcTop[sensortype];
  fCurrentStripCalcBot = fStripCalcBot[sensortype];
  fCurrentDigiPar = digipar;
  fCurrentChargeAlgos = fChargeAlgos[sensortype];
  fCurrentChargeAlgos->SetCalcStrip(fCurrentStripCalcTop);
  fCurrentClusterfinder = fClusterFinderList[sensortype];
  fCurrentChargeCalc = fChargeCalc[sensortype];
  fChargeCut = digipar->GetChargeCut();
  return;
}



// -------------------------------------------------------------------------
void PndMvdStripClusterTask::ResetClusterFinders()
{
  //recursively clean the digis in the clusterfinder objects each event
  for(std::map<const char*,PndMvdStripClusterBuilder*>::iterator CFiter = fClusterFinderList.begin();
      CFiter != fClusterFinderList.end(); CFiter++)
  {
    (CFiter->second)->ClearDigis();
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void PndMvdStripClusterTask::Finish()
{
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
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
		    fCurrentStripCalcTop->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
			if(fCurrentChargeCalc->GetCharge(*myDigi)<=0) Warning("CalcMeanCharge","Digi has a non-valid charge: c = %d electrons.",myDigi->GetCharge());
			charge += fCurrentChargeCalc->GetCharge(*myDigi);
			meanstrip += fCurrentChargeCalc->GetCharge(*myDigi) * strip;
			meanerr += fCurrentChargeCalc->GetCharge(*myDigi)*fCurrentChargeCalc->GetCharge(*myDigi);
		  }
			if (charge > 0)
			{
		    meanstrip = meanstrip/charge;
		    // this error treatment is: dx = dpitch * sqrt(weigthsquares)
		    meanerr = sqrt(meanerr/(charge*charge));
      } else {
        Error("CalcMeanCharge","No correct charge collected! C = %d electrons.");
        meanstrip=-1;
        meanerr=-1;
        return;
      }
		  if(side==kTOP){
		    meanerr = meanerr * (fCurrentDigiPar->GetTopPitch()/sqrt(12.));
		  }else{
		    meanerr = meanerr * (fCurrentDigiPar->GetBotPitch()/sqrt(12.));
		  }
	//} else {
	//	//TODO: Apply other clusterfinder mean & error algorithms
	//	if(fVerbose>1)std::cout<<"-W- PndMvdStripClusterTask::CalcMeanCharge: Using a preliminary Chargeweighting, please set fMeanAlgo = 0 ."<<std::endl;
	//	fCurrentChargeAlgos->center_of_gravity(onecluster);
	//}

}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
Bool_t PndMvdStripClusterTask::Backmap( TVector2 meantopPoint, Double_t meantoperr, TVector2 meanbotPoint, Double_t meanboterr,
		TVector3 &hitPos, TVector3 &hitErr, TString &detname)
{
  // BACKMAPPING
  // get the backmapped point

  TVector3 localpos, locDpos;
  Double_t t, b;
  Double_t errZ = fGeoH->GetSensorDimensionsId(detname).Z();

  TVector2 onsensorPoint = 
    CalcLineCross(meantopPoint, fCurrentStripCalcTop->GetStripDirection(), meanbotPoint, fCurrentStripCalcBot->GetStripDirection() );
  // here we assume the sensor system to be in the _Middle_ of the volume
  localpos.SetXYZ( onsensorPoint.X(), onsensorPoint.Y(), 0.);

  // let's see if we're still on the sensor (cut combinations with noise off)
  if(fabs(localpos.X()) > fabs(fCurrentDigiPar->GetTopAnchor().X())) 
  { 
    if(fVerbose>1) Warning("Backmap","Calculated hit out of sensor bounds: lpos_x=%d, anchor_x=%d",fabs(localpos.X()),fabs(fCurrentDigiPar->GetTopAnchor().X()));
    return kFALSE;
  }
  if(fabs(localpos.Y()) > fabs(fCurrentDigiPar->GetTopAnchor().Y()))
  { 
    if(fVerbose>1) Warning("Backmap","Calculated hit out of sensor bounds: lpos_y=%d, anchor_y=%d",fabs(localpos.X()),fabs(fCurrentDigiPar->GetTopAnchor().X()));
    return kFALSE;
  }
  
  //do the transformation from sensor to lab frame
  hitPos = fGeoH->LocalToMasterId(localpos,detname.Data());

  if (fVerbose>2)  cout<<" meantoperr="<<meantoperr<<endl;
  // calculate the errors corresponding to a skewed system!
  t = meantoperr*cos(fCurrentDigiPar->GetOrient());
  b = meanboterr*cos(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
  if (fVerbose>2)cout<<" t="<<t<<" b="<<b<<endl;

  locDpos.SetX( TMath::Sqrt( t*t + b*b ) );
  t = meantoperr*sin(fCurrentDigiPar->GetOrient());
  b = meanboterr*sin(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
  locDpos.SetY( TMath::Sqrt( t*t + b*b ) );
  locDpos.SetZ( errZ );
  if (fVerbose>2){cout << " errors:"<<endl;
  cout << "  dx = "<<locDpos.X()<<endl;
  cout << "  dy = "<<locDpos.Y()<<endl;
  cout << "  dz = "<<locDpos.Z()<<endl;
  }

  //do the transformation from sensor to lab frame
  hitErr = fGeoH->LocalToMasterErrorsId(locDpos,detname.Data());

  return kTRUE;
}
// -------------------------------------------------------------------------


ClassImp(PndMvdStripClusterTask);

