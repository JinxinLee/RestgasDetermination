/* **********************************************
 * MVA classifiers trainers interface.          *
 * Author: M. Babai                             *
 * M.Babai@rug.nl                               *
 * Version:                                     *
 * LICENSE:                                     *
 * **********************************************
 */

#include "PndMvaTrainer.h"

using namespace std;

/**
 * Constructor.
 *@param InPut The input parameters.
 *@param ClassNames Names of available labels(classes).
 *@param VarNames Names of available variables.
 */
PndMvaTrainer::PndMvaTrainer(std::vector< std::pair<std::string, std::vector<float>*> > const& InputEvtsParam,
			     std::vector<std::string> const& ClassNames, 
			     std::vector<std::string> const& VarNames,
			     bool trim)
  : m_dataSets(InputEvtsParam, ClassNames, VarNames, PRE_INIT_EVTS),
    m_RND_seed(0),
    m_trim(trim),
    m_testSetSize(0)
{}

//! Constructor
PndMvaTrainer::PndMvaTrainer(std::string const& InPut,
			     std::vector<std::string> const& ClassNames, 
			     std::vector<std::string> const& VarNames,
			     bool trim)
  : m_dataSets(InPut, ClassNames, VarNames, TRAIN),
    m_RND_seed(0),
    m_trim(trim),
    m_testSetSize(50)
{}

//! Destructor
PndMvaTrainer::~PndMvaTrainer()
{
  m_testSet_indices.clear();
  m_StepErro.clear();
}

void PndMvaTrainer::Initialize()
{
  // Trim data set
  m_dataSets.SetTrim(m_trim);

  // Initialize data set.
  m_dataSets.Initialize();

  // Split test and train set.
  if ( m_testSetSize != 0 )
  {
    splitTetsSet();
  }

  /*
   * Initialize class conditional mean vectors for the current data
   * set. Also modifies the label objects to reflect the situation
   * after excluding the events specified by the exclusion set (see
   * InitClsCondMeans docs in "PndMvaDataSet").
   */
  m_dataSets.InitClsCondMeans(m_testSet_indices);
  
  // Init random seed for this run.
  if(m_RND_seed == 0)
  {
    srand ( time(NULL) );
    m_RND_seed = (rand() % 10000000) + 1;
  }
  std::cout << "<INFO> Seed for current run is "
	    << m_RND_seed << '\n';
}

/**
 * Creates test and train data sets.
 */
void PndMvaTrainer::splitTetsSet()
{
  TRandom3 rndIndx(m_RND_seed);
  double tempIndex = 0.0;
  size_t tsindx    = 0;

  // Get all available examples.
  std::vector<std::pair<std::string, std::vector<float>*> > const& events = m_dataSets.GetData();
  assert( events.size() != 0);
  
  // Get available labels
  std::vector<PndMvaClass> const& labels = m_dataSets.GetClasses();

  // Number of examples in the test set.
  size_t TestEvtCnt = (m_testSetSize * events.size()) / 100 ;
  
  std::cout << "<INFO> preparing train and test sets.\n"
	    << "\t-I- Test set containes " << TestEvtCnt
	    <<" examples and train set " << (events.size() - TestEvtCnt) 
	    << '\n';
  
  // Select the index of the examples that are going to be used as the
  // test set.
  while(m_testSet_indices.size() < TestEvtCnt)
  {
    tempIndex = rndIndx.Uniform( 0.0, events.size() ) * events.size();
    tsindx = static_cast<size_t>(tempIndex);
    // Make sure ( 0 <= index <= (size - 1) )
    tsindx = tsindx % events.size();
    m_testSet_indices.insert(tsindx);
  }
  //======================================
  // Print some extra information on number of examples per label.
  std::set<size_t>::iterator it;
  std::map <std::string, size_t> tsMap;

  for(size_t lb = 0; lb < labels.size(); ++lb)
  {
    tsMap[labels[lb].Name] = 0;
  }
  
  for(it = m_testSet_indices.begin(); it != m_testSet_indices.end(); ++it)
  {
    size_t id = *it;
    std::string const& name = (events[id]).first;
    tsMap[name] += 1;
  }

  for(size_t lb = 0; lb < labels.size(); ++lb)
  {
    std::string const& label = labels[lb].Name;
    std::cout << "\t-I- Test Events for[ " << label
 	      << " ] = " << tsMap[label]  << " train events = "
 	      << ( labels[lb].NExamples - tsMap[label] )
   	      << '\n';
  }
  //______________________________________
}
/**
 * Writes the train and test errors evaluations to a given file.
 *@param FileName Output file name.
 */
/*
 * Warn: Maybe we need to write per class errors as well. It is
 * ofcourse needed to do statistics and not so important for plotting.
 */
void PndMvaTrainer::WriteErroVect(std::string const& FileName) const
{
  std::ofstream Outfile;
  
  Outfile.open(FileName.c_str(), ios::out| ios::trunc);
  
  Outfile << "# Error vector output\n";
  Outfile << "#<Step number> \t<Train Error> \t<Test Error>\n";
  
  for(size_t i = 0; i < m_StepErro.size(); i++)
  {
    Outfile << " " << (m_StepErro[i]).m_step  <<"\t"
	    << " " << (m_StepErro[i]).m_trErr <<"\t"
	    << " " << (m_StepErro[i]).m_tsErr 
	    << '\n';
  }
  Outfile << std::flush;
  Outfile.close();
}

/**
 * Write the training and normalization data to outFile.
 */
void PndMvaTrainer::WriteToWeightFile(std::vector< std::pair<std::string, 
				      std::vector<float>*> > const& weights) const
{
  
  std::cout << "<INFO> Writing classifier Output to "
	    << m_outFile
	    << '\n';

  // Get labels
  std::vector <PndMvaClass> const& classes = m_dataSets.GetClasses();
  
  // Get variable names.
  std::vector <PndMvaVariable> const& vars = m_dataSets.GetVars();
  
  /* 
   * Open out-put file and write coordinates.
   */
  if(m_outFile == "")
  {
    std::cerr << "<ERROR> The output file name could not be an empty string.\n"
	      << "        Set the outPut name if you want to store"
	      << " the generated weights."
	      << std::endl;
    return;
  }

  TFile out (m_outFile.c_str(), "RECREATE", "WeightFileOutput", 9);

  for(size_t cls = 0; cls < classes.size(); cls++)
  {
    std::vector<float> buffer(vars.size(), 0.0);
    
    std::string name = classes[cls].Name;
    std::string desc = "Description Of " + name;
    char const* treeName = name.c_str();
    char const* treeDesc = desc.c_str();
    
    // Create a tree
    TTree sig (treeName, treeDesc);
    
    // Create branches and bind the variables
    for(size_t j = 0; j < vars.size(); j++)
    {
      std::string vname = vars[j].Name;
      std::string leaf  = vname + "/F" ;
      char const* bname = vname.c_str();
      char const* lname = leaf.c_str();
      
      // Bind the parameters to the tree elements.
      sig.Branch(bname, &buffer[j], lname);
    }
    
    // Fill The tree
    for(size_t i = 0; i < weights.size(); i++)
    {
      if(weights[i].first == name)
      {
        for(size_t k = 0; k < buffer.size(); k++)
        {
          buffer[k] = (weights[i].second)->at(k);
        }
        sig.Fill();
      }
    }
    // Write the created tree
    std::cout << "<INFO> Writing weights for " << name
	      << '\n';
    sig.Write();
  }//End for cls

  // _______________ Normalization and transformation data _________

  //______________________ Write normFactors
  std::vector<float> buffer(vars.size(), 0.0);
  std::string name = "NormFact";
  std::string desc = "desc of " + name;
  
  TTree fact(name.c_str(), desc.c_str());

  // Create branches and bind the variables
  for(size_t j = 0; j < vars.size(); j++)
  {
    std::string vname = vars[j].Name;
    std::string leaf  = vname + "/F" ;
    char const* bname = vname.c_str();
    char const* lname = leaf.c_str();
    
    // Bind the parameters to the tree elements.
    fact.Branch(bname, &buffer[j], lname);
  }
  
  // Fill the trees.
  for(size_t i = 0; i < vars.size(); i++)
  {
    buffer[i] = vars[i].NormFactor;
  }
  fact.Fill();
  fact.Write();

  //______________________ Write mean
  name = "Means";
  desc = "desc of " + name;
  
  TTree meanTree(name.c_str(), desc.c_str());

  // Create branches and bind the variables
  for(size_t j = 0; j < vars.size(); j++)
  {
    std::string vname = vars[j].Name;
    std::string leaf  = vname + "/F" ;
    char const* bname = vname.c_str();
    char const* lname = leaf.c_str();
    
    // Bind the parameters to the tree elements.
    meanTree.Branch(bname, &buffer[j], lname);
  }
  
  // Fill the trees.
  for(size_t i = 0; i < vars.size(); i++)
  {
    buffer[i] = vars[i].Mean;
  }
  meanTree.Fill();
  meanTree.Write();

  //_______________ Write PCA Data
  if(m_dataSets.Used_PCA())
  {
    PndMvaVarPCATransform const& pca_tmp = m_dataSets.Get_PCA();
    TVectorD const& MeanVals   = pca_tmp.GetMeanValues();
    TMatrixD const& EigenVects = pca_tmp.GetEigenVectors();
    
    // Write to output.
    MeanVals.Write("PCAMeans");
    EigenVects.Write("PCAEigenVectors");
  }

  //___________ List of classes, variables, Object names, ....
  // Classes.
  TObjArray Labels(0, 0);
  Labels.SetName("Labels");
  Labels.SetOwner(kTRUE);
  // Add labels
  for(size_t cls = 0; cls < classes.size(); cls++)
  {
    std::string la = classes[cls].Name;
    Labels.Add(new TObjString(la.c_str()));
  }
  Labels.Write("Labels", TObject::kSingleKey);

  // Variables
  TObjArray variables(0, 0);
  variables.SetName("Variables");
  variables.SetOwner(kTRUE);
  // Add variables
  for(size_t j = 0; j < vars.size(); j++)
  {
    std::string vn = vars[j].Name;
    variables.Add(new TObjString(vn.c_str()));    
  }
  variables.Write("Variable", TObject::kSingleKey);
  
  // List of other performed operations input.
  TObjArray Modifiers (0, 0);
  Modifiers.SetName("Modifiers");
  Modifiers.SetOwner(kTRUE);

  Modifiers.Add(new TObjString("Means"));
  Modifiers.Add(new TObjString("NormFact"));

  if(m_dataSets.Used_PCA())
  {
    Modifiers.Add(new TObjString("PCAMeans"));
    Modifiers.Add(new TObjString("PCAEigenVectors"));
  }
  Modifiers.Write("Modifiers", TObject::kSingleKey);
  //__________________________________
  //Close open the file
  out.Close();
}

#if (TRAIN_INC_FOAM > 0)
void PndMvaTrainer::WriteToWeightFile(std::vector<TMVA::PDEFoam*> const& foamList) const
{
  std::cout << "<INFO> Writing Foams to file "
	    << m_outFile
	    << '\n';
  /* 
   * Open out-put file and write coordinates.
   */
  if(m_outFile == "")
  {
    std::cerr << "<ERROR> The output file name could not be an empty string.\n"
	      << "        Set the outPut name if you want to store"
	      << " the generated weights."
	      << std::endl;
    return;
  }
  
  TFile rootFile (m_outFile.c_str(), "RECREATE", "foamfile", 9);

  for(size_t j = 0; j < foamList.size(); j++)
  {
    foamList[j]->Write(foamList[j]->GetFoamName().Data());

    // DEBUG
    foamList[j]->Print();
    std::cout << "  i is: " 
	      << typeid(foamList[j]).name() 
	      << std::endl;
  }
  
  std::vector<PndMvaVariable> const& vars = m_dataSets.GetVars();
  // Write normFactors
  std::vector<float> buffer(vars.size(), 0.0);
  std::string name = "NormFact";
  std::string desc = "desc of " + name;
  
  TTree fact(name.c_str(), desc.c_str());
  
  // Create branches and bind the variables
  for(size_t j = 0; j < vars.size(); j++)
  {
    std::string vname = vars[j].Name;
    std::string leaf  = vname + "/F" ;
    char const* bname = vname.c_str();
    char const* lname = leaf.c_str();
    
    // Bind the parameters to the tree elements.
    fact.Branch(bname, &buffer[j], lname);
  }
  // Fill the trees.
  for(size_t i = 0; i < vars.size(); i++)
  {
    buffer[i] = vars[i].NormFactor;
  }
  fact.Fill();
  fact.Write();
  
  // Write mean
  name = "Means";
  desc = "desc of " + name;
  
  TTree meanTree(name.c_str(), desc.c_str());
  
  // Create branches and bind the variables
  for(size_t j = 0; j < vars.size(); j++)
  {
    std::string vname = vars[j].Name;
    std::string leaf  = vname + "/F" ;
    char const* bname = vname.c_str();
    char const* lname = leaf.c_str();
    
    // Bind the parameters to the tree elements.
    meanTree.Branch(bname, &buffer[j], lname);
  }
  
  // Fill the trees.
  for(size_t i = 0; i < vars.size(); i++)
  {
    buffer[i] = vars[i].Mean;
  }
  meanTree.Fill();
  meanTree.Write();
  
  // Close open file
  rootFile.Close();
}
#endif
/*
  void PndMvaTrainer::WriteDataSetToOutFile()
  {
  if(m_outFile.size() == 0)
  {
  std::cerr << "<Error> Empty Output File Name."
  << std::endl;
  return;
  }
  m_dataSets.WriteDataSet(m_outFile);
  }
*/

//! Select input data normalization scheme.
void PndMvaTrainer::NormalizeData(NormType t)
{
  m_dataSets.SetNormType(t);
}

/**
 * Parameter decorrelation.
 *
 * Performs PCA (Principal component analysis) on the input dataset.
 */
void PndMvaTrainer::PCATransForm()
{ 
  m_dataSets.Use_PCA(true);
}

void PndMvaTrainer::SetTestSetSize(size_t prc)
{
  // Using all events for testing does not make sence.
  if(prc < 100)
  {
    m_testSetSize = prc;
  }
  else
  {
    std::cerr << "<ERROR> Too large test Set.\n"
	      << "        The size is set to 50%."
	      << std::endl;
    exit(EXIT_FAILURE);
  }
}

void PndMvaTrainer::SetTestSet(std::set <size_t> const& samples)
{
  m_testSet_indices = std::set <size_t> (samples);
}

void PndMvaTrainer::EvalClassifierError()
{
  std::cout << "\n\n\n\t<WARNING>"
	    <<"Virtual function. Do not call me.\n\n\n";
}
