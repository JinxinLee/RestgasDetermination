// -------------------------------------------------------------------------
// -----                PndLmdPixelClusterTasksource file             -----
// -------------------------------------------------------------------------


#include "PndLmdPixelClusterTask.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "PndSdsPixelDigiPar.h"
#include "PndLmdContFact.h"
#include "PndLmdAlignPar.h"
// -----   Default constructor   -------------------------------------------
PndLmdPixelClusterTask::PndLmdPixelClusterTask() :
PndSdsPixelClusterTask("LMD Clustertisation Task")
{
  fPersistance = kTRUE;
  fAlignParamList = new TList();
  readAlign = true;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndLmdPixelClusterTask::~PndLmdPixelClusterTask()
{
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
  lmdalignpar->Print();
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
  const Double_t  kHalfFoilThickness  = 0.0075; // Thickness of sensitive foil (cm)
  const Double_t  kTransZ = 1100.; //(cm) //move at z-position
  const Double_t  kRotUmZ = 476.03; //(cm) //z-point to rotate
  const Double_t  kTransX = 25; //(cm) //move at x-position
  const Double_t  kRot =  0.040596358401388; // 2.326 degree  = 4.05963584013881024e-02 rad
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
  Double_t theta=2.326;
  Double_t degrad = TMath::Pi()/180.;
  Double_t sintheta = TMath::Sin(degrad*theta);
  Double_t costheta = TMath::Cos(degrad*theta);
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
		tempCluster->Reset();
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

    TVector3 hitPos = myHit.GetPosition();
    // do the transformation from lab frame to LUMI frame (with z-axis perp. to lumi planes)
    combitransToLumiFrame(hitPos);
    // //do correction due to misalignemt of sensor
    // int sensorID = myHit.GetSensorID();
    // alignmentCorr(hitPos,sensorID);
    myHit.SetPosition(hitPos);//save value

    TMatrixD hitCov = myHit.GetCov();
    //transformation to LUMI frame
    hitCov = rotateToLumiFrame(hitCov);
    myHit.SetCov(hitCov);//save value

 //   myHit.SetCharge(myHit.GetCharge());
    if(fVerbose>0){
      std::cout << " -I-  PndSdsPixelClusterTask::Exec(): Calculated Hit: " << std::endl;
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

