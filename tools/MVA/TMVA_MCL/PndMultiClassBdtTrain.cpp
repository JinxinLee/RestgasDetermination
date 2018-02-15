/* ***************************************
 * MultiClass BDTG  Training functions   *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */

#include "PndMultiClassBdtTrain.h"

/*
 * Constructor.
 *@param InPut The file containig the event data.
 *@param ClassNames Labels of the classes to be used.
 *@param VarNames  The name of the involved variables(features).
 *@param trim If the data set needs to be trimmed.
 */
PndMultiClassBdtTrain::PndMultiClassBdtTrain(std::string const& InPut,
					     std::vector<std::string> const& ClassNames, 
					     std::vector<std::string> const& VarNames,
					     bool trim)
  : PndMvaTrainer(InPut, ClassNames, VarNames, trim),
    m_factory(0),
    EvalFile (0),
    m_JName(""),
    m_transform(""),
    m_BdtOptions(""),
    m_evalFileName(""),
    m_weightDirName("PndMVAWeights"),
    m_Evaluate(false)
{}

/*
 * Destructor.
 */
PndMultiClassBdtTrain::~PndMultiClassBdtTrain()
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
void PndMultiClassBdtTrain::Train()
{
  std::string const& inFileName = m_dataSets.GetInFileName();
  std::vector<PndMvaClass> const& labels = m_dataSets.GetClasses();
  
  //
  TFile InFile (inFileName.c_str(), "READ");

  for(size_t cl = 0; cl < labels.size(); ++cl)
  {
    char const* curName = (labels[cl].Name).c_str();
    m_factory->AddTree( (TTree*)InFile.Get(curName), curName);
  }

  // Book the methods and perform the actual training
  m_factory->PrepareTrainingAndTestTree( "", "SplitMode=Random:NormMode=NumEvents:!V" );
  m_factory->BookMethod(TMVA::Types::kBDT, "BDTG_Method", m_BdtOptions.c_str());

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
void PndMultiClassBdtTrain::Initialize()
{
  InitBdt();
  AddVariables();
}

/*
 *Initialize Classifier and data structures.
 */
void PndMultiClassBdtTrain::InitBdt()
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
    m_transform = "Transformations=I;D;P;G,D";
    std::cerr << "<WARNING> No transformation was specified. We will set this to:\n\t"
	      << m_transform << '\n';
  }
  if (m_BdtOptions == "")
  {
    m_BdtOptions = "!H:!V:NTrees=1000:BoostType=Grad:Shrinkage=0.10:UseBaggedGrad:GradBaggingFraction=0.50:nCuts=20:NNodesMax=8";
    std::cerr << "<WARNING> No train options were specified. We will set this to:\n\t"
	      << m_BdtOptions <<'\n';
  }
  if(m_evalFileName == "")
  {
    m_evalFileName = m_JName + "_EvaluationFile.root";
    std::cerr << "<WARNING> No evaluation file name was specified. We will set this to:\n\t"
	      << m_evalFileName <<'\n';
  }
  // Set the Weight files directory name.
  (TMVA::gConfig().GetIONames()).fWeightFileDir = m_weightDirName.c_str();

  // Set the factory options and create the object
  std::string factOpt = "!V:!Silent:Color:DrawProgressBar:" + m_transform + ":AnalysisType=multiclass";

  // Init factory and Evaluation file
  EvalFile  = new TFile(m_evalFileName.c_str(), "RECREATE");
  m_factory = new TMVA::Factory(m_JName.c_str(), EvalFile, factOpt.c_str());
}

void PndMultiClassBdtTrain::AddVariables()
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
void PndMultiClassBdtTrain::storeWeights()
{
  /**
   * Does not need implementation. TMVA does this.
   */
}
