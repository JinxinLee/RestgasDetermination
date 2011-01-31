/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  pid classifier                    *
 *                                    *
 * Created: 23-03-2010                *
 * Modified:                          *
 *                                    *
 * ************************************/
#include "PndPidMvaAssociatorTask.h"

#define DEBUG 0

//==========================================================
#ifdef DEBUG
// Function to use for debugging
void printResult(std::map<std::string,float>& res){
  std::cout << "\n\t================================== \n";
  for( std::map<std::string,float>::iterator ii=res.begin(); 
       ii != res.end(); ++ii){
    std::cout <<"\t" << (*ii).first 
	      << "\t=> " << (*ii).second << std::endl;
  }
  std::cout << "\n\t================================== \n";
}
#endif

//==========================================================

/**
 * Default Constructor.
 */
PndPidMvaAssociatorTask::PndPidMvaAssociatorTask()
  : FairTask("PndPidMvaAssociatorTaskSTD")
{
  std::cout << "<INFO> Call Default task constructor. " 
	    << "(PndPidAssociatorTask)\n";
  // Init charged and neutral probab. containers.
  fPidChargedProb = new TClonesArray("PndPidProbability");
  fPidNeutralProb = new TClonesArray("PndPidProbability");
  
  fNumNeigh = 200;
  SetDefaultWeightsPath();
}

//___________________________________________________________
/**
 * Constructor.
 */
PndPidMvaAssociatorTask::PndPidMvaAssociatorTask(const char *name, const char *title)
  : FairTask(name)
{
  std::cout << title << std::endl;
  // Init charged and neutral probab. containers.
  fPidChargedProb = new TClonesArray("PndPidProbability");
  fPidNeutralProb = new TClonesArray("PndPidProbability");
  
  fNumNeigh = 200;
  SetDefaultWeightsPath();
}

void PndPidMvaAssociatorTask::SetDefaultWeightsPath()
{
  fWeightsFileName  = std::string(getenv("VMCWORKDIR"));
  fWeightsFileName += std::string("/pid/PidClassifier/PndMVAWeights/");
}

//___________________________________________________________
/**
 * Destructor.
 */
PndPidMvaAssociatorTask::~PndPidMvaAssociatorTask()
{
  // Clean-up allocated stuff.
  fManager->Write();
  
  if(fManager) delete fManager;
  if(fPidChargedCand) delete fPidChargedCand;
  if(fPidNeutralCand) delete fPidNeutralCand;
  if(fPidChargedProb) delete fPidChargedProb;
  if(fPidNeutralProb) delete fPidNeutralProb;
  if(fMCTrack) delete  fMCTrack;
  if(fKnnCls) delete fKnnCls;
}

//___________________________________________________________
InitStatus PndPidMvaAssociatorTask::Init()
{
  std::cout << "InitStatus PndPidAssociatorTask::Init()\n";
  
  fManager = FairRootManager::Instance();
  
  // Get charged candidates.
  fPidChargedCand = (TClonesArray *)fManager->GetObject("PidChargedCand");
  
  if ( !fPidChargedCand){
    std::cerr << "-E- PndPidMvaAssociatorTask::Init: No PidChargedCand there!"
	      << std::endl;
    return kERROR;
  }

  // Get Neutral candidates.
  fPidNeutralCand = (TClonesArray *)fManager->GetObject("PidNeutralCand");
  
  if ( ! fPidNeutralCand){
    std::cerr << "-E- PndPidMvaAssociatorTask::Init: No PidNeutralCand there!"
	      << std::endl;
    return kERROR;
  }
  // Get track array.
  /*
    fMCTrack = (TClonesArray*) fManager->GetObject("MCTrack");
    if (!fMCTrack){
    std::cout << "-I- PndPidAssociatorTask::Init: No MC Track array there!" << std::endl;
    return kERROR;
    }
  */
  Register();
  
  std::cout << "<INFO> Using weight file  " << fWeightsFileName
	    << "\n<INFO> Init classifiers.\n";
  
  // Init Classifier object
  fKnnCls = new PndKnnClassify(fWeightsFileName, fClassNames, fVarNames);
  
  if(!fKnnCls){
    std::cerr << "<Error> Failed to initialize classifier." << std::endl;
    return kERROR;
  }
  
  //Fixme Fixme
  fKnnCls->SetEvtParam(0.8, 1.0);
  fKnnCls->SetKnn(fNumNeigh);
  fKnnCls->InitKNN();
  
  std::cout << "-I- PndPidMvaAssociatorTask::Init: Success!\n";
  return kSUCCESS;
}

//______________________________________________________
void PndPidMvaAssociatorTask::SetParContainers()
{}

//______________________________________________________
void PndPidMvaAssociatorTask::Exec(Option_t* option)
{
  if (fPidChargedProb->GetEntriesFast() != 0){
    fPidChargedProb->Delete();
  }
  std::cout << "<INFO> Call to Exec with " << option << std::endl;  
  
  if(fVerbose > 1){
    std::cout << "-I- Start PndPidAssociatorTask.\n";
  }

  // Get the charged Candidates
  for(int i = 0; i < fPidChargedCand->GetEntriesFast(); i++){
    PndPidCandidate* pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
    TClonesArray& pidRef = *fPidChargedProb;
    PndPidProbability* prob = new(pidRef[i]) PndPidProbability();// initializes with zeros
    if(fVerbose > 1){
      std::cout << "-I- PndPidAssociatorTask Ch BEFORE  "
		<< pidcand->GetLorentzVector().M() << std::endl;
    }
    
    DoPidMatch(*pidcand, *prob);
    
    if(fVerbose > 1){
      std::cout << "-I- PndPidAssociatorTask Ch AFTER "
		<< pidcand->GetLorentzVector().M() << std::endl;
    }
  }
  
  // Get the Neutral Candidates
  for(int i = 0; i < fPidNeutralCand->GetEntriesFast(); i++){
    PndPidCandidate* pidcand = (PndPidCandidate*)fPidNeutralCand->At(i);
    TClonesArray& pidRef = *fPidNeutralProb;
    PndPidProbability* prob = new(pidRef[i]) PndPidProbability();// initializes with zeros
    
    DoPidMatch(*pidcand, *prob);
  }
}

/**
 * Performs the actual classification.
 *@param pidcand Current pid candidate to be classified.
 *@param prob    Output probabilities.
 */
void PndPidMvaAssociatorTask::DoPidMatch(PndPidCandidate& pidcand,
					 PndPidProbability& prob)
{
  std::map<std::string, float> out;
  std::vector<float> const* evtPidData = PrepareEvtVect(pidcand);
  fKnnCls->GetMvaValues( *evtPidData, out);
  delete evtPidData;

#ifdef DEBUG
  std::cout << "****************************************************"<< std::endl;
  std::cout << "Momentum " << (pidcand.GetMomentum()).Mag() << std::endl;
  std::cout << "GetEnergy " << pidcand.GetEnergy() << std::endl;
  std::cout << "EMC " << pidcand.GetEmcCalEnergy() << std::endl;
  std::cout << "EMC/P " 
	    << (pidcand.GetEmcCalEnergy())/((pidcand.GetMomentum()).Mag()) 
	    << std::endl;
  std::cout << "STT " << pidcand.GetSttMeanDEDX() << std::endl;
  std::cout << "MVD " << pidcand.GetMvdDEDX() << std::endl;
  std::cout << "DRC_TC " << pidcand.GetDrcThetaC() << std::endl;
  std::cout << "TPC " << pidcand.GetTpcMeanDEDX() << std::endl;
  std::cout << "EMCZ20 " << pidcand.GetEmcClusterZ20() << std::endl;
  std::cout << "EMCZ53 " << pidcand.GetEmcClusterZ53() << std::endl;
  std::cout << "EMCLAT " << pidcand.GetClusterLat() << std::endl;
  printResult(out);
  std::cout << "===================================================="<< std::endl;
#endif

  // Set probs.
  for(size_t i = 0; i < fClassNames.size(); i++){
    std::string name = fClassNames[i];
    
    if(name == "electron")
      prob.SetElectronPdf(out[name]);
    
    if(name == "muon")
      prob.SetMuonPdf(out[name]);
    
    if(name == "pion")
      prob.SetPionPdf(out[name]);
    
    if(name == "kaon")
      prob.SetKaonPdf(out[name]);
    
    if(name == "proton")
      prob.SetProtonPdf(out[name]);
  }
}

std::vector<float> const* PndPidMvaAssociatorTask::PrepareEvtVect(PndPidCandidate const& pidcand) const
{
  std::vector<float>* vect = new std::vector<float>();
  float mom = (pidcand.GetMomentum()).Mag();
  
  for(size_t i = 0; i < fVarNames.size(); i++){
    
    if(fVarNames[i] == "p"){
      vect->push_back((pidcand.GetMomentum()).Mag());
    }
    // This needs to be fixed (exception??)
    
    if(fVarNames[i] == "emc"){
      if(mom != 0.0){
	vect->push_back( (pidcand.GetEmcCalEnergy())/mom);
      }
      else{
	vect->push_back(pidcand.GetEmcCalEnergy());
      }
    }
    
    //======== Zernike & moments
    if(fVarNames[i] == "z20"){
      vect->push_back(pidcand.GetEmcClusterZ20());
    }
    
    if(fVarNames[i] == "z53"){
      vect->push_back(pidcand.GetEmcClusterZ53());
    }
    
    // Cluster Second lat. moment
    if(fVarNames[i] == "lat"){
      vect->push_back(pidcand.GetClusterLat());
    }
    
    // ==========================
   
    if(fVarNames[i] == "stt"){
      vect->push_back(pidcand.GetSttMeanDEDX());
    }
    
    if(fVarNames[i] == "mvd"){
      vect->push_back(pidcand.GetMvdDEDX());
    }
    
    if(fVarNames[i] == "tpc"){
      vect->push_back(pidcand.GetTpcMeanDEDX());
    }
    
    if(fVarNames[i] == "thetaC"){
      vect->push_back(pidcand.GetDrcThetaC());
    } 
  }
  return vect;
}

//_________________________________________________________________
void PndPidMvaAssociatorTask::Register()
{
  //---
  FairRootManager::Instance()->Register("PidChargedProbability","Pid",
					fPidChargedProb, kTRUE); 
  FairRootManager::Instance()->Register("PidNeutralProbability","Pid", 
					fPidNeutralProb, kTRUE);
}

//_________________________________________________________________
void PndPidMvaAssociatorTask::Finish()
{
  //  FairRootManager* ioman = FairRootManager::Instance();
  //  ioman->W
}
//_________________________________________________________________
void PndPidMvaAssociatorTask::Reset()
{}

ClassImp(PndPidMvaAssociatorTask)
