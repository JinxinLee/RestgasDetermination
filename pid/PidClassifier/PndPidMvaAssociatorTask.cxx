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

ClassImp(PndPidMvaAssociatorTask)

//==========================================================
#if (DEBUG != 0)
// Function to use for debugging
void printResult(std::map<std::string,float>& res)
{
  std::cout << "\n\t================================== \n";
  for( std::map<std::string,float>::iterator ii=res.begin(); 
       ii != res.end(); ++ii)
  {
    std::cout <<"\t"     << (*ii).first 
	      << "\t=> " << (*ii).second
	      << '\n';
  }
  std::cout << "\n\t================================== \n";
}
#endif

//==========================================================

/**
 * Default Constructor.
 */
PndPidMvaAssociatorTask::PndPidMvaAssociatorTask()
  : FairTask("PndPidMvaAssociatorTaskSTD"),
    fNumNeigh(200), fScFact(0.8),
    fWeight(1.00), fClassifier(0),
    fMethodName("UNKNOWN_METHOD")
{
  std::cout << "<INFO> Call Default task constructor. " 
	    << "(PndPidMvaAssociatorTask)\n";
  // Init charged and neutral probab. containers.
  fPidChargedProb = new TClonesArray("PidMvaProb");
  //fPidNeutralProb = new TClonesArray("PndPidProbability");

  // Set Default path to the weight file  
  SetDefaultWeightsPath();
}

//___________________________________________________________
/**
 * Constructor.
 */
PndPidMvaAssociatorTask::PndPidMvaAssociatorTask(char const* name, char const* title)
  : FairTask(name),
    fNumNeigh(200), fScFact(0.8), fWeight(1.00),
    fClassifier(0), fMethodName("UNKNOWN_METHOD")
{
  std::cout << title << '\n';
  // Init charged and neutral probab. containers.
  fPidChargedProb = new TClonesArray("PidMvaProb");
  //fPidNeutralProb = new TClonesArray("PndPidProbability");

  // Set Default path to the weight file
  SetDefaultWeightsPath();
}

void PndPidMvaAssociatorTask::SetDefaultWeightsPath()
{
  fWeightsFileName  = std::string(getenv("VMCWORKDIR"));
  fWeightsFileName += std::string("/PndTools/MVA/PndMVAWeights/");
  std::cout<<"<INFO> Default Weights path is set to "
	   << fWeightsFileName
	   << '\n';
}

//___________________________________________________________
/**
 * Destructor.
 */
PndPidMvaAssociatorTask::~PndPidMvaAssociatorTask()
{
  // Clean-up allocated stuff.
  fManager->Write();
  
  if(fManager)
    delete fManager;
  
  if(fPidChargedCand)
    delete fPidChargedCand;

  if(fPidChargedProb)
    delete fPidChargedProb;

  //if(fPidNeutralCand)
  //delete fPidNeutralCand;  
  
  //if(fPidNeutralProb)
  //delete fPidNeutralProb;
  
  if(fMCTrack)
    delete  fMCTrack;

  if(fClassifier)
    delete fClassifier;
}

//___________________________________________________________
InitStatus PndPidMvaAssociatorTask::Init()
{
  std::cout << "<-I-> InitStatus PndPidMvaAssociatorTask::Init()\n";
  
  fManager = FairRootManager::Instance();
  
  // Get charged candidates.
  fPidChargedCand = (TClonesArray *)fManager->GetObject("PidChargedCand");
  
  if ( !fPidChargedCand)
  {
    std::cerr << "<ERROR> PndPidMvaAssociatorTask::Init: No PidChargedCand there!"
	      << std::endl;
    return kERROR;
  }

  // Get Neutral candidates.
  /*
    fPidNeutralCand = (TClonesArray *)fManager->GetObject("PidNeutralCand");
    
    if ( ! fPidNeutralCand)
    {
    std::cerr << "<ERROR> PndPidMvaAssociatorTask::Init: No PidNeutralCand there!"
    << std::endl;
    return kERROR;
    }
  */
  
  Register();
  
  std::cout << "<INFO> Using weight file  "
	    << fWeightsFileName
	    << "\n<INFO> Init classifiers.\n";
  
  // Init Classifier object
  switch(fMethodType)
  {
  case TMVA_MLP:// Multi label MLP classifier from TMVA.
  {
    PndMultiClassMlpClassify* TmvaMlpCls = new PndMultiClassMlpClassify(fWeightsFileName, fClassNames,
                                                                        fVarNames);
    if(!TmvaMlpCls)
    {
      std::cerr << "<Error> Failed to initialize TMVA_MLP classifier."
		<< std::endl;
      return kERROR;
    }
    // Init
    TmvaMlpCls->Initialize();
    
    //fClassifier = dynamic_cast<PndMultiClassMlpClassify*>(TmvaMlpCls);
    fClassifier = TmvaMlpCls;
    std::cout << "<INFO> TMVA_MLP initialized using " << fWeightsFileName << '\n';
    fMethodName = "TMVAMLP";
  }
  break;
  
  case TMVA_BDT:// Multi label BDT classifier from TMVA.
  {
    PndMultiClassBdtClassify* TmvaBdtCls = new PndMultiClassBdtClassify(fWeightsFileName, fClassNames,
                                                                        fVarNames);
    if(!TmvaBdtCls)
    {
      std::cerr << "<Error> Failed to initialize TMVA_BDT classifier."
		<< std::endl;
      return kERROR;
    }
    // INIT
    TmvaBdtCls->Initialize();
    
    //fClassifier = dynamic_cast<PndMultiClassBdtClassify*>(TmvaBdtCls);
    fClassifier = TmvaBdtCls;
    std::cout << "<INFO> TMVA_BDT initialized using " << fWeightsFileName << '\n';
    fMethodName = "TMVABDT";
  }
  break;
  
  case LVQ:
  {
    PndLVQClassify* LvqCls = new PndLVQClassify(fWeightsFileName, fClassNames, fVarNames);
    
    if(!LvqCls)
    {
      std::cerr << "<Error> Failed to initialize LVQ classifier."
		<< std::endl;
      return kERROR;
    }
    // Init
    LvqCls->Initialize();
    
    //fClassifier = dynamic_cast<PndMvaClassifier*>(LvqCls);
    fClassifier = LvqCls;
    std::cout << "<INFO> LVQ initialized using " << fWeightsFileName << '\n';
    fMethodName = "LVQ";
  }
  break;
  
  case KNN:
  default:
  {
    PndKnnClassify* KnnCls = new PndKnnClassify(fWeightsFileName, fClassNames, fVarNames);
    
    if(!KnnCls)
    {
      std::cerr << "<Error> Failed to initialize KNN classifier."
		<< std::endl;
      return kERROR;
    }
    
    // Set parameters.
    KnnCls->SetEvtParam(fScFact, fWeight);
    KnnCls->SetKnn(fNumNeigh);
    KnnCls->Initialize();
    
    //fClassifier = dynamic_cast<PndMvaClassifier*>(KnnCls);
    fClassifier = KnnCls;
    std::cout << "<INFO> KNN initialized using " << fWeightsFileName << '\n';

    fMethodName = "KNN";
  }
  break;
  }// End of switch(fMethodType)
  
  std::cout << "<INFO> PndPidMvaAssociatorTask::Init: Success!\n";
  return kSUCCESS;
}

//______________________________________________________
void PndPidMvaAssociatorTask::SetParContainers()
{}

void PndPidMvaAssociatorTask::SetClassifier(std::string const& methodNameStr)
{
  fMethodName = methodNameStr;

  if(methodNameStr == "KNN")
  {
    fMethodType = KNN;
  }
  else if(methodNameStr == "LVQ")
  {
    fMethodType = LVQ;
  }
  else if(methodNameStr == "TMVA_MLP")
  {
    fMethodType = TMVA_MLP;
  }
  else if(methodNameStr == "TMVA_BDT")
  {
    fMethodType = TMVA_BDT;
  }
  else
  {
    std::cerr << "<ERROR> Unknown Method."
	      << std::endl;
  }
};
//______________________________________________________
void PndPidMvaAssociatorTask::Exec(Option_t* option)
{
  if (fPidChargedProb->GetEntriesFast() != 0)
  {
    fPidChargedProb->Delete();
  }

#if (DEBUG != 0)
  std::cout << "<INFO> Call to Exec with " << option
	    << '\n';
#endif

  if(fVerbose > 1)
  {
    std::cout << "-I- Start PndPidMvaAssociatorTask.\n";
  }

  // Get the charged Candidates
  for(int i = 0; i < fPidChargedCand->GetEntriesFast(); i++)
  {
    PndPidCandidate* pidcand = (PndPidCandidate*)fPidChargedCand->At(i);
    TClonesArray& pidRef = *fPidChargedProb;

    // initializes with zeros
    PndPidProbability* prob = new(pidRef[i]) PndPidProbability();
    
    if(fVerbose > 1)
    {
      std::cout << "-I- PndPidMVAAssociatorTask Ch BEFORE  "
		<< pidcand->GetLorentzVector().M()
		<< '\n';
    }
    // Classify
    DoPidMatch(*pidcand, *prob);
    
    if(fVerbose > 1)
    {
      std::cout << "-I- PndPidMVAAssociatorTask Ch AFTER "
		<< pidcand->GetLorentzVector().M()
		<< '\n';
    }
  }
  
  // Get the Neutral Candidates
  /*
    for(int i = 0; i < fPidNeutralCand->GetEntriesFast(); i++)
    {
    PndPidCandidate* pidcand = (PndPidCandidate*)fPidNeutralCand->At(i);
    TClonesArray& pidRef = *fPidNeutralProb;
    
    // initializes with zeros
    PndPidProbability* prob = new(pidRef[i]) PndPidProbability();
    
    // Classify
    DoPidMatch(*pidcand, *prob);
    }
  */
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
  // Perform Recognition.
  fClassifier->GetMvaValues( *evtPidData, out);
  
  delete evtPidData;

#if (DEBUG != 0)
  std::cout << "****************************************************\n"
	    << "Momentum " << (pidcand.GetMomentum()).Mag()
	    << "\nGetEnergy " << pidcand.GetEnergy()
	    << "\nEMC " << pidcand.GetEmcCalEnergy()
	    << "\nEMC/P "
	    << (pidcand.GetEmcCalEnergy())/((pidcand.GetMomentum()).Mag())
	    << "\nEMCZ20 " << pidcand.GetEmcClusterZ20()
	    << "\nEMCZ53 " << pidcand.GetEmcClusterZ53()
	    << "\nEMCLAT " << pidcand.GetEmcClusterLat()
            << "\nEmcE1 "  << pidcand.GetEmcClusterE1()
            << "\nEmcE9 "  << pidcand.GetEmcClusterE9()
            << "\nEmcE25 " << pidcand.GetEmcClusterE25()
	    << "\nSTT " << pidcand.GetSttMeanDEDX()
	    << "\nMVD " << pidcand.GetMvdDEDX()
	    << "\nDRC_TC " << pidcand.GetDrcThetaC()
            << '\n';
  printResult(out);
  std::cout << "====================================================\n";
#endif

  // Set probs.
  for(size_t i = 0; i < fClassNames.size(); i++)
  {
    std::string name = fClassNames[i];
    
    if(name == "electron")
    {
      prob.SetElectronPdf(out[name]);
    }
    else if(name == "muon")
    {
      prob.SetMuonPdf(out[name]);
    }
    else if(name == "pion")
    {
      prob.SetPionPdf(out[name]);
    }
    else if(name == "kaon")
    {
      prob.SetKaonPdf(out[name]);
    }
    else if(name == "proton")
    {
      prob.SetProtonPdf(out[name]);
    }
  }
}

std::vector<float> const* PndPidMvaAssociatorTask::PrepareEvtVect(PndPidCandidate const& pidcand) const
{
  std::vector<float>* vect = new std::vector<float>();
  float mom = (pidcand.GetMomentum()).Mag();
  
  for(size_t i = 0; i < fVarNames.size(); i++)
  {  
    if( fVarNames[i] == "p" )
    {
      vect->push_back((pidcand.GetMomentum()).Mag());
    }
    // This needs to be fixed (exception??)
    
    else if(fVarNames[i] == "emc")
    {
      if(mom != 0.0)
      { // E/p
	vect->push_back( (pidcand.GetEmcCalEnergy())/mom);
      }
      else
      {
	std::cerr << "<ER-I> p = " << mom << std::endl;
	vect->push_back(pidcand.GetEmcCalEnergy());
      }
    }    
    //======== Zernike & moments
    else if(fVarNames[i] == "z20")
    {
      vect->push_back(pidcand.GetEmcClusterZ20());
    }
    else if(fVarNames[i] == "z53")
    {
      vect->push_back(pidcand.GetEmcClusterZ53());
    }
    // Cluster Second lat. moment
    else if(fVarNames[i] == "lat")
    {
      vect->push_back(pidcand.GetEmcClusterLat());
    }
    // Cluster Ex parameters.
    else if(fVarNames[i] == "e1")
    {
      vect->push_back(pidcand.GetEmcClusterE1());
    }
    else if(fVarNames[i] == "e9")
    {
      vect->push_back(pidcand.GetEmcClusterE9());
    }
    else if(fVarNames[i] == "e25")
    {
      vect->push_back(pidcand.GetEmcClusterE25());
    }
    // ==========================
    else if(fVarNames[i] == "stt")
    {
      vect->push_back(pidcand.GetSttMeanDEDX());
    }
    else if(fVarNames[i] == "mvd")
    {
      vect->push_back(pidcand.GetMvdDEDX());
    }    
    else if(fVarNames[i] == "thetaC")
    {
      vect->push_back(pidcand.GetDrcThetaC());
    } 
  }
  return vect;
}

//_________________________________________________________________
void PndPidMvaAssociatorTask::Register()
{
  std::string tcaName = "Pid" + fMethodName + "MvaProb";
  //---
  FairRootManager::Instance()->Register(tcaName.c_str(),"Pid",
					fPidChargedProb, kTRUE); 
  //FairRootManager::Instance()->Register("PidMvaNeutralProbability","Pid", 
  //					fPidNeutralProb, kTRUE);
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
