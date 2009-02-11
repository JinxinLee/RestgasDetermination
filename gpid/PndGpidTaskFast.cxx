
#include "PndGpidTaskFast.h"
#include "Event.h"
#include <iostream>
using namespace std;

PndGpidTaskFast::PndGpidTaskFast()
{
}

PndGpidTaskFast::~PndGpidTaskFast()
{
}

InitStatus PndGpidTaskFast::Init()
{
  
  cout<<"-I-  Here is the Initialization"<<endl;
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    std::cout << "-E- PndGpidTaskFast::Init: "
         << "RootManager not instantised!" << std::endl;
    return kFATAL;
  }

 fPndTrackArr = new TClonesArray("PndPidCand");
  ioman->Register("PndPidCand","Geane", fPndTrackArr, kTRUE);



 cout<<ioman
     <<"							---------------------------------------------"<<endl;
 //fArrCand = (TClonesArray*) ioman->GetObject("PndMicroCandidates");
 fArrCand = (TClonesArray*) ioman->GetObject("PndMcTracks");
 if (!fArrCand){
   cout << "-W- PndGpidTaskFast::Init: "
	<< "No PidCand array!" << endl;
   return kFATAL;
 }
 cout<<"							---------------------------------------------"<<fArrCand->IsEmpty()<<endl;
 cout<<"							---------------------------------------------"<<fArrCand->GetEntriesFast()<<endl;
 
 Config();
 AddVar();
 BookingMVA();
 
 return kSUCCESS;
}
void PndGpidTaskFast::Config()
{
  ifstream inFile;
  string sbuff,sfName,small_buff,vName;
  char buff[512];
  sfName = fAPPNAME + ".dat";
  inFile.open(sfName.c_str(),ios::in);
  inFile.getline(buff,512);
  fNVAR = (int) atoi(buff);
  inFile.getline(buff,512);
  fNCLASS = (int) atoi(buff);
  inFile.getline(buff,512);
  
  cout<<fNVAR<<endl;
  cout<<fNCLASS<<endl;
  sbuff=buff;

  while(1){
    size_t prompt = sbuff.find(":");
    sbuff = sbuff.substr(prompt+1,sbuff.size());
    if( sbuff.size() == 0)break;
    vName = sbuff.substr(0,sbuff.find(":"));
    fVarNameArray.push_back(vName); 
    cout<<vName<<endl; 
  }
  inFile.getline(buff,512);
  sbuff=buff;
  while(1){
    size_t prompt = sbuff.find(":");
    sbuff = sbuff.substr(prompt+1,sbuff.size());
    if( sbuff.size() == 0)break;
    vName = sbuff.substr(0,sbuff.find(":"));
    fClassNameArray.push_back(vName); 
    cout<<vName<<endl; 
  }
}

void PndGpidTaskFast::AddVar()
{
  for ( int i = 0 ; i < fNVAR ; i++  ){
    varArray[i] = 0.0;  
  }
  int count = 0;
  
  for ( int i = 0 ; i < fNVAR ; i++  ){
    TString varName,s;
    varName = fVarNameArray.at(i);
    cout<<varName<<endl;
    for ( int j = 0; j < fNCLASS ; j++){
      reader[j].AddVariable(varName, &(varArray[i]));  
    }
  }
}


void PndGpidTaskFast::BookingMVA()
{
 for (int i = 0 ; i < fNCLASS ; i++  ){
   string anaFile;
  switch (fMVAmode){
  case BDT: 
    anaFile =  fDIR + fAPPNAME + fClassNameArray.at(i) + "_BDT.weights.txt";
    reader[i].BookMVA("BDT method", anaFile );
    fClassifier = "BDT method";
    break;
    
  case KNN: 
    anaFile =  fDIR + fAPPNAME + fClassNameArray.at(i) + "_KNN.weights.txt";
    reader[i].BookMVA("KNN method", anaFile );
    fClassifier = "KNN method";
    break;
    
  case MLP:
    anaFile =  fDIR + fAPPNAME + fClassNameArray.at(i) + "_MLP.weights.txt";
    reader[i].BookMVA("MLP method", anaFile );
    fClassifier = "MLP method";
    break;
  }
 }
}


void PndGpidTaskFast::Exec(Option_t* opt)
{
  
  TClonesArray& clref2 = *fPndTrackArr;
  Int_t size2 = clref2.GetEntriesFast();
  PndPidCand *fTrack= new(clref2[size2]) PndPidCand();
  
  float s,p,gamma2;
  for(int k=0; k < fArrCand->GetEntriesFast();k++){
    PndMicroCandidate *pid = (PndMicroCandidate *) fArrCand->At(k);
    cout<<"say some thing"<<pid<<endl;
    if(pid ==NULL) continue;
    
    TVector3 vec; 
 for (int i = 0 ; i< fNVAR; i++){
   string varName;
   
   varName = fVarNameArray.at(i);
   if (varName == "stt"){
     varArray[i] = pid->GetSttMeanDEdx();
     cout<<varArray[i]<<"stt val"<<endl;
   }
   if (varName == "mvd"){
     varArray[i] = pid->GetMvdMeanDEdx();
   }
   
   if (varName == "thetaC"){
     varArray[i]=pid->GetBarrelDrcThetaC();
   }
   
   if (varName == "p"){
     pid->GetMomentum();
     varArray[i]=vec.Mag();
   }
   
   if (varName == "tof")
     varArray[i]=pid->GetTofM2();
   // cout<<varArray[i]<<" "<<varName<<endl;
 }

   for (int i = 0 ; i < fNCLASS; i++){
     string className;
     Float_t mvaValue;
     className = fClassNameArray.at(i);
     mvaValue = reader[i].EvaluateMVA(fClassifier);
     switch (fMVAmode){
     case BDT:
       mvaValue = (mvaValue - (-0.92))/1.72;
     case KNN:
       mvaValue = mvaValue; 
     case MLP:
       mvaValue = (mvaValue - (-1.1))/2.2;
     }
     /*
       if (className == "pion")
       pid->SetPionPidLH(mvaValue);
       if (className == "proton")
       pid->SetProtonPidLH(mvaValue);
       if (className == "kaon")
       pid->SetKaonPidLH(mvaValue);
       if (className == "muon")
       pid->SetMuonPidLH(mvaValue);
       if (className == "electron")
       pid->SetElectronPidLH(mvaValue);
     */
     //  cout<<mvaValue<<endl;
     fTrack->Set(className,mvaValue);
   }
  }  
}
ClassImp(PndGpidTaskFast)
