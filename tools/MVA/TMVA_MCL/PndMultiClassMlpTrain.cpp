/* ***************************************
 * MultiClass MLP  Training functions    *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */

#include "PndMultiClassMlpTrain.h"

/*
 * Constructor.
 *@param InPut The file containig the event data.
 *@param ClassNames Labels of the classes to be used.
 *@param VarNames  The name of the involved variables(features).
 *@param trim If the data set needs to be trimmed.
 */
PndMultiClassMlpTrain::PndMultiClassMlpTrain(std::string const& InPut,
					     std::vector<std::string> const& ClassNames, 
					     std::vector<std::string> const& VarNames,
					     bool trim)
  : PndMvaTrainer(InPut, ClassNames, VarNames, trim),
    m_factory(0),
    EvalFile (0),
    m_JName(""),
    m_transform(""),
    m_MlpOptions(""),
    m_evalFileName(""),
    m_weightDirName("PndMVAWeights"),
    m_Evaluate(false)
{}

/*
 * Destructor.
 */
PndMultiClassMlpTrain::~PndMultiClassMlpTrain()
{
  if (EvalFile)
  {
    EvalFile->Close();
    delete EvalFile;
    EvalFile = 0;
  }
  
  if(m_factory)
  {
    delete m_factory;
    m_factory = 0;
  }
}

/*
 * Train the classifier.
 */
void PndMultiClassMlpTrain::Train()
{
  std::string const& inFileName = m_dataSets.GetInFileName();
  std::vector<PndMvaClass> const& labels = m_dataSets.GetClasses();

  //Initialize();
  //AddVariables();

  TFile InFile (inFileName.c_str(), "READ");

  for(size_t cl = 0; cl < labels.size(); ++cl)
  {
    char const* curName = (labels[cl].Name).c_str();
    m_factory->AddTree( (TTree*)InFile.Get(curName), curName);
  }

  m_factory->PrepareTrainingAndTestTree( "", "SplitMode=Random:NormMode=NumEvents:!V" );
  m_factory->BookMethod(TMVA::Types::kMLP, "MLP_Method", m_MlpOptions.c_str());

  // Train MVAs using the set of training events
  m_factory->TrainAllMethods();
  
  // ---- Evaluate all MVAs using the set of test events
  m_factory->TestAllMethods();
  
  // ----- Evaluate and compare performance of all configured MVAs
  if(m_Evaluate)
  {
    m_factory->EvaluateAllMethods();
  }
  // Cleaning
  InFile.Close();
}

/*
 *Initialize Classifier and data structures.
 */
void PndMultiClassMlpTrain::Initialize()
{
  InitMlp();
  AddVariables();
}

// Initialize mlp object and set the options.
void PndMultiClassMlpTrain::InitMlp()
{
  PndMvaTrainer::SetAppType(TMVATRAIN);
  // PndMvaTrainer::Initialize();

  if(m_JName == "")
  {
    m_JName = "PANDA";
    std::cerr << "<WARNING> No job name was specified. We will set this to:\n\t"
	      << m_JName << '\n';
  }
  if( m_transform == "")
  {
    m_transform = "Transformations=I;N;D;P;G";
    std::cerr << "<WARNING> No transformation was specified. We will set this to:\n\t"
	      << m_transform << '\n';
  }
  if (m_MlpOptions == "")
  {
    m_MlpOptions = "!H:!V:NeuronType=tanh:NCycles=100:HiddenLayers=N+5:TestRate=5:EstimatorType=MSE";
    std::cerr << "<WARNING> No train options were specified. We will set this to:\n\t"
	      << m_MlpOptions <<'\n';
  }
  if(m_evalFileName == "")
  {
    m_evalFileName = m_JName + "_EvaluationFile.root";
    std::cerr << "<WARNING> No evaluation file name was specified. We will set this to:\n\t"
	      << m_evalFileName <<'\n';
  }
  // Set the Weight files directory name.
  (TMVA::gConfig().GetIONames()).fWeightFileDir = m_weightDirName.c_str();

  std::string factOpt = "!V:!Silent:Color:DrawProgressBar:" + m_transform + ":AnalysisType=Multiclass";

  // Evaluation File. Needed by TMVA factory
  EvalFile  = new TFile(m_evalFileName.c_str(), "RECREATE");
  // Create and init the factory object.
  m_factory = new TMVA::Factory(m_JName.c_str(), EvalFile, factOpt.c_str());
}

// Add the variables to the TMVA factory object.
void PndMultiClassMlpTrain::AddVariables()
{
  std::vector<PndMvaVariable> const& variables = m_dataSets.GetVars();
  // Add variables to the TMVA factory.
  for(size_t v = 0; v < variables.size(); ++v)
  {
    m_factory->AddVariable( (variables[v].Name).c_str(), 'F' );
  }
}

/**
 * Store weights in the output File. If output file name is not
 * specified, then write nothing.
 */
void PndMultiClassMlpTrain::storeWeights()
{
  /**
   * Does not need implementation. TMVA does this.
   */
}
