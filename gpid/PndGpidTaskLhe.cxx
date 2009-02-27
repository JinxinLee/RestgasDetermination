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
 */

#include "PndGpidTaskLhe.h"

//default constructor
PndGpidTaskLhe::PndGpidTaskLhe()
{
}

// default destructor
PndGpidTaskLhe::~PndGpidTaskLhe()
{
  fVarNameArray.clear();
  fClassNameArray.clear();
  m_varVec.clear();
  delete m_lvq;
}

//Init call of the task inherited from the Cbmtask Registers the
//output of the track and get the inputs of the track Input is
//LhePidTrack Output is PndPidCand

InitStatus PndGpidTaskLhe::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();
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
  
  Config();
  AddVar();
  BookingMVA();
  
  cout<<"-I- This is Init part of the PNDGPIDTASK"<<endl;
  
  return kSUCCESS;
}

// config call private member reading the config file 
//
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
  while(1){
    size_t prompt = sbuff.find(":");
    sbuff = sbuff.substr(prompt+1,sbuff.size());
    if( sbuff.size() == 0) break;
    vName = sbuff.substr(0,sbuff.find(":"));
    fVarNameArray.push_back(vName); 
    cout<<vName<<endl; 
  }
  inFile.getline(buff,512);
  sbuff=buff;
  while(1){
    size_t prompt = sbuff.find(":");
    sbuff = sbuff.substr(prompt+1,sbuff.size());
    if( sbuff.size() == 0) break;
    vName = sbuff.substr(0,sbuff.find(":"));
    fClassNameArray.push_back(vName); 
    cout<<vName<<endl; 
  }
}


// Adding the variable called by Init

void PndGpidTaskLhe::AddVar()
{
  m_varVec = std::vector<float>(fNVAR, 0.00);
  
  for ( int i = 0 ; i < fNVAR ; i++  ){
    TString varName,s;
    varName = fVarNameArray.at(i);
    cout<<varName<<endl;
    for ( int j = 0; j < fNCLASS ; j++){
      reader[j].AddVariable(varName, &(m_varVec[i]));
    }
  }
}

// books the MVA called by the Init
void PndGpidTaskLhe::BookingMVA()
{
  for (int i = 0 ; i < fNCLASS ; i++  )
  {
    string anaFile;
    switch (fMVAmode){
    case TMBDT: 
      anaFile =  fDIR + fAPPNAME + fClassNameArray.at(i) + "_BDT.weights.txt";
      reader[i].BookMVA("BDT method", anaFile );
      fClassifier = "BDT method";
      break;
      
    case TMMLP:
      anaFile =  fDIR + fAPPNAME + fClassNameArray.at(i) + "_MLP.weights.txt";
      reader[i].BookMVA("MLP method", anaFile );
      fClassifier = "MLP method";
      break;
      
    case TMKNN: 
      anaFile =  fDIR + fAPPNAME + fClassNameArray.at(i) + "_KNN.weights.txt";
      reader[i].BookMVA("KNN method", anaFile );
      fClassifier = "KNN method";
      break;
    case MulClsKNN:
      std::cout << "Not available yet" << std::endl;
      break;

    case LVQ1:
      m_lvq = new PndLVQClassify(M_InFileName,fClassNameArray,fVarNameArray);
      break;
      
    default:
      std::cout << "<ERROR:> NO classifier was selected." << std::endl;
      break;
    }
  }
}

// Exec call of the task
void PndGpidTaskLhe::Exec(Option_t* opt)
{
  
  fArrPid->Delete();
  
  TClonesArray& clref2 = *fArrPid;
  Int_t size2 = clref2.GetEntriesFast();
  PndPidCand *fTrack= new(clref2[size2]) PndPidCand(); 
  float skp=0; 
  
  // Loop through the Tracks
  for (int k=0; k < fPidTrackCand->GetEntriesFast(); k++){
    PndLhePidTrack *pid = (PndLhePidTrack *) fPidTrackCand->At(k);
    cout<<"this is Exec"<<endl;
    string varName;

    for (int l = 0; l < fNVAR; l++){
      cout << fVarNameArray.at(l) <<" = "<< m_varVec[l] << endl;//varArray[l] << endl;
    }

    int count =0 ;
    
    //reading the input variable
    for (int i = 0 ; i< fNVAR; i++){
      varName = fVarNameArray.at(i);
      
      if (varName == "tof") {
	m_varVec[i]=pid->GetTof();
	if (m_varVec[i] < 0 || m_varVec[i] == 0 ) count+=1;
	fTrack->Set(varName, m_varVec[i]);
      }

      if (varName == "emc"){
	m_varVec[i]= pid->GetEmcELoss()/pid->GetP();
	if (m_varVec[i] < 0 || m_varVec[i] ==0 ) count+=1;
	cout<<"emc "<< m_varVec[i]<< endl;
	fTrack->Set(varName,m_varVec[i]);
      }
      
      if (varName == "stt"){

	skp=pid->GetSttDEDX();
	if(!isnan(skp)) m_varVec[i] = skp;

	cout<<"stt hit counts "<<pid->GetSttHitCounts()
	    <<" Value is "<<pid->GetSttDEDX()<<endl;

	if ( skp < 0 || skp ==0 ) count+=1;
	if (skp == 0) continue;

	fTrack->Set(varName,m_varVec[i]);
      }

      if (varName == "mvd"){
	m_varVec[i]=pid->GetMvdDEDX();
	if (m_varVec[i] < 0 || m_varVec[i] ==0 ) count+=1;

	fTrack->Set(varName,m_varVec[i]);
      }

      if (varName == "thetaC"){
	m_varVec[i]=pid->GetDrcThetaC();
	if (m_varVec[i] < 0 || m_varVec[i] ==0 ) count+=1;

	fTrack->Set(varName,m_varVec[i]);
      }
      
      if (varName == "p"){
	m_varVec[i]=pid->GetMomentum().Mag();
	if (m_varVec[i] < 0 || m_varVec[i] == 0 ) count+=1;

	cout<<"momentum "<<pid->GetMomentum().Mag()<<endl;
	fTrack->Set(varName,m_varVec[i]);
      }
      // cout<< varName << " = " << m_varVec[i] << endl;
    }
    
    if (count > 0 ) continue;

      if(fMVAmode == MulClsKNN || fMVAmode == LVQ1){
	std::map<std::string,float> res;

	switch(fMVAmode){
	case MulClsKNN:
	  std::cout << "Not available" << std::endl;
	  break;
	case LVQ1:
	  m_lvq->Classify(m_varVec, res);
	  printResult(res);
	  fTrack->Set(res);
	  break;
	default:
	  std::cout << "Unknown classifier" << std::endl;
	}
      }

    else{
      float mvaValue;
      for (int i = 0 ; i < fNCLASS; i++){
	std::string className;
	
	className = fClassNameArray.at(i);
	mvaValue = reader[i].EvaluateMVA(fClassifier);
	
	switch (fMVAmode){
	case TMBDT:
	  mvaValue = (mvaValue - (-1.0))/2.0;
	  break;
	  
	case TMMLP:
	  mvaValue = (mvaValue - (-1.1))/2.2;
	  break;
	  
	case TMKNN:
	  cout<<"MVA KNN "<<mvaValue<<endl;
	  mvaValue = mvaValue;//-0.5; 
	  cout<<"MVA correction "<<mvaValue<<endl;
	  break;
	  
	default:
	  std::cout << "<ERROR:> NO classifier was selected." << std::endl;
	  break;
	}
	
	fTrack->Set(className,mvaValue);
	cout<<"Likelihood for the class "<<className<<"is :"<<mvaValue<<endl;
      }//End of for (int i = 0 ; i < fNCLASS; i++){
    }//End of else branch(if(fMVAmode))
  }// End of Loop through the Tracks 
}
ClassImp(PndGpidTaskLhe);
