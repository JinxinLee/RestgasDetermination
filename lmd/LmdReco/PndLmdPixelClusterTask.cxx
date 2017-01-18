// -------------------------------------------------------------------------
// -----                PndLmdPixelClusterTasksource file             -----
// -------------------------------------------------------------------------


#include "PndLmdPixelClusterTask.h"
#include "PndSdsPixelDigiPar.h"
#include "PndLmdContFact.h"
#include "PndLmdAlignPar.h"
#include "PndLmdDim.h"
#include "TLorentzVector.h"
//FAIR
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairRunAna.h"
// -----   Default constructor   -------------------------------------------
PndLmdPixelClusterTask::PndLmdPixelClusterTask() :
PndSdsPixelClusterTask("LMD Clustertisation Task")
{
  fPersistance = kTRUE;
  fAlignParamList = new TList();
  readAlign = true;
  //flagMS = true;
  //  mtxpath = "../../../pandaroot/input/";
  //  mtxpath = "";
  mtxpath = "../../geometry/";
  lmddim = NULL;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndLmdPixelClusterTask::~PndLmdPixelClusterTask()
{
  if (fBackMapping != 0) delete fBackMapping;
  if (fClusterFinder != 0) delete fClusterFinder;
  if (fFunctor != 0) delete fFunctor;
  if(fAlignParamList !=0) delete fAlignParamList;

  if(fStartFunctor !=0) delete fStartFunctor;
  if(fGeoH!=0) delete fGeoH;
  if(lmddim!=0 ) lmddim->Cleanup();
}
// -------------------------------------------------------------------------

InitStatus PndLmdPixelClusterTask::Init()
{
  FairBaseParSet* par=(FairBaseParSet*)(rtdb->findContainer("FairBaseParSet"));
  fPbeam = par->GetBeamMom();

  SetBranchNames();

  SetBackMapping();
  SetClusterFinder();
  
  FairRootManager* ioman = FairRootManager::Instance();
  
  if ( ! ioman )
  {
    std::cout << "-E- PndSdsPixelClusterTask::Init: "
    << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  

  // Get input array
  fDigiArray = (TClonesArray*) ioman->GetObject(fInBranchName);
//
  if ( ! fDigiArray )
  {
    std::cout << "-E- PndSdsPixelClusterTask::Init: "
    << "No SDSDigi array!" << std::endl;
    return kERROR;
  }
  
  fClusterArray = ioman->Register(fClustBranchName, "PndSdsClusterPixel", fFolderName, fPersistance);

  fHitArray =  ioman->Register(fOutBranchName, "PndSdsHit", fFolderName, fPersistance);
  
  SetInBranchId();

  fFunctor = new TimeGap();
  fStartFunctor = new StopTime();
  
  if(fVerbose>1) fDigiPar->Print();

  if(lmddim!=0 ) lmddim->Cleanup();
  lmddim = PndLmdDim::Instance();
  if(readAlign) SetAlignConst();

  fGeoH->SetVerbose(fVerbose);

  std::cout << "-I- PndSdsPixelClusterTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Initialization  of Parameter Containers -------------------------
void PndLmdPixelClusterTask::SetParContainers()
{
  std::cout<<"PndLmdPixelClusterTask::SetParContainers() "<<std::endl;
  // Get Base Container
	ana = FairRun::Instance();
	rtdb=ana->GetRuntimeDb();
	fDigiPar = (PndSdsPixelDigiPar*)(rtdb->getContainer("SDSPixelDigiPar"));
	//	rtdb->getContainer("SDSPixelTotDigiPar");
	
	PndLmdContFact* themvdcontfact = (PndLmdContFact*)rtdb->getContFactory("PndLmdContFact");
	//read params for lumi alignment
	TList* theAlignLMDContNames = themvdcontfact->GetAlignParNames();
	Info("SetParContainers()","AlignLMD The container names list contains %i entries",theAlignLMDContNames->GetEntries());
	TIter cfAlIter(theAlignLMDContNames);
	while (TObjString* contname = (TObjString*)cfAlIter()) {
	  TString parsetname = contname->String();
	  Info("SetParContainers()","%s",parsetname.Data());
	  PndLmdAlignPar *lmdalignpar = (PndLmdAlignPar*)(rtdb->getContainer(parsetname.Data()));
	  if(!lmdalignpar) Fatal("SetParContainers","No ALIGN parameter found: %s",parsetname.Data());
	  fAlignParamList->Add(lmdalignpar); 
	}
	// //
	PndSdsPixelClusterTask::SetParContainers();
}

void PndLmdPixelClusterTask::SetAlignConst(){
  std::cout<<"PndLmdPixelClusterTask::SetAlignConst() "<<std::endl;

  TIter alignparams(fAlignParamList); 
  PndLmdAlignPar* lmdalignpar=(PndLmdAlignPar*)alignparams();
  lmddim -> Read_DB_offsets(lmdalignpar);
  //  TString mtx_perfect =  "matrices_perfect.txt";
  TString mtx_perfect =   mtxpath+"trafo_matrices_lmd.dat";
  TString mtx_corr =   mtxpath+"matrices_corrected.txt";
  lmddim -> Read_transformation_matrices(mtx_perfect.Data(), false);
  lmddim -> reCreate_transformation_matrices();
  lmddim -> Write_transformation_matrices(mtx_corr.Data(), false);
  lmddim -> Read_transformation_matrices(mtx_perfect.Data(), false);
  lmddim -> Read_transformation_matrices(mtx_corr.Data(), true);
}
void PndLmdPixelClusterTask::SetBackMapping()
{
  // at Init() stage we already have fGeoH filled with the parameters
	fBackMapping = new PndLmdChargeWeightedPixelMapping(fGeoH, fVerbose);
}

void PndLmdPixelClusterTask::SetClusterFinder()
{
  std::cout<<"PndLmdPixelClusterTask::SetClusterFinder() "<<std::endl;
	fClusterFinder = new PndLmdSimplePixelClusterFinder(fVerbose);
}

// -----   Manula I/O folders/branches   ----------------------------------------------------
void PndLmdPixelClusterTask::SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName)
{
  fInBranchName = inBranchname;
  fOutBranchName = outHitBranchname;
  fClustBranchName = outClustBranchname;
  fFolderName = folderName;
}

// -----   Default I/O folder/branches   ----------------------------------------------------
void PndLmdPixelClusterTask::SetBranchNames()
{
	if (FairRunAna::Instance()->IsTimeStamp())
		fInBranchName = "LMDSortedPixelDigis";
	else
		fInBranchName = "LMDPixelDigis";
  fOutBranchName = "LMDHitsPixel";
  fClustBranchName = "LMDPixelClusterCand";
  fFolderName = "cbmsim";
}

TVector3 PndLmdPixelClusterTask::AddMSErr(TVector3 hpos, TVector3 hposerr){
  if(fVerbose>0) Info("PndLmdPixelClusterTask::AddMSErr","calculation additional errors due to multiple scaterring");

  //Calculation of ThetaMS -------------------------------------
  //Charge & mass of particle
  //Int_t PDGCode = -2212; //[R.K. 01/2017] unused variable
  Double_t fMass = 0.938272046;
  Double_t Ebeam = TMath::Hypot(fPbeam,fMass);
  TLorentzVector LorMom(0, 0, fPbeam, Ebeam);
  Double_t beta = LorMom.Beta();
  Double_t X_to_X0 = 0.00306;//for one plane: flexcable+(HV-MAPS)+cooling disk+(HV-MAPS)+flexcable
    X_to_X0 -= 2*0.00053;//-(HV-MAPS)
   // Double_t thetaMS = 13.6*1e-3*TMath::Sqrt(X_to_X0)/(beta*fPbeam);
   Double_t thetaMS = 13.6*1e-3*TMath::Sqrt(X_to_X0)*(1+0.038*TMath::Log(X_to_X0))/(beta*fPbeam);
   // cout<<"thetaMS = "<<thetaMS<<endl;
  //-----------------------------------------------------------

  //TO DO: use parameters from geometry info for LUMI
  Double_t d1 = 15.; 
  Double_t d2 = 10.; 
  double xerr,yerr;
  xerr = hposerr.X();
  yerr = hposerr.Y();
  double zhit = hpos.Z();
  int num = 0;
  if(zhit>1140 && zhit<1150) num=1;
  if(zhit>1150 && zhit<1160) num=2;
  if(zhit>1160 && zhit<1170) num=3;

  
  //cout<<"Plane #"<<num<<" before: zhit="<<zhit<<" xerr = "<<xerr<<" yerr = "<<yerr<<endl;
 
  double sigmaMS;
  for(int j=0;j<num;j++){
    // sigmaMS = 2*(j+1)*d*thetaMS;
    double d=d2;
    if(j==0) d=d1;
    sigmaMS = (j+1)*d*thetaMS;
    xerr = TMath::Hypot(xerr,sigmaMS);
    yerr = TMath::Hypot(yerr,sigmaMS); 
    //  cout<<"num:"<<num<<" j="<<j<<" d = "<<d<<endl;
  }
  //  cout<<" num:"<<num<<"(Z="<<zhit<<") xerr="<<xerr<<" yerr="<<yerr<<endl;
  TVector3 res(xerr,yerr,hposerr.Z());
  return res;
};

// -----   Public method Exec   --------------------------------------------
void PndLmdPixelClusterTask::Exec(Option_t* opt)
{
  // std::cout<<" fEventNr = "<< fEventNr<<std::endl;
  // Reset output array
  fClusterArray = FairRootManager::Instance()->GetTClonesArray(fClustBranchName);
  if ( ! fClusterArray ) Fatal("Exec", "No ClusterArray");
  fClusterArray->Delete();

  fHitArray = FairRootManager::Instance()->GetTClonesArray(fOutBranchName);
  if ( ! fHitArray ) Fatal("Exec", "No HitArray");
  fHitArray->Delete();

  std::vector<PndSdsDigiPixel> DigiPixelArray;
  DigiPixelArray.clear();

  Double_t EventTime = FairRootManager::Instance()->GetEventTime();

  if(fVerbose>0) std::cout << "-I- PndSdsPixelClusterTask::Exec EventTime: " << EventTime << std::endl;

  // Get input array
  if (FairRunAna::Instance()->IsTimeStamp()){
    fDigiArray->Clear();
    fDigiArray = FairRootManager::Instance()->GetData(fInBranchName, fStartFunctor, EventTime + 10);
  }
  else
    fDigiArray = (TClonesArray*)FairRootManager::Instance()->GetObject(fInBranchName);

  Int_t nPoints = fDigiArray->GetEntriesFast();

  // convert from TClonesArray to a std::vector
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++){
    PndSdsDigiPixel myDigi = *(PndSdsDigiPixel*)(fDigiArray->At(iPoint));
    DigiPixelArray.push_back(myDigi);
    myDigi.Clear();
  }
  // Retrieve the calculated clusters with the chosen clusterfinder
  std::vector< std::vector< Int_t> > clusters = fClusterFinder->GetClusters(DigiPixelArray);
  if(fVerbose>1) std::cout << " -I-  PndSdsPixelClusterTask::Exec(): We have "<<clusters.size()<<" pixel clusters" << std::endl;
  // store the list
  for (UInt_t i = 0; i < clusters.size(); i++)
    {
      PndSdsClusterPixel* tempCluster = new((*fClusterArray)[i]) PndSdsClusterPixel(fInBranchId, clusters[i]);
      if (FairRunAna::Instance()->IsTimeStamp()){
	tempCluster->ResetLinks();
	for (UInt_t j = 0; j < clusters[i].size(); j++){
	  PndSdsDigiPixel* tempDigi = (PndSdsDigiPixel*)fDigiArray->At(clusters[i][j]);
	  tempCluster->AddLink(FairLink(tempDigi->GetEntryNr()));
	  tempDigi->Clear();
	}
      }
    }
  
  // do the backmapping with charge-weight
  for (UInt_t i = 0; i < clusters.size(); i++)
    {
      //    if(fVerbose>2) std::cout << clusters[i].size() << " " << std::endl;
      std::vector<PndSdsDigiPixel> clusterArray;
      for (UInt_t j=0;j < clusters[i].size();j++)
	{ // convert
	  clusterArray.push_back(DigiPixelArray[clusters[i][j]]);
	}
    
      // mapping with the choosen back mapping
      //Int_t iHits = fHitArray->GetEntriesFast(); //[R.K. 01/2017] unused variable
      //      PndSdsHit *myHit = new((*fHitArray)[iHits]) PndSdsHit(fBackMapping->GetCluster(clusterArray));
      PndSdsHit myHit = fBackMapping->GetCluster(clusterArray);
      clusterArray.clear();
      myHit.SetClusterIndex(fClusterType,i, 0, fEventNr);
      TVector3 hitPos = myHit.GetPosition();
      TMatrixD hitCov = myHit.GetCov();
      // hitCov(0,0) = 5.29e-06; //assuming hit resolution for x-y 23 mkm
      // hitCov(1,1) = 5.29e-06; //assuming hit resolution for x-y 23 mkm
      hitCov(0,0) = 2.25e-06; //assuming hit resolution for x-y 15 mkm
      hitCov(1,1) = 2.25e-06; //assuming hit resolution for x-y 15 mkm
      hitCov(2,2) = 4.28489999999999954e-08; //assuming hit resolution for z 2.07 mkm NB: isn't used in Trk rec.

      // //Add multiple scattering error ---------------
      // if(flagMS){
      // 	//	TVector3 hitPos = myHit.GetPosition();
      // 	TVector3 hitErr(sqrt(hitCov[0][0]),sqrt(hitCov[1][1]),sqrt(hitCov[2][2]));
      // 	TVector3 hitErrMSadd = AddMSErr(hitPos, hitErr);
      // 	hitCov[0][0] = TMath::Power(hitErrMSadd.X(),2);
      // 	hitCov[1][1] = TMath::Power(hitErrMSadd.Y(),2);
      // 	hitCov[2][2] = TMath::Power(hitErrMSadd.Z(),2);
      // }
      // cout<<"!!!!!!!!!"<<endl;
      // hitCov.Print();
      // cout<<"!!!!!!!!!"<<endl;
      myHit.SetCov(hitCov);//save value
      myHit.SetDx(sqrt(hitCov[0][0]));
      myHit.SetDy(sqrt(hitCov[1][1]));
      myHit.SetDz(sqrt(hitCov[2][2]));


      // //Alignment: translate to LMD local (not corrected) frame and back to GLOBAL (corrected) ----------------
      if(readAlign){
	if(fVerbose>0){
	  cout<<"BEFORE [mis]ALIGN:"<<endl;
	  myHit.Print();
	}
	int sensorID = myHit.GetSensorID();
	int ihalf, iplane, imodule, iside, idie, isensor;
	lmddim->Get_sensor_by_id(sensorID, ihalf, iplane, imodule, iside, idie, isensor);
	TVector3 hitPos1(lmddim->Transform_global_to_sensor(hitPos,ihalf,iplane,imodule,iside,idie,isensor,false,false));
	TVector3 hitPos2(lmddim->Transform_sensor_to_global(hitPos1,ihalf,iplane,imodule,iside,idie,isensor,false,true));
	TMatrixD hitCov1(lmddim->Transform_global_to_sensor(hitCov,ihalf,iplane,imodule,iside,idie,isensor,false));
	TMatrixD hitCov2(lmddim->Transform_sensor_to_global(hitCov1,ihalf,iplane,imodule,iside,idie,isensor,true));
	myHit.SetPosition(hitPos2);//save value
	myHit.SetCov(hitCov2);//save value
	myHit.SetDx(sqrt(hitCov2[0][0]));
	myHit.SetDy(sqrt(hitCov2[1][1]));
	myHit.SetDz(sqrt(hitCov2[2][2]));
	if(fVerbose>0){
	  cout<<"AFTER [mis]ALIGN:"<<endl;
	  myHit.Print();
	}
      }
      // //Alignment: (END) ---------------------------------------------------------------------------------------   

      if(fVerbose>0){
	std::cout << " -I-  PndSdsPixelClusterTask::Exec(): Calculated Hit(LUMI frame): " << std::endl;
	myHit.Print();
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,34,10))
	((FairMultiLinkedData_Interface)(myHit)).Print();
#else
	((FairMultiLinkedData)(myHit)).Print();
#endif
      }
      TClonesArray& clref = *fHitArray;
      Int_t size = clref.GetEntriesFast();
      PndSdsHit *freshHit = new(clref[size]) PndSdsHit(myHit);
      //myHit.Delete();
      //      myHit.Clear();
      //      std::cout<<"fHitArray.size() = "<<sizeof(*fHitArray)<<std::endl;
    }
  clusters.clear();
  DigiPixelArray.clear();
  if(fVerbose>1)std::cout << std::endl;

  if(fVerbose>1){
    std::cout << "-I- PndLmdPixelClusterTask: " << fClusterArray->GetEntriesFast()
	      << " Sds Clusters and " << fHitArray->GetEntriesFast()<<" Hits calculated." << std::endl;
  }
  fEventNr++;
  fHitArray->Sort();

  // fDigiArray->Delete();

  // std::cout<<"lmddim.size() = "<<sizeof(*lmddim)<<std::endl;

  //  std::cout<<"DigiPixelArray.size() = "<<DigiPixelArray.size()<< std::endl;
  // FinishEvent();
  //  std::cout<<"size() = "<<sizeof(*this)<<std::endl;
  return;  
}

void PndLmdPixelClusterTask::FinishEvent(){

	if (fDigiArray != NULL) 	fDigiArray->Delete();
	if (fHitArray != NULL) 	  fHitArray->Delete();
	if (fClusterArray > 0)  fClusterArray->Delete();
       	FinishEvents();
	//std::cout << "-I- PndLmdPixelClusterTask: FinishEvent()" << std::endl;
}

ClassImp(PndLmdPixelClusterTask);

