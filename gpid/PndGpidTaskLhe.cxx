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

// Default constructor
PndGpidTaskLhe::PndGpidTaskLhe()
{}

// Default destructor
PndGpidTaskLhe::~PndGpidTaskLhe()
{
  fVarNameArray.clear();
  fClassNameArray.clear();
  m_varVec.clear();
  
  // Delete all TMVA::Reader objects
  for(int i = 0 ; i < fNVAR ; i++){
    delete reader[i];
  }
  if(m_lvq){
    delete m_lvq;
  }
  if(m_knn){
    delete m_knn;
  }
}
/*
 * Init call of the task inherited from the FairTask. Registers the
 * output of the track and get the inputs of the track. Input is
 * LhePidTrack Output is PndPidCand
*/
InitStatus PndGpidTaskLhe::Init()
{
  FairRootManager* ioman = FairRootManager::Instance();
  if ( !ioman ){
    std::cout << "-E- PndGpidTaskLhe::Init: "
	      << "RootManager not instantised!" << std::endl;
    return kFATAL;
  }
  
  fArrPid = new TClonesArray("PndPidCand");
  ioman->Register("PndPidCand","Geane", fArrPid, kTRUE);
  
  fPidTrackCand = (TClonesArray*) ioman->GetObject("LhePidTrack");
  if ( !fPidTrackCand ) {
    cout << "-W- PndGpidTaskLhe::Init: "
         << "No SamEvt array!" << endl;
    return kERROR;
  }
  
  Config();
  AddVar();
  BookingMVA();
  
  std::cout << "-I- Finished Init of PndGpidTask." << std::endl;
  
  return kSUCCESS;
}

/* 
 * Reading the config file and set the number of classes and
 * variables.
 */
void PndGpidTaskLhe::Config()
{
  ifstream inFile;
  string sbuff,sfName,small_buff,vName;
  char buff[512];
  sfName = fAPPNAME + ".dat";
  
  inFile.open(sfName.c_str(),ios::in);
  
  if (!inFile.is_open()){
    std::cout << "\n\n\t<ERROR>: Could not open file "
	      << sfName <<"\n\n"<< std::endl;
    exit(0);
  }
  
  // Set the number of variables
  inFile.getline(buff,512);
  fNVAR = (int) atoi(buff);
  
  // Set the number of Classes
  inFile.getline(buff,512);
  fNCLASS = (int) atoi(buff);
  inFile.getline(buff,512);
  
  cout<<"    <INFO:> " << fNVAR 
      << " variables and "<< fNCLASS 
      << " classes" << endl;
  sbuff = buff;
  
  // Fetch the variable names from the config file.
  std::cout << "\n<INFO:> The following variables are used: ";
  while(1){
    size_t prompt = sbuff.find(":");
    sbuff = sbuff.substr(prompt+1,sbuff.size());
    if( sbuff.size() == 0) break;
    vName = sbuff.substr(0,sbuff.find(":"));
    fVarNameArray.push_back(vName); 
    cout<< vName << ", "; 
  }
  std::cout << std::endl;

  inFile.getline(buff,512);
  sbuff = buff;
  
  // Fetch the names of available classes.
  std::cout << "\n<INFO:> The following Classes are available: ";
  while(1){
    size_t prompt = sbuff.find(":");
    sbuff = sbuff.substr(prompt+1,sbuff.size());
    if( sbuff.size() == 0) break;
    vName = sbuff.substr(0,sbuff.find(":"));
    fClassNameArray.push_back(vName); 
    cout<< vName << ", "; 
  }
  std::cout << std::endl;
}

/* 
 * Adding the variable which were used during the training phase.
 * These names are stored locally in a file with the same name as the
 * application name.
 */
void PndGpidTaskLhe::AddVar()
{
  // Init a container to hold the variables.
  m_varVec = std::vector<float>(fNVAR, 0.00);

  // Initialize a TMVA::Reader for each class.
  for(int i = 0 ; i < fNCLASS ; i++){
    TMVA::Reader* rd = new TMVA::Reader();
    reader.push_back(rd);
  }
  
  // Bind the variables to the reader  of each class.
  for ( int i = 0 ; i < fNVAR ; i++  ){
    TString varName;
    varName = fVarNameArray.at(i);
    //cout << varName << endl;// Print debug info
    for ( int j = 0; j < fNCLASS ; j++){
      reader[j]->AddVariable(varName, &(m_varVec[i]));
    }
  }
}

/* 
 * Books the algorithm we want to use for classification.
*/
void PndGpidTaskLhe::BookingMVA()
{
  // Select and book the classifier. The first set are all placed in
  // PndTools and the second set is implemented in TMVA package.

  if(fMVAmode == MulClsKNN || fMVAmode == LVQ1 || fMVAmode == LVQ21)
  {
    switch (fMVAmode){
    case MulClsKNN:// Multi class KNN. In PndTools.
      m_knn = new PndKnnClassify(M_InFileName, fClassNameArray, fVarNameArray);
      m_knn->InitKNN();
      std::cout << "<INFO KNN> This implementation needs to be tested." 
		<< std::endl;
      break;
      
    case LVQ1:// LVQ1 & LVQ2.1. In PndTools.
    case LVQ21:
      m_lvq = new PndLVQClassify(M_InFileName, fClassNameArray, fVarNameArray);
      std::cout << "<INFO LVQ> This implementation needs to be tested." 
		<< std::endl;
      break;
   
    default:
      std::cout << "<ERROR:> NO classifier was selected." << std::endl;
      break;
    }
  }
  else{// TMVA classifiers  
    for (int i = 0 ; i < fNCLASS ; i++  )
    {
      string anaFile;
      
      switch (fMVAmode){
      case TMBDT:// TMVA BDT
	anaFile =  fDIR + fAPPNAME + fClassNameArray.at(i) + "_BDT.weights.txt";
	reader[i]->BookMVA("BDT method", anaFile );
	fClassifier = "BDT method";
	break;
	
      case TMMLP:// TMVA MLP
	anaFile =  fDIR + fAPPNAME + fClassNameArray.at(i) + "_MLP.weights.txt";
	reader[i]->BookMVA("MLP method", anaFile );
	fClassifier = "MLP method";
	break;
	
      case TMKNN:// TMVA KNN
	anaFile =  fDIR + fAPPNAME + fClassNameArray.at(i) + "_KNN.weights.txt";
	reader[i]->BookMVA("KNN method", anaFile );
	fClassifier = "KNN method";
	break;
	
      default:
	std::cout << "<ERROR:> NO classifier was selected." << std::endl;
	break;
      }
    }
  }
}

// Exec call of the task
void PndGpidTaskLhe::Exec(Option_t* opt)
{
  if(!fArrPid){
    std::cout << "<ERROR>: Fatal Error, PIDTask::Exec."
	      << std::endl;
    return;
  }
  
  fArrPid->Delete();
  
  TClonesArray& clref2 = *fArrPid;
  Int_t size2 = clref2.GetEntriesFast();
  PndPidCand *fTrack= new(clref2[size2]) PndPidCand(); 
  
  // Loop through the Tracks.
  std::cout << "\n Number of available tracks " 
	    << fPidTrackCand->GetEntriesFast()
	    << std::endl;
  for (int k = 0; k < fPidTrackCand->GetEntriesFast(); k++){
    // Select the current track
    PndLhePidTrack *pid = (PndLhePidTrack *) fPidTrackCand->At(k);
    std::string varName;

    int skip = 0 ;
    // DEBUG info
    std::cout << " <INFO:> Track number is "<< k << std::endl;
    
    // Reading the input variable
    for (int i = 0 ; i< fNVAR; i++){
      varName = fVarNameArray.at(i);
      
      if (varName == "tof") {
	m_varVec[i] = pid->GetTof();
	
	if (m_varVec[i] <= 0) {skip = 1;}
	
	fTrack->SetVarVal(varName, m_varVec[i]);
      }

      if (varName == "emc"){
	m_varVec[i] = pid->GetEmcELoss()/pid->GetP();
	
	if (m_varVec[i] <= 0 ){skip = 1;}
	
	fTrack->SetVarVal(varName,m_varVec[i]);
      }
      
      if (varName == "stt"){

	m_varVec[i] = pid->GetSttDEDX();
	
	if ( m_varVec[i] <= 0 || isnan(m_varVec[i])){ skip = 1;}
	
	fTrack->SetVarVal(varName,m_varVec[i]);
      }

      if (varName == "mvd"){
	m_varVec[i] = pid->GetMvdDEDX();

	if (m_varVec[i] <= 0 ){ skip = 1;}

	fTrack->SetVarVal(varName,m_varVec[i]);
      }

      if (varName == "thetaC"){
	m_varVec[i] = pid->GetDrcThetaC();
	
	if (m_varVec[i] <= 0 ){ skip = 1;}

	fTrack->SetVarVal(varName,m_varVec[i]);
      }
      
      if (varName == "p"){
	m_varVec[i] = pid->GetMomentum().Mag();
	
	if (m_varVec[i] <= 0 ){ skip = 1;}

	fTrack->SetVarVal(varName,m_varVec[i]);
      }
      // Print Debug information
      std::cout << varName << " = " << m_varVec[i] << " ";
    }//End of Reading variables.
    std::cout <<  std::endl;

    // Check if something is wrong, skip the track
    if (skip != 0 ){
      std::cerr << "\t<WARNING:> Skipping Track " 
		<< k << ".\n\tOne or more values are not valid numbers."
		<<"\n\tThe track itself is stored but not classified.\n"
		<< std::endl;
      continue;
      // The track is stored but we will not classify the bad track
    }
    // Select if we want to use TMVA or a MVA from PndTools.
      if(fMVAmode == MulClsKNN || fMVAmode == LVQ1 || fMVAmode == LVQ21){
	std::map<std::string,float> res;

	switch(fMVAmode){
	case MulClsKNN:
	  m_knn->Classify(m_varVec, M_KNN, res);
	  // Copy the results into fTrack
	  for( std::map<std::string,float>::iterator ii=res.begin(); 
               ii != res.end(); ++ii){
            fTrack->SetClsVal((*ii).first, (*ii).second);
          }
	  break;

	case LVQ1:
	case LVQ21:
	  m_lvq->Classify(m_varVec, res);
	  // Copy the results into fTrack
	  for( std::map<std::string,float>::iterator ii=res.begin(); 
               ii != res.end(); ++ii){
            fTrack->SetClsVal((*ii).first, (*ii).second);
          }
	  break;
	  
	default:
	  std::cout << "Unknown classifier" << std::endl;
	}
      }
      else{ // We want to renormalize TMVA output.
	
	float mvaValue;// Store MVA output.
	for (int i = 0 ; i < fNCLASS; i++){
	  std::string className;
	  
	  className = fClassNameArray.at(i);

	  mvaValue = reader[i]->EvaluateMVA(fClassifier);
	  
	  switch (fMVAmode){
	  case TMBDT:
	    mvaValue = (mvaValue - (-1.0))/2.0;
	    break;
	    
	  case TMMLP:
	    mvaValue = (mvaValue - (-1.1))/2.2;
	    break;
	    
	  case TMKNN:
	    mvaValue = mvaValue; 
	    break;
	    
	  default:
	    std::cout << "<ERROR:> NO classifier was selected." << std::endl;
	    break;
	  }
	  
	  fTrack->SetClsVal(className,mvaValue);
	  std::cout << "======== DEBUG INFO =======" << std::endl;
	  cout << "Likelihood for the class "
	       << className << " is :"<< mvaValue << endl << std::endl;
	}//End of for (int i = 0 ; i < fNCLASS; i++){
      }//End of else branch(if(fMVAmode))
  }// End of Loop through the Tracks 
}
ClassImp(PndGpidTaskLhe);
