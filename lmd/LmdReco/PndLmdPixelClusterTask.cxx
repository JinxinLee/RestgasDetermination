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
  flagMS = true;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndLmdPixelClusterTask::~PndLmdPixelClusterTask()
{
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
  
  std::cout << "-I- PndSdsPixelClusterTask: Initialisation successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Initialization  of Parameter Containers -------------------------
void PndLmdPixelClusterTask::SetParContainers()
{
  // Get Base Container
	ana = FairRun::Instance();
	rtdb=ana->GetRuntimeDb();
	fDigiPar = (PndSdsPixelDigiPar*)(rtdb->getContainer("SDSPixelDigiPar"));
	rtdb->getContainer("SDSPixelTotDigiPar");
	

	//Read lmd geo description. still not sure where and how to do it as soon as Init stays in PndSdsPixelClusterTask
	lmddim = PndLmdDim::Instance();
	// lmddim -> Read_transformation_matrices("matrices.txt", true);
	lmddim -> Read_transformation_matrices("matrices_perfect.txt", false);

	PndLmdContFact* themvdcontfact = (PndLmdContFact*)rtdb->getContFactory("PndLmdContFact");
	//read params for lumi alignment
	TList* theAlignLMDContNames = themvdcontfact->GetAlignParNames();
	Info("SetParContainers()","AlignLMD The container names list contains %i entries",theAlignLMDContNames->GetEntries());
	TIter cfAlIter(theAlignLMDContNames);
	while (TObjString* contname = (TObjString*)cfAlIter()) {
	  TString parsetname = contname->String();
	  Info("SetParContainers()",parsetname.Data());
	  PndLmdAlignPar *lmdalignpar = (PndLmdAlignPar*)(rtdb->getContainer(parsetname.Data()));
	  if(!lmdalignpar) Fatal("SetParContainers","No ALIGN parameter found: %s",parsetname.Data());
	  fAlignParamList->Add(lmdalignpar); 
	}
	//
	PndSdsPixelClusterTask::SetParContainers();

}

void PndLmdPixelClusterTask::SetAlignConst(){
  TIter alignparams(fAlignParamList); 
  PndLmdAlignPar* lmdalignpar=(PndLmdAlignPar*)alignparams();
  if(0==lmdalignpar) { 
    Error("PndLmdStripClusterTask::SetCalculators()","A ALIGN Parameter Set does not exist properly.");
  } 
  else{
    //   lmdalignpar->Print();
    Int_t nsens = lmdalignpar->GetNsensors();
    Int_t nsides = lmdalignpar->GetNsides();
    Int_t nplanes = lmdalignpar->GetNplanes();
    for(int ik=0;ik<nplanes*nsides*nsens;ik++){
      fShiftX.push_back(lmdalignpar->GetShiftX(ik));
      fShiftY.push_back(lmdalignpar->GetShiftY(ik));
      fShiftZ.push_back(lmdalignpar->GetShiftZ(ik));
      fRotateX.push_back(lmdalignpar->GetRotateX(ik));
      fRotateY.push_back(lmdalignpar->GetRotateY(ik));
      fRotateZ.push_back(lmdalignpar->GetRotateZ(ik));
      if (fVerbose > 2) cout<<"fShiftX["<<ik<<"]="<<fShiftX[ik]<<" fRotateX["<<ik<<"]="<<fRotateX[ik]
			    <<" fRotateY["<<ik<<"]="<<fRotateY[ik]<<" fRotateZ["<<ik<<"]="<<fRotateZ[ik]<<endl;
    }
  }
 if (fVerbose > 2)  lmdalignpar->Print();
}
void PndLmdPixelClusterTask::SetBackMapping()
{
  // at Init() stage we already have fGeoH filled with the parameters
	fBackMapping = new PndLmdChargeWeightedPixelMapping(fGeoH, fVerbose);
}

void PndLmdPixelClusterTask::SetClusterFinder()
{
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

void PndLmdPixelClusterTask::combitransToLumiFrame(TVector3& hitPos){
  //do the transformation from lab frame to LUMI frame (with z-axis perp. to lumi planes)
  double end_seg_upstream = 360.1; // where bending starts with
  double r_bend = 5750.; // the bending radius
  double phi_bend = 40.068e-3; // and the angle of the circle path
  // const Double_t kRot = phi_bend/3.141*180.;//=2.295727;//2.326; //(deg) //Rotate to dipol
  const Double_t kRot = phi_bend;//here we need abgle in rad!
  // the point where both tangents of the straight beam pipe tubes meet is
  const Double_t kRotUmZ = end_seg_upstream + tan(phi_bend/2.)*r_bend;//476.03; //(cm) //z-point to rotate
  const Double_t kTransZ = 1130.; //(cm) //move at z-position
  const Double_t kTransX = (kTransZ - end_seg_upstream - tan(phi_bend/2.)*r_bend)*tan(phi_bend); // 25 (cm) //move at x-position
  // cout<<"kTransX = "<<kTransX<<" kRotUmZ = "<<kRotUmZ<<" kRot = "<<kRot<<endl;
  // const Double_t  kHalfFoilThickness  = 0.0075; // Thickness of sensitive foil (cm)
  // const Double_t  kTransZ = 1130.; //(cm) //move at z-position
  // const Double_t  kRotUmZ = 476.03; //(cm) //z-point to rotate
  // const Double_t  kTransX = 25; //(cm) //move at x-position
  // const Double_t  kRot =  0.040596358401388; // 2.326 degree  = 4.05963584013881024e-02 rad
  TVector3 LumiTrans(0,0,kRotUmZ);
  hitPos -=LumiTrans;
  hitPos.RotateY(-kRot);
  LumiTrans = TVector3(0,0,kTransZ-kRotUmZ);
  hitPos -=LumiTrans;
  // cout<<"!!! NEW HIT position in LUMI frame!!! "<<endl;
  //  hitPos.Print();
}

void PndLmdPixelClusterTask::rotateToLumiFrame(TVector3& hitPos){
  TMatrixD hitMtx(3,3);
  hitMtx[0][0] = hitPos[0];
  hitMtx[1][0] = hitPos[1];
  hitMtx[2][0] = hitPos[2];
  TMatrixD res = rotateToLumiFrame(hitMtx);
  hitPos = TVector3(hitMtx(0,0),hitMtx(1,0),hitMtx(2,0));
}
TMatrixD PndLmdPixelClusterTask::rotateToLumiFrame(TMatrixD& hitCov){
  double phi_bend = 40.068e-3; // and the angle of the circle path
  Double_t sintheta = TMath::Sin(phi_bend);
  Double_t costheta = TMath::Cos(phi_bend);
  TMatrixD rot(3,3);// Rotation around Y axis
  rot[0][0]= costheta;
  rot[0][1]= 0;
  rot[0][2]= sintheta;
  rot[1][0]= 0;
  rot[1][1]= 1;
  rot[1][2]= 0;
  rot[2][0]= -sintheta;
  rot[2][1]= 0;
  rot[2][2]= costheta;
  TMatrixD result = rot;
  result.T();
  result*=hitCov;
  hitCov = result;
  result*=rot;
  return result;
}


//Correction to hit position due to misalignment of sensor
//TO DO: find a way do it in global and not hit by hit.
void PndLmdPixelClusterTask::alignmentCorr(TVector3& hitPos, int ssensID){
  if(readAlign){
    SetAlignConst();
    readAlign = false;
  }
  int sensID = ssensID;// aligment only on petal with 4 sensors???
  cout<<"fShiftX["<<sensID<<"]="<<fShiftX[sensID]<<endl;
  TVector3 hitPos_loc(hitPos.X(),hitPos.Y(),0.);
  hitPos_loc -=TVector3(fShiftX[sensID],fShiftY[sensID],fShiftZ[sensID]);
  double xnew = hitPos_loc.X()+fRotateZ[sensID]*hitPos_loc.Y()-fRotateY[sensID]*hitPos_loc.Z();
  double ynew = hitPos_loc.Y()-fRotateZ[sensID]*hitPos_loc.X()+fRotateX[sensID]*hitPos_loc.Z();
  double znew = hitPos_loc.Z()-fRotateY[sensID]*hitPos_loc.X()-fRotateX[sensID]*hitPos_loc.Y();
  hitPos = TVector3(xnew,ynew,hitPos.Z()+znew);
}


TVector3 PndLmdPixelClusterTask::AddMSErr(TVector3 hpos, TVector3 hposerr){
  if(fVerbose>0) Info("PndLmdPixelClusterTask::AddMSErr","calculation additional errors due to multiple scaterring");

  //Calculation of ThetaMS -------------------------------------
  //Charge & mass of particle
  Int_t PDGCode = -2212;
  Double_t fMass = 0.938272046;
  Double_t Ebeam = TMath::Hypot(fPbeam,fMass);
  TLorentzVector LorMom(0, 0, fPbeam, Ebeam);
  Double_t beta = LorMom.Beta();
  Double_t X_to_X0 = 0.00306;//for one plane: flexcable+(HV-MAPS)+cooling disk+(HV-MAPS)+flexcable
  X_to_X0 -= 0.00053;//-(HV-MAPS)
  Double_t thetaMS = 13.6*1e-3*TMath::Sqrt(X_to_X0)/(beta*fPbeam);
  //-----------------------------------------------------------

  //TO DO: use parameters from geometry info for LUMI
  Double_t d1 = 20.; 
  Double_t d2 = 10.; 
  double xerr,yerr;
  xerr = hposerr.X();
  yerr = hposerr.Y();
  double zhit = hpos.Z();
  int num = 0;
  if(zhit>1140 && zhit<1150) num=1;
  if(zhit>1150 && zhit<1160) num=2;
  if(zhit>1160 && zhit<1170) num=3;

  
  // cout<<"Plane #"<<num<<" before: zhit="<<zhit<<" xerr = "<<xerr<<" yerr = "<<yerr<<endl;
 
  double sigmaMS;
  for(int j=0;j<num;j++){
    // sigmaMS = 2*(j+1)*d*thetaMS;
    double d=d2;
    if(j==0) d=d2;
    sigmaMS = (j+1)*d*thetaMS;
    xerr = TMath::Hypot(xerr,sigmaMS);
    yerr = TMath::Hypot(yerr,sigmaMS); 
    //  cout<<"num:"<<num<<" j="<<j<<" d = "<<d<<endl;
  }
  // cout<<" num:"<<num<<"(Z="<<zhit<<") xerr="<<xerr<<" yerr="<<yerr<<endl;
  TVector3 res(xerr,yerr,hposerr.Z());
  return res;
};

// -----   Public method Exec   --------------------------------------------
void PndLmdPixelClusterTask::Exec(Option_t* opt)
{
  std::vector<PndSdsDigiPixel> DigiPixelArray;
  // Reset output array
  if ( ! fClusterArray ) Fatal("Exec", "No ClusterArray");

  fGeoH->SetVerbose(fVerbose);

  Double_t EventTime = FairRootManager::Instance()->GetEventTime();

   if(fVerbose>0) std::cout << "-I- PndSdsPixelClusterTask::Exec EventTime: " << EventTime << std::endl;

    if (FairRunAna::Instance()->IsTimeStamp()){
    	fDigiArray = FairRootManager::Instance()->GetData(fInBranchName, fStartFunctor, EventTime + 10);
    }

  if ( ! fHitArray ) Fatal("Exec", "No HitArray");

  Int_t nPoints = fDigiArray->GetEntriesFast();
  //std::cout << "Points in DigiArray: " << nPoints << std::endl;
  // convert from TClonesArray to a std::vector
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++){
    PndSdsDigiPixel myDigi = *(PndSdsDigiPixel*)(fDigiArray->At(iPoint));
    DigiPixelArray.push_back(myDigi);
  }
  // Retrieve the calculated clusters with the chosen clusterfinder
  std::vector< std::vector< Int_t> > clusters = fClusterFinder->GetClusters(DigiPixelArray);
  if(fVerbose>1) std::cout << " -I-  PndSdsPixelClusterTask::Exec(): We have "<<clusters.size()<<" pixel clusters" << std::endl;
  // store the list
  for (UInt_t i = 0; i < clusters.size(); i++)
  {
    PndSdsClusterPixel* tempCluster = new((*fClusterArray)[i]) PndSdsClusterPixel(fInBranchId, clusters[i]);

    if (FairRunAna::Instance()->IsTimeStamp()){
//		std::cout << "TempCluster: " << *tempCluster << std::endl;
		tempCluster->ResetLinks();
		for (UInt_t j = 0; j < clusters[i].size(); j++){
			PndSdsDigiPixel* tempDigi = (PndSdsDigiPixel*)fDigiArray->At(clusters[i][j]);
//			std::cout << "TempDigi: " << *tempDigi << std::endl;
//			std::cout << "EntryNr: ";
//			tempDigi->GetEntryNr().Print();
//			std::cout << std::endl;
			tempCluster->AddLink(FairLink(tempDigi->GetEntryNr()));
//			std::cout << "Links: " << (FairMultiLinkedData)(*tempCluster) << std::endl;
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
    PndSdsHit myHit = fBackMapping->GetCluster(clusterArray);
    myHit.SetClusterIndex(fClusterType,i, 0, fEventNr);
    TMatrixD hitCov = myHit.GetCov();
    // hitCov.Print();
    hitCov(0,0) = 5.56960000000000085e-06; //assuming hit resolution for x-y 23.6 mkm
    hitCov(1,1) = 5.56960000000000085e-06; //assuming hit resolution for x-y 23.6 mkm
    hitCov(2,2) = 4.28489999999999954e-08; //assuming hit resolution for z 2.07 mkm
    
    //Add multiple scattering error ---------------
    if(flagMS){
      TVector3 hitPos = myHit.GetPosition();
      TVector3 hitErr(sqrt(hitCov[0][0]),sqrt(hitCov[1][1]),sqrt(hitCov[2][2]));
      TVector3 hitErrMSadd = AddMSErr(hitPos, hitErr);
      hitCov[0][0] = TMath::Power(hitErrMSadd.X(),2);
      hitCov[1][1] = TMath::Power(hitErrMSadd.Y(),2);
      hitCov[2][2] = TMath::Power(hitErrMSadd.Z(),2);
    }
    myHit.SetCov(hitCov);//save value

    //translate to LUMI frame --------------------
    // if(fVerbose>0){
    //   cout<<"Before transl to LUMI frame:"<<endl;
    //myHit.Print();
    // }
    // TVector3 hitPos = myHit.GetPosition();
    // TMatrixD hitCov = myHit.GetCov();
    // ///don't want touch BackMapping now
    // ///Let's work with coordinates in Global frame
    // hitPos = lmddim->Transform_global_to_lmd_local(hitPos, false, false);
    // hitCov = lmddim->Transform_global_to_lmd_local(hitCov, false);
    // if(fVerbose>0){
    //   cout<<"After Transform_global_to_lmd_local:"<<endl;
    //   hitPos.Print();
    //   hitCov.Print();
    // }
   
    // myHit.SetPosition(hitPos);//save value
    // myHit.SetCov(hitCov);//save value
    //translate to LUMI frame (END) ---------------

    if(fVerbose>0){
      std::cout << " -I-  PndSdsPixelClusterTask::Exec(): Calculated Hit(LUMI frame): " << std::endl;
      myHit.Print();
      ((FairMultiLinkedData)(myHit)).Print();
    }

    new ((*fHitArray)[i]) PndSdsHit(myHit);
  }
  if(fVerbose>1)std::cout << std::endl;

  if(fVerbose>1){
    std::cout << "-I- PndSdsPixelClusterTask: " << fClusterArray->GetEntriesFast()
    << " Sds Clusters and " << fHitArray->GetEntriesFast()<<" Hits calculated." << std::endl;
  }
  fEventNr++;
  fHitArray->Sort();
  return;
  
}


ClassImp(PndLmdPixelClusterTask);

