/*
 *  S.Vanniarajan  V.Suyam.Jothi@kvi.nl
 *
 *  This is the Mutivariate Event classification tool
 *  designed for PANDAROOT Analysis package for 
 *  PANDA Experiments.
 *       TMVA(Toolkit for MultiVariate Analysis) is a 
 *  two class classifier(signal and background). 
 *  available with ROOT package. mainly used for 
 *  Event Classification in High Energy Physics Experiments.
 *         
 *       This tool here is Designed from TMVA, for Multiclass 
 * Classification purpose. 
 *
 *                  S.Vanniarajan  01-08-08
 * Modified:
 * M. Babai
 * Vanni 09-02-08 
 */

#include "PndGpidTask.h"


//default constructor

PndGpidTask::PndGpidTask()
{}

// default destructor

PndGpidTask::~PndGpidTask()
{
  fVarNameArray.clear();
  fClassNameArray.clear();
}

//Init call of the task inherited from the Cbmtask 
//Registers the output of the track and get the inputs of the track

InitStatus PndGpidTask::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    std::cout << "-E- PndGpidTask::Init: "
	      << "RootManager not instantised!" << std::endl;
    return kFATAL;
  }
  /*
    fPidTrackCand = (TClonesArray*) ioman->GetObject("LhePidTrack");
    if ( ! fPidTrackCand ) {
    cout << "-W- PndGpidTask::Init: "
    << "No SamEvt array!" << endl;
    return kERROR;
    }
  */
  fArrTpc = (TClonesArray*) ioman->GetObject("PndTpcPoint");
  if (!fArrTpc){
    cout << "-W- PndGpidTask::Init: "
         << "No TpcPoint array!" << endl;
    return kERROR;
  }
  fArrMvd = (TClonesArray*) ioman->GetObject("PndMvdMCPoint");
  if (!fArrTpc){
    cout << "-W- PndGpidTask::Init: "
         << "No MvdPoint array!" << endl;
    return kERROR;
  }
  fArrPid = (TClonesArray*) ioman->GetObject("PndPidCand");
  if (!fArrPid){
    cout << "-W- PndGpidTask::Init: "
         << "No PidCand array!" << endl;
    return kERROR;
  }
  Config();
  AddVar();
  BookingMVA();
  
  return kSUCCESS;
}



// config call private member reading the config file 
void PndGpidTask::Config()
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


// Adding the variable called by Init
void PndGpidTask::AddVar()
{
  for ( int i = 0 ; i < fNVAR ; i++){
    varArray[i] = 0.0;  
  }
  int count = 0;
  
  for ( int i = 0 ; i < fNVAR ; i++){
    TString varName,s;
    varName = fVarNameArray.at(i);
    cout<<varName<<endl;
    for ( int j = 0; j < fNCLASS ; j++){
      reader[j].AddVariable(varName, &(varArray[i]));  
    }
  }
}


// books the MVA called by the Init 
void PndGpidTask::BookingMVA()
{
  for (int i = 0 ; i < fNCLASS ; i++  ){
    string anaFile;
    switch (fMVAmode){
    case BDT: 
      anaFile =  fDIR + fAPPNAME + fClassNameArray.at(i) + "_BDT.weights.txt";
      reader[i].BookMVA("BDT method", anaFile );
      fClassifier = "BDT method";
      break;
    
    case MLP:
      anaFile =  fDIR + fAPPNAME + fClassNameArray.at(i) + "_MLP.weights.txt";
      reader[i].BookMVA("MLP method", anaFile );
      fClassifier = "MLP method";
      break;
      
      default://case KNN: 
      anaFile =  fDIR + fAPPNAME + fClassNameArray.at(i) + "_KNN.weights.txt";
      reader[i].BookMVA("KNN method", anaFile );
      fClassifier = "KNN method";
      break;
    }
  }
}



// Exec call of the task
void PndGpidTask::Exec(Option_t* opt)
{
  
  float s,p,gamma2;
  PndPidCand *pid = (PndPidCand *) fArrPid->At(0);
  
  for (int i = 0 ; i< fNVAR; i++){
    string varName;
    float de_tpc=0;
    float dx_tpc=0;
    float de_mvd=0;
    float dx_mvd=0;
    
    varName = fVarNameArray.at(i);
    if (varName == "tpc"){
      for (Int_t j = 0; j < fArrTpc->GetEntriesFast(); j++ ){
	PndTpcPoint *tpc = (PndTpcPoint *) fArrTpc->At(j);
	if(tpc == 0)continue;
	de_tpc += tpc->GetEnergyLoss();
	dx_tpc += tpc->GetLength();
      }
    }
    if (varName == "mvd"){
      for (Int_t j = 0; j < fArrMvd->GetEntriesFast(); j++ ){
	PndMvdMCPoint *mvd = (PndMvdMCPoint *) fArrMvd->At(j);
	if(mvd == 0)continue;
	de_mvd += mvd->GetEnergyLoss();
	dx_mvd += mvd->GetLength();
      }
    }
    
    if (varName == "beta"){
      varArray[i]=pid->Get("speed");
    }
    
    if (varName == "PMag"){
      varArray[i]=pid->Get("PMag");
    }
    
    if (varName == "msquare")
      varArray[i]=pid->Get("msquare");
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
    pid->Set(className,mvaValue);
    //  cout<<mvaValue<<endl;
    // events->Set(className,mvaValue);
  }
}
ClassImp(PndGpidTask);
