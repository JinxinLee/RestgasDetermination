// -------------------------------------------------------------------------
// -----                PndLmdStripClusterTask source file             -----
// -------------------------------------------------------------------------

//LUMI
#include "PndLmdStripClusterTask.h"
#include "PndLmdContFact.h"
//PANDA
#include "PndSdsTotChargeConversion.h"
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsChargeWeightingAlgorithms.h"
#include "PndSdsSimpleStripClusterFinder.h"
#include "PndSdsStripAdvClusterFinder.h"
#include "PndSdsTotDigiPar.h"
//FAIR
#include "FairRun.h"
#include "FairRuntimeDb.h"
//ROOT
#include "TList.h"

// -----   Default constructor   -------------------------------------------

PndLmdStripClusterTask::PndLmdStripClusterTask() :
  PndSdsStripClusterTask("LMD Strip Clusterisation Task")
{
/*  fChargeCut = 1.e8; // this ist really large and shall have no effect
  fGeoFile = "";
  fClusterMod=ClusterMod;
  if(fClusterMod>1 || fClusterMod<0) fClusterMod=0;
  fRadChannel=RadChannel;
  fRadTime=RadTime;*/
  // fyRotation = 2.326;
}


// -----   Destructor   ----------------------------------------------------
PndLmdStripClusterTask::~PndLmdStripClusterTask()
{
  //if(0!=fGeoH)  delete fGeoH;
  //if(0!=fChargeAlgos) delete fChargeAlgos;
}
// -------------------------------------------------------------------------

void PndLmdStripClusterTask::SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName)
{
  fInBranchName = inBranchname;
  fOutBranchName = outHitBranchname;
  fClustBranchName = outClustBranchname;
  fFolderName = folderName;
}

void PndLmdStripClusterTask::SetBranchNames()
{
  fInBranchName = "LMDStripDigis";
  fOutBranchName = "LMDHitsStrip";
  fClustBranchName = "LMDStripClusterCand";
  fFolderName = "PndLmd";
}



// -----   Initialization  of Parameter Containers -------------------------
void PndLmdStripClusterTask::SetParContainers()
{
  cout<<"PndLmdStripClusterTask::SetParContainers() !!!"<<endl;
  // called from the FairRun::Init()
  // Caution: The Parameter Set is not filled from the DB IO, yet. 
  // This will be done just before this Tasks Init() is called.
  FairRun* ana = FairRun::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  PndLmdContFact* themvdcontfact = (PndLmdContFact*)rtdb->getContFactory("PndLmdContFact");
  TList* theContNames = themvdcontfact->GetDigiParNames();
  Info("SetParContainers()","The container names list contains %i entries",theContNames->GetEntries());
  TIter cfIter(theContNames);
  while (TObjString* contname = (TObjString*)cfIter()) {
    TString parsetname = contname->String();
    Info("SetParContainers()",parsetname.Data());
    if(parsetname.BeginsWith("SDSStripDigiPar")){
      PndSdsStripDigiPar* digipar = (PndSdsStripDigiPar*)(rtdb->getContainer(parsetname.Data()));
      if ( ! digipar ) Fatal("SetParContainers","No DIGI parameter found: %s",parsetname.Data());
      fDigiParameterList->Add(digipar);
    }
    if(parsetname.BeginsWith("SDSStripTotDigiPar")){
      PndSdsTotDigiPar* totdigipar = (PndSdsTotDigiPar*)(rtdb->getContainer(parsetname.Data()));
      if ( ! totdigipar ) Fatal("SetParContainers","No TOT parameter found: %s",parsetname.Data());
      fChargeDigiParameterList->Add(totdigipar);
    }
  }//while
  PndSdsStripClusterTask::SetParContainers();
}

void PndLmdStripClusterTask::SetCalculators()
{
  std::cout<<"PndLmdStripClusterTask::SetCalculators() !!!"<<std::endl;
  Info("SetCalculators","lmd");
  PndSdsStripClusterTask::SetCalculators();
  TIter params(fDigiParameterList);
  TIter totparams(fChargeDigiParameterList);
  while( PndSdsStripDigiPar* digipar = (PndSdsStripDigiPar*)params() ){
    PndSdsTotDigiPar* totdigipar = (PndSdsTotDigiPar*) totparams();
    if ( 0==digipar ) continue;
    const char* senstype = digipar->GetSensType();
    if ( digipar->GetChargeConvMethod() == 1 ){
			if(fVerbose>0)	Info("SetCalculators()","Use Tot charge conversion for %s sensors",senstype);
			fChargeConverter[senstype] = new PndSdsTotChargeConversion(
										   totdigipar->GetChargingTime(),
                                                                 totdigipar->GetConstCurrent(),
										   digipar->GetThreshold(),
										   totdigipar->GetClockFrequency(),
										   fVerbose);
    }
    else{
      if(fVerbose>0)	Info("SetCalculators()","Use Ideal charge conversion for %s sensors",senstype);
      fChargeConverter[senstype] = new PndSdsIdealChargeConversion();
    }
    //fChargeAlgos[senstype] = new PndSdsChargeWeightingAlgorithms(fDigiArray, digipar->GetRaisingTime(),digipar->GetFallingRatio(),digipar->GetThreshold());
    Int_t ClusterMod = digipar->GetClusterMod(); 
    Int_t RadChannel = digipar->GetRadChannel(); 
    Int_t RadTime    = digipar->GetRadTime(); 
    if(0==ClusterMod) { 
      fClusterFinderList[senstype] = new PndSdsSimpleStripClusterFinder(fInBranchId, RadChannel ); //search radius in channel no. 
    } else if(1==ClusterMod) { 
      fClusterFinderList[senstype] = new PndSdsStripAdvClusterFinder(fInBranchId, RadChannel, RadTime); 
    } 
	}
}

TVector3 PndLmdStripClusterTask::AddMSErr(TVector3 hpos, TVector3 hposerr){
  //  return hposerr;
  double xerr,yerr;
  double zhit = hpos.Z();
  cout<<"0 xerr = "<<hposerr.X()<<" yerr = "<<hposerr.Y()<<endl;
  if(zhit<1100.) return hposerr;
  if(zhit>1100.){
    xerr = TMath::Hypot(hposerr.X(),0.002);
    yerr = TMath::Hypot(hposerr.Y(),0.002);
    cout<<"1 xerr = "<<xerr<<" yerr = "<<yerr<<endl;
    if(zhit>1110.){
      xerr = TMath::Hypot(xerr,0.002);
      yerr = TMath::Hypot(yerr,0.002);
      cout<<"2 xerr = "<<xerr<<" yerr = "<<yerr<<endl;
      if(zhit>1120.){
	xerr = TMath::Hypot(xerr,0.002);
	yerr = TMath::Hypot(yerr,0.002);
	cout<<"3 xerr = "<<xerr<<" yerr = "<<yerr<<endl;
      }
      if(zhit>1130.){
	cout<<"Something wrong! in PndLmdStripClusterTask::AddMSErr"<<endl;
      }
    }
  }
  TVector3 res(xerr,yerr,hposerr.Z());
  return res;
};

Bool_t PndLmdStripClusterTask::Backmap( TVector2 meantopPoint, Double_t meantoperr, TVector2 meanbotPoint, Double_t meanboterr,
                                       TVector3 &hitPos, TMatrixD &hitCov, Int_t &sensorID)
{
  // BACKMAPPING
  // get the backmapped point
  
  TVector3 localpos;
  TMatrixD locCov(3,3);
  Double_t t, b;
  
  Double_t errZ = 2.*fGeoH->GetSensorDimensionsShortId(sensorID).Z()/TMath::Sqrt(12.0);
  
  TVector2 onsensorPoint = 
  CalcLineCross(meantopPoint, fCurrentStripCalcTop->GetStripDirection(), meanbotPoint, fCurrentStripCalcBot->GetStripDirection() );
  // here we assume the sensor system to be in the _Middle_ of the volume
  localpos.SetXYZ( onsensorPoint.X(), onsensorPoint.Y(), 0.);
  
  // let's see if we're still on the sensor (cut combinations with noise off)
  if(fabs(localpos.X()) > fabs(fCurrentDigiPar->GetTopAnchor().X())) return kFALSE;
  if(fabs(localpos.Y()) > fabs(fCurrentDigiPar->GetTopAnchor().Y())) return kFALSE;
  
  //do the transformation from sensor to lab frame
  hitPos = fGeoH->LocalToMasterShortId(localpos,sensorID);
  
  // calculate the errors corresponding to a skewed system!
  t = meantoperr*fCurrentDigiPar->GetTopPitch()*cos(fCurrentDigiPar->GetOrient());
  b = meanboterr*fCurrentDigiPar->GetBotPitch()*cos(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
  locCov[0][0]=t*t+b*b;
  t = meantoperr*fCurrentDigiPar->GetTopPitch()*sin(fCurrentDigiPar->GetOrient());
  b = meanboterr*fCurrentDigiPar->GetBotPitch()*sin(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
  locCov[1][1]=t*t+b*b;
  locCov[2][2]=errZ*errZ;
  
  //do the transformation from sensor to lab frame
  hitCov = fGeoH->LocalToMasterErrorsShortId(locCov,sensorID);
  // TVector3 hitErr(sqrt(locCov[0][0]),sqrt(locCov[1][1]),sqrt(locCov[2][2]));
  // TVector3 hitErrMSadd = AddMSErr(hitPos, hitErr);
  // locCov[0][0] = TMath::Power(hitErrMSadd.X(),2);
  // locCov[1][1] = TMath::Power(hitErrMSadd.Y(),2);
  // locCov[2][2] = TMath::Power(hitErrMSadd.Z(),2);
  return kTRUE;
}


// Bool_t PndLmdStripClusterTask::Backmap( TVector2 meantopPoint, Double_t meantoperr, TVector2 meanbotPoint, 
// 					Double_t meanboterr,
// 					TVector3 &hitPos, TVector3 &hitErr, TString &detname)
// {
//   // BACKMAPPING
//   // get the backmapped point
//   TVector3 localpos, locDpos;
//   Double_t t, b;
//   Double_t errZ = 2.*fGeoH->GetSensorDimensionsShortId(detname).Z()/TMath::Sqrt(12.0);

//   TVector2 onsensorPoint =
//     CalcLineCross(meantopPoint, fCurrentStripCalcTop->GetStripDirection(), meanbotPoint, fCurrentStripCalcBot->GetStripDirection() );
//   // here we assume the sensor system to be in the _Middle_ of the volume


//   // if(fyRotation==0.){  						//TODO: make this generaly
//   //   localpos.SetXYZ( onsensorPoint.X(), onsensorPoint.Y(), 0.);
//   // }else{
//   //   localpos.SetXYZ( onsensorPoint.X(), onsensorPoint.Y(), onsensorPoint.X()*tan(fyRotation*TMath::Pi()/180.));
//   // }

//   //local system already rotated!
//   localpos.SetXYZ( onsensorPoint.X(), onsensorPoint.Y(), 0.);
  
//   // let's see if we're still on the sensor (cut combinations with noise off)
//   if(fabs(localpos.X()) > fabs(fCurrentDigiPar->GetTopAnchor().X())) return kFALSE;
//   if(fabs(localpos.Y()) > fabs(fCurrentDigiPar->GetTopAnchor().Y())) return kFALSE;
	
//   //do the transformation from sensor to lab frame
//   hitPos = fGeoH->LocalToMasterId(localpos,detname.Data());
 	
//   // calculate the errors corresponding to a skewed system!
//   t = meantoperr*fCurrentDigiPar->GetTopPitch()*cos(fCurrentDigiPar->GetOrient());
//   b = meanboterr*fCurrentDigiPar->GetBotPitch()*cos(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
//   locDpos.SetX( sqrt(t*t+b*b) );
//   t = meantoperr*fCurrentDigiPar->GetTopPitch()*sin(fCurrentDigiPar->GetOrient());
//   b = meanboterr*fCurrentDigiPar->GetBotPitch()*sin(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
//   locDpos.SetY( sqrt(t*t+b*b) );
//   locDpos.SetZ( errZ );
//   // cout<<"@@@@@ PndLmdStripClusterTask::Backmap (ErrX, ErrY, ErrZ)loc = "
//   //     <<sqrt(t*t+b*b)<<", "<<sqrt(t*t+b*b)<<", "<<errZ<<endl;
 	 
//   //do the transformation from sensor to lab frame
//   hitErr = fGeoH->LocalToMasterErrorsId(locDpos,detname.Data());
//   TVector3 hitErrMSadd = AddMSErr(hitPos, hitErr);
//   hitErr = hitErrMSadd;
//   return kTRUE;
// }

ClassImp(PndLmdStripClusterTask);

