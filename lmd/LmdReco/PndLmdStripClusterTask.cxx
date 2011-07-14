// -------------------------------------------------------------------------
// -----                PndLmdStripClusterTask source file             -----
// -----           modified for Lmd by M. Michel & A.Karavdina         -----
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
#include "FairBaseParSet.h"
//ROOT
#include "TList.h"
#include "TDatabasePDG.h"
#include "TLorentzVector.h"
// -----   Default constructor   -------------------------------------------

PndLmdStripClusterTask::PndLmdStripClusterTask() :
  PndSdsStripClusterTask("LMD Strip Clusterisation Task")
{
  fGeoH = PndGeoHandling::Instance();
}

// -----   Public method Init   --------------------------------------------
/*InitStatus PndLmdStripClusterTask::Init()
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
  fDigiArray = (TClonesArray*) ioman->GetObject(fInBranchName);
  if ( ! fDigiArray )
  {
    std::cout << "-W- PndSdsStripClusterTask::Init: "
    << "No SDSDigi array!" << std::endl;
    return kERROR;
  }
  
  // set output arrays
  
  fClusterArray = new TClonesArray("PndSdsClusterStrip");
  ioman->Register(fClustBranchName, fFolderName, fClusterArray, fPersistance);
  
  fHitArray = new TClonesArray("PndSdsHit");
  ioman->Register(fOutBranchName, fFolderName, fHitArray, fPersistance);

  SetInBranchId();

  SetCalculators();
  
  //  FairRun* 
  ana = FairRun::Instance();
  //  FairRuntimeDb* 
  rtdb = ana->GetRuntimeDb();
  FairBaseParSet* par=(FairBaseParSet*)
    (rtdb->findContainer("FairBaseParSet"));
  //  cout<<"par = "<<par<<endl;
  fPbeam = par->GetBeamMom();
  //  cout<<"PndLmdStripClusterTask::Init() fPbeam = "<<fPbeam<<endl;
  Info("Init","Initialisation successfull");
  return kSUCCESS;
  }*/

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
  // called from the FairRun::Init()
  // Caution: The Parameter Set is not filled from the DB IO, yet. 
  // This will be done just before this Tasks Init() is called.
  //FairRun* ana = FairRun::Instance();
  //  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

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
  if(fVerbose>0) Info("AddMSErr","calculation additional errors due to multiple scaterring");

  //Calculation of ThetaMS -------------------------------------
  //Charge & mass of particle
  Int_t PDGCode = -2212;
  TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
  TParticlePDG *fParticle = fdbPDG->GetParticle(PDGCode);
  // Double_t  fCharge = fParticle->Charge();
  Double_t fMass = fParticle->Mass();

  Double_t Ebeam = TMath::Hypot(fPbeam,fMass);
  TLorentzVector LorMom(0, 0, fPbeam, Ebeam);
  Double_t beta = LorMom.Beta();
  //  cout<<"beta = "<<beta<<" for fPbeam="<<fPbeam<<" GeV/c"<<endl;
  Double_t X = 0.015;
  //  Double_t X = 0.0012; //TEST
  // Double_t X = 0.03; //TEST
  Double_t X0 = 9.36;
  Double_t thetaMS = 13.6*1e-3*TMath::Sqrt(X/X0)*(1+0.038*TMath::Log(X/X0))/(beta*fPbeam);
  //  Double_t thetaMS = 13.6*1e-3*TMath::Power(X/X0,0.555)/(beta*fPbeam);
  // cout<<"thetaMS [OLD] = "<<thetaMSold<<endl;
  // Double_t thetaMSnew = 13.6*1e-3*TMath::Sqrt(X/X0)/(beta*fPbeam); //TEST!!!
  // cout<<"thetaMS [NEW] = "<<thetaMSnew<<endl;
  // Double_t thetaMS = thetaMSnew;
  //-----------------------------------------------------------

  //TO DO: use parameters from geometry info for LUMI
  Double_t d = 10; 

  double xerr,yerr;
  double zhit = hpos.Z();
  //  const double Z0 = 1100.;
  const double Z0 = 1099.;
  int num = (zhit-Z0)/d;
  //  double numd = (zhit-Z0)/10.;
  //  cout<<"num = "<<num<<" num(double) = "<<numd<<endl;
  xerr = hposerr.X();
  yerr = hposerr.Y();
  // cout<<"Plane #"<<num<<" before: zhit="<<zhit<<" xerr = "<<xerr<<" yerr = "<<yerr<<endl;
  Double_t sigmaMSplane = X*thetaMS; //TEST
  //  cout<<"sigmaMSplane = "<<sigmaMSplane<<" um"<<endl;
  if(num==0){
    xerr = TMath::Hypot(xerr,sigmaMSplane);
    yerr = TMath::Hypot(yerr,sigmaMSplane);
  }
  double xhit = hpos.X();
 
  //  Double_t l = 10./cos(2.326*TMath::Pi()/180.);
  double sigmaMS;
  for(int j=0;j<num;j++){
    sigmaMS = (j+1)*d*thetaMS;
    //    cout<<"sigmaMS = "<<sigmaMS<<" xerr="<<xerr<<" yerr="<<yerr<<endl;
    //sigmaMS = j*d*thetaMS;
    xerr = TMath::Hypot(xerr,sigmaMS);
    yerr = TMath::Hypot(yerr,sigmaMS);  

  }
 
  //  cout<<"Plane #"<<num<<" after: zhit="<<zhit<<" xerr = "<<xerr<<" yerr = "<<yerr<<endl;

  // /// only for test -----
  // double sigma_add = 1e-4*(7.083-num*d*0.6238);
  // xerr = TMath::Hypot(xerr,sigma_add);
  // yerr = TMath::Hypot(yerr,sigma_add);
  // ///--------------------

  //cout<<"Plane #"<<num<<" zhit="<<zhit<<" xerr = "<<xerr<<" yerr = "<<yerr<<" sigma_add = "<<sigma_add<<endl;
  //  cout<<"Plane #"<<num<<" after: zhit="<<zhit<<" xerr = "<<xerr<<" yerr = "<<yerr<<endl;
  // if(zhit<Z0){
  //   cout<<"0 zhit="<<zhit<<" xerr = "<<hposerr.X()<<" yerr = "<<hposerr.Y()<<endl;
  //   return hposerr;
  // }
  // if(zhit>Z0){
  //   Double_t sigma1 = d*thetaMS;
  //   xerr = TMath::Hypot(hposerr.X(),sigma1);
  //   yerr = TMath::Hypot(hposerr.Y(),sigma1);
  //   cout<<"1 zhit="<<zhit<<" xerr = "<<xerr<<" yerr = "<<yerr<<" sigma1 = "<<sigma1<<endl;
  //   if(zhit>Z0+d){
  //     Double_t sigma2 = 2*d*thetaMS;
  //     xerr = TMath::Hypot(xerr,sigma2);
  //     yerr = TMath::Hypot(yerr,sigma2);
  //     cout<<"2 zhit="<<zhit<<" xerr = "<<xerr<<" yerr = "<<yerr<<" sigma2 = "<<sigma2<<endl;
  //     if(zhit>Z0+2*d){
  // 	Double_t sigma3 = 3*d*thetaMS;
  // 	xerr = TMath::Hypot(xerr,sigma3);
  // 	yerr = TMath::Hypot(yerr,sigma3);
  // 	cout<<"3 zhit="<<zhit<<" xerr = "<<xerr<<" yerr = "<<yerr<<" sigma3 = "<<sigma3<<endl;
  //     }
  //     if(zhit>Z0+3*d){
  // 	cout<<"Something wrong! in PndLmdStripClusterTask::AddMSErr"<<endl;
  //     }
  //   }
  // }
  TVector3 res(xerr,yerr,hposerr.Z());
  return res;
};

Bool_t PndLmdStripClusterTask::Backmap( TVector2 meantopPoint, Double_t meantoperr, TVector2 meanbotPoint, Double_t meanboterr,
                                       TVector3 &hitPos, TMatrixD &hitCov, Int_t &sensorID)
{
  // BACKMAPPING
  // get the backmapped point
  //  cout<<"PndLmdStripClusterTask::BACKMAP"<<endl;
  //Info("Backmap","Sensor ID is %s",sensorID);
  TVector3 localpos;
  TMatrixD locCov(3,3);
  Double_t t, b;
  //  cout<<"sensorID = "<<sensorID<<endl;
  //  cout<<"fGeoH = "<<fGeoH<<endl;
  Double_t errZ = 2.*fGeoH->GetSensorDimensionsShortId(sensorID).Z()/TMath::Sqrt(12.0);
  // cout<<"fGeoH->GetSensorDimensionsShortId(sensorID).Z() = "<<fGeoH->GetSensorDimensionsShortId(sensorID).Z()<<endl;
  //  Double_t errZ = fGeoH->GetSensorDimensionsShortId(sensorID).Z()/TMath::Sqrt(12.0);//TEST!!!
  // cout<<"errZ = "<<errZ<<endl;

  TVector2 onsensorPoint = 
  CalcLineCross(meantopPoint, fCurrentStripCalcTop->GetStripDirection(), meanbotPoint, fCurrentStripCalcBot->GetStripDirection() );
  // here we assume the sensor system to be in the _Middle_ of the volume
  localpos.SetXYZ( onsensorPoint.X(), onsensorPoint.Y(), 0.);
  //double zloc = fGeoH->GetSensorDimensionsShortId(sensorID).Z()+onsensorPoint.X()*tan(2.326*TMath::Pi()/180.);//TEST
  //double zloc = (-1)*fGeoH->GetSensorDimensionsShortId(sensorID).Z();//TEST
  //  localpos.SetXYZ( onsensorPoint.X(), onsensorPoint.Y(), zloc);//TEST
  // cout<<"onsensorPoint.X()*tan(2.326*TMath::Pi()/180.) = "<<onsensorPoint.X()*tan(2.326*TMath::Pi()/180.)
  //     <<" onsensorPoint.X() = "<<onsensorPoint.X()<<endl;
  // errZ +=fabs(onsensorPoint.X())*tan(2.326*TMath::Pi()/180.); //TEST
  //  cout<<"onsensorPoint.X() = "<<onsensorPoint.X()<<" errZ = "<<errZ<<endl;
  // let's see if we're still on the sensor (cut combinations with noise off)
  if(fabs(localpos.X()) > fabs(fCurrentDigiPar->GetTopAnchor().X())) return kFALSE;
  if(fabs(localpos.Y()) > fabs(fCurrentDigiPar->GetTopAnchor().Y())) return kFALSE;
  
  //do the transformation from sensor to lab frame
  hitPos = fGeoH->LocalToMasterShortId(localpos,sensorID);
  
  // calculate the errors corresponding to a skewed system!
  //  cout<<"PndLmdStripClusterTask::Backmap meantoperr="<<meantoperr<<endl;
  t = meantoperr*fCurrentDigiPar->GetTopPitch()*cos(fCurrentDigiPar->GetOrient());
  b = meanboterr*fCurrentDigiPar->GetBotPitch()*cos(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
  // t = meantoperr*fCurrentDigiPar->GetTopPitch()*cos(0.5*TMath::Pi()-fCurrentDigiPar->GetOrient());
  // b = meanboterr*fCurrentDigiPar->GetBotPitch()*cos(0.5*TMath::Pi()-fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());//TEST
    
  //  cout<<"t = "<<t<<" b = "<<b<<" meantoperr = "<<meantoperr<<" meanboterr = "<<meanboterr<<endl;
  //  locCov[0][0]=t*t+b*b;
  locCov[0][0]=t*t+b*b+2*fabs(t*b*cos(fCurrentDigiPar->GetSkew())); //TEST
  t = meantoperr*fCurrentDigiPar->GetTopPitch()*sin(fCurrentDigiPar->GetOrient());
  b = meanboterr*fCurrentDigiPar->GetBotPitch()*sin(fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
  // t = meantoperr*fCurrentDigiPar->GetTopPitch()*sin(0.5*TMath::Pi()-fCurrentDigiPar->GetOrient());
  // b = meanboterr*fCurrentDigiPar->GetBotPitch()*sin(0.5*TMath::Pi()-fCurrentDigiPar->GetOrient()+fCurrentDigiPar->GetSkew());
  //  locCov[1][1]= locCov[0][0];
  locCov[1][1]=t*t+b*b+2*fabs(t*b*cos(fCurrentDigiPar->GetSkew())); //TEST
  locCov[2][2]=errZ*errZ;
  //  cout<<"meantoperr="<<meantoperr<<" fCurrentDigiPar->GetOrient()="<<fCurrentDigiPar->GetOrient()<<" fCurrentDigiPar->GetSkew()="<<fCurrentDigiPar->GetSkew()<<endl;
  //  cout<<"t = "<<t<<" b = "<<b<<" meantoperr = "<<meantoperr<<" meanboterr = "<<meanboterr<<endl;
  //do the transformation from sensor to lab frame

 
  TVector3 hitErr(sqrt(locCov[0][0]),sqrt(locCov[1][1]),sqrt(locCov[2][2]));
  TVector3 hitErrMSadd = AddMSErr(hitPos, hitErr);
  locCov[0][0] = TMath::Power(hitErrMSadd.X(),2);
  locCov[1][1] = TMath::Power(hitErrMSadd.Y(),2);
  locCov[2][2] = TMath::Power(hitErrMSadd.Z(),2);

  hitCov = fGeoH->LocalToMasterErrorsShortId(locCov,sensorID);
  // cout<<"sqrt(locCov[0][0]) = "<<sqrt(locCov[0][0])<<" sqrt(locCov[1][1]) = "<<sqrt(locCov[1][1])
  //     <<" sqrt(locCov[2][2]) = "<<sqrt(locCov[2][2])<<endl;
  // cout<<"sqrt(hitCov[0][0]) = "<<sqrt(hitCov[0][0])<<" sqrt(hitCov[1][1]) = "<<sqrt(hitCov[1][1])
  //     <<" sqrt(hitCov[2][2]) = "<<sqrt(hitCov[2][2])<<endl;


  // TVector3 hitErr(sqrt(hitCov[0][0]),sqrt(hitCov[1][1]),sqrt(hitCov[2][2]));
  // TVector3 hitErrMSadd = AddMSErr(hitPos, hitErr);
  // hitCov[0][0] = TMath::Power(hitErrMSadd.X(),2);
  // hitCov[1][1] = TMath::Power(hitErrMSadd.Y(),2);
  // hitCov[2][2] = TMath::Power(hitErrMSadd.Z(),2);
  // // cout<<"GLOBAL hitPos:"<<endl;
  // // hitPos.Print();
  
  // // cout<<" sqrt(hitCov[0][0]) = "<<sqrt(hitCov[0][0])<<" sqrt(hitCov[1][1]) = "<<sqrt(hitCov[1][1])
  // //     <<" sqrt(hitCov[2][2]) = "<<sqrt(hitCov[2][2])<<endl;

  return kTRUE;
}

ClassImp(PndLmdStripClusterTask);

