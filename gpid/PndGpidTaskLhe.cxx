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
 */

#include "PndGpidTaskLhe.h"


PndGpidTaskLhe::PndGpidTaskLhe()
{
}

PndGpidTaskLhe::~PndGpidTaskLhe()
{
  fVarNameArray.clear();
  fClassNameArray.clear();
}

InitStatus PndGpidTaskLhe::Init()
{
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman ) {
    std::cout << "-E- PndGpidTaskLhe::Init: "
         << "RootManager not instantised!" << std::endl;
    return kFATAL;
  }

fArrPid = new TClonesArray("PndPidCand");
ioman->Register("PndPidCand","Geane", fArrPid, kTRUE);

  fPidTrackCand = (TClonesArray*) ioman->GetObject("LhePidTrack");
  if ( ! fPidTrackCand ) {
    cout << "-W- PndGpidTaskLhe::Init: "
         << "No SamEvt array!" << endl;
    return kERROR;
  }

/*
  fArrTpc = (TClonesArray*) ioman->GetObject("PndTpcPoint"); if
  (!fArrTpc){ cout << "-W- PndGpidTaskLhe::Init: " << "No TpcPoint
  array!" << endl; return kERROR; } fArrMvd = (TClonesArray*)
  ioman->GetObject("PndMvdMCPoint"); if (!fArrTpc){ cout << "-W-
  PndGpidTaskLhe::Init: " << "No MvdPoint array!" << endl; return
  kERROR; } fArrPid = (TClonesArray*) ioman->GetObject("PndPidCand");
  if (!fArrPid){ cout << "-W- PndGpidTaskLhe::Init: " << "No PidCand
  array!" << endl; return kERROR; }
*/
Config();
AddVar();
BookingMVA();

cout<<"-I- This is Init part of the PNDGPIDTASK"<<endl;

 return kSUCCESS;
}
void PndGpidTaskLhe::Config()
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
  while(1)
  {
 size_t prompt = sbuff.find(":");
 sbuff = sbuff.substr(prompt+1,sbuff.size());
 if( sbuff.size() == 0)break;
 vName = sbuff.substr(0,sbuff.find(":"));
 fVarNameArray.push_back(vName); 
 cout<<vName<<endl; 
 }
 inFile.getline(buff,512);
 sbuff=buff;
while(1)
 {
 size_t prompt = sbuff.find(":");
 sbuff = sbuff.substr(prompt+1,sbuff.size());
 if( sbuff.size() == 0)break;
 vName = sbuff.substr(0,sbuff.find(":"));
 fClassNameArray.push_back(vName); 
 cout<<vName<<endl; 
 }
}

void PndGpidTaskLhe::AddVar()
{
 for ( int i = 0 ; i < fNVAR ; i++  )
 {
  varArray[i] = 0.0;  
 }
 //int count = 0;

 for ( int i = 0 ; i < fNVAR ; i++  )
 {
  TString varName,s;
  varName = fVarNameArray.at(i);
  cout<<varName<<endl;
   for ( int j = 0; j < fNCLASS ; j++)
   {
    reader[j].AddVariable(varName, &(varArray[i]));  
   }
 }
}


void PndGpidTaskLhe::BookingMVA()
{
 for (int i = 0 ; i < fNCLASS ; i++  )
 {
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
    //break;
  }
 }
}


void PndGpidTaskLhe::Exec(Option_t* opt)
{
  
  fArrPid->Delete();
  
  TClonesArray& clref2 = *fArrPid;
  Int_t size2 = clref2.GetEntriesFast();
  PndPidCand *fTrack= new(clref2[size2]) PndPidCand(); 
  
  float skp=0; 
  
  for (int k=0; k < fPidTrackCand->GetEntriesFast(); k++){
    //float s,p,gamma2;
    PndLhePidTrack *pid = (PndLhePidTrack *) fPidTrackCand->At(k);
    cout<<"this is Exec"<<endl;
    string varName;
    
    for (int i = 0 ; i< fNVAR; i++){
      varName = fVarNameArray.at(i);
      //FIXME if stt is included and is nan we need to skip the 
      // total event, restructure the CODE.
      if (varName == "stt"){
	    skp=pid->GetSttDEDX();
	    
	  //if(!isnan(skp))
	  if(!(skp == std::numeric_limits<float>::signaling_NaN()))// It is not NaN 
	     varArray[i]=skp;
	  
	  cout<<"stt hit counts"<<pid->GetSttHitCounts()<<"  "<<pid->GetSttDEDX()<<endl;
	  if (skp == 0) 
	    continue;
	
	   fTrack->Set(varName,varArray[i]);
     }
      if (varName == "tof") {
	varArray[i]=pid->GetTof();
	fTrack->Set(varName,varArray[i]);
      }
      if (varName == "emc"){
	varArray[i]=pid->GetEmcELoss()/pid->GetP();
	fTrack->Set(varName,varArray[i]);
      }
     /* Original place of the stt-var read code
    if (varName == "stt"){
	 skp=pid->GetSttDEDX();
	if(!isnan(skp)) varArray[i]=skp;
	   cout<<"stt hit counts"<<pid->GetSttHitCounts()<<"  "<<pid->GetSttDEDX()<<endl;
	if (skp==0) continue;
	
	fTrack->Set(varName,varArray[i]);
    }*/
      if (varName == "mvd"){
	varArray[i]=pid->GetMvdDEDX();
	fTrack->Set(varName,varArray[i]);
      }
      if (varName == "thetaC"){
	varArray[i]=pid->GetDrcThetaC();
	fTrack->Set(varName,varArray[i]);
      }
      if (varName == "p"){
	varArray[i]=pid->GetMomentum().Mag();
	cout<<"momentum "<<pid->GetMomentum().Mag()<<endl;
	fTrack->Set(varName,varArray[i]);
      }
      
      // cout<<varArray[i]<<" "<<varName<<endl;
    }
    
    
    
    //if(varArray[0] == 0 || varArray[1] == 0 || varArray[2] == 0 ||
    //varArray[3] == 0 || varArray[4] ==0 || varArray[5] ==0 )
    //continue; if(varArray[0] < 0 || varArray[1] < 0 || varArray[2] <
    //0 || varArray[3] < 0 || varArray[4] < 0 || varArray[5] < 0 )
    //continue;
    /*
      if(varArray[1] == 0) continue;
      if(varArray[2] == 0) continue;
      if(varArray[3] == 0) continue;
      if(varArray[4] == 0) continue;
      if(varArray[5] == 0) continue;
    */
    
    for (int l = 0; l < fNVAR; l++){
      cout<<fVarNameArray.at(l)<<"  "<<varArray[l]<<endl;
    }
    Float_t mvaValue;
   for (int i = 0 ; i < fNCLASS; i++){
     string className;
     
     className = fClassNameArray.at(i);
     mvaValue = reader[i].EvaluateMVA(fClassifier);
     
     switch (fMVAmode){
       
     case BDT:
       mvaValue = (mvaValue - (-1.0))/2.0;
       break;
       
     case MLP:
       mvaValue = (mvaValue - (-1.1))/2.2;
       break;
       
     default:// KNN
      //mvaValue = mvaValue;
      break;
     }
     
     if (mvaValue < 0) mvaValue = 0.0001;
     
     fTrack->Set(className,mvaValue);
     cout<<"Likelihood for the class "<<className<<"is :"<<mvaValue<<endl;
   }
  }
}
ClassImp(PndGpidTaskLhe);
