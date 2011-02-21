/* ***************************************
 * MVA Data Set implementation           *
 *                                       *
 * Author: M.Babai@rug.nl                *
 * Edited: E.A.Dijck@student.rug.nl      *
 *                                       *
 * LICENSE:                              *
 * Version:                              *
 * ***************************************
 */
#include "PndMvaDataSet.h"

using namespace std;

/**
 * Constructor.
 *@param inputFilename  Input weights File name.
 */
/*
  PndMvaDataSet::PndMvaDataSet(std::string const& WeightFile)
  : m_input(WeightFile),
  m_UsePCA(false),
  m_NormType(NONORM),
  m_AppType(UNKAPP)
  {}
*/
/**
 * Constructor.
 *@param inputFilename  Input File name.
 *@param classNames     Names of available Labels (classes).
 *@param varNames       Available variabl names.
 *@param type           Application Type.
 */
PndMvaDataSet::PndMvaDataSet(std::string const& WeightFile,
			     std::vector<std::string> const& classNames,
			     std::vector<std::string> const& varNames,
			     AppType type)
  : m_input(WeightFile),
    m_UsePCA(false),
    m_NormType(NONORM),
    m_AppType(type)
{
  // Init labels.
  InitClasses(classNames);

  // Init variables.
  InitVariables(varNames);

  // FIXME DELETE ME.
  //InitDataSet();
  /*
  switch (type)
  {
  case TMVATRAIN:
  case TMVACLS:
    std::cout << "Yet To be done\n"
	      << "NIet alles tegelijkertijd, :P\n";
    break;
  case CLASSIFY:
    ValidateWeightFile();
    //FIXME FIXME FIXME
    ReadWeightsFromFile();
    // Read input file
    ReadInput();
    break;
  case TRAIN: // Read input file
    ReadInput();
    break;
  case UNKAPP:
  default:
    std::cerr << "<ERROR> Unknown application type.\n"
	      << "We do not know what to do\n"
	      << std::endl;
    return;
    break;
    }*/
}

//! Destructor
PndMvaDataSet::~PndMvaDataSet()
{
  //Clean up event data container
  for(size_t i = 0; i < m_events.size(); i++)
  {
    delete m_events[i].second;
  }
  m_events.clear();

  // Clean up the container for class Conditional means
  map< string, vector<float>* >::iterator it;
  for(it = m_ClassCondMeans.begin(); it != m_ClassCondMeans.end(); ++it)
  {
    delete it->second;
  }
  m_ClassCondMeans.clear();

  m_classes.clear();
  m_vars.clear();
}

/**
 * Init Dataset. Determine how to handle input, based on the
 * application type.
 */
void PndMvaDataSet::Initialize()
{
  switch (m_AppType)
  {
  case TMVATRAIN:// Train TMVA method
  case TMVACLS: // Use trained TMVA method.
    std::cout << "Yet To be done\n"
	      << "NIet alles tegelijk,:P\n";
    break;
  case CLASSIFY:
    // Validate the weight File
    if(ValidateWeightFile())
    {
      // Read weight File.
      // ReadWeightsFromFile();
      
      // Read input file
      ReadInput();
    }
    else
    {
      // FIXME Better to do exception :?
      exit(1);
    }
    break;
  case TRAIN:
    // Read input file
    ReadInput();
    break;
  case UNKAPP:
  default:
    std::cerr << "<ERROR> Unknown application type.\n"
	      << "We do not know what to do\n"
	      << std::endl;
    exit(1);
    break;
  }
  std::cout <<"<INFO> Initialization done.\n" ;
}

/**
 * Creates a data set with equal number of events for each class.
 */
void PndMvaDataSet::Trim()
{
  std::cout << "<INFO> Triming data set.\n";
  
  // If the data set is initialized.
  assert(m_events.size() > 1 );

  time_t seconds;
  seconds = time (NULL);
  unsigned int  m_RND_seed = seconds;
  TRandom3 rnd(m_RND_seed);

  int minCntIndex = 0;
  unsigned int minEvtCnt   = std::numeric_limits<unsigned>::max();

  // Find the class with minimum number of examples.
  for(size_t i = 0; i < m_classes.size(); i++)
  {
    if(m_classes[i].NExamples < minEvtCnt)
    {
      minEvtCnt = m_classes[i].NExamples;
      minCntIndex = i;
    }
  }

  // Indices of examples to deleted.
  std::set<size_t> delIdxs;
  unsigned int totCnt = 0;
  
  // Select random indices per class
  for(size_t j = 0; j < m_classes.size(); j++)
  {
    std::cout << "\t-I- Selecting events for " << m_classes[j].Name
	      <<'\n';
    
    unsigned int diff = m_classes[j].NExamples  - minEvtCnt;

    if(!(diff == 0))
    {
      totCnt += diff;
      int stIx = m_classes[j].StartIdx;
      int enIx = m_classes[j].EndIdx;

      while(delIdxs.size() < totCnt)
      {
	int index = static_cast<int>(rnd.Uniform(stIx, enIx));
	delIdxs.insert(index);
      }
    }
  }

  // Copy all events we want to keep.
  vector< pair<string, vector<float>*> > newEvents(m_classes.size() * minEvtCnt);
  set<size_t>::const_iterator nextDelIdx = delIdxs.begin();  // set is sorted

  size_t newIdx = 0;
  for(size_t oldIdx = 0; oldIdx < m_events.size(); oldIdx++)
  {
    if(nextDelIdx != delIdxs.end() && *nextDelIdx == oldIdx)
    {
      // Delete data of current event.
      delete m_events[oldIdx].second;
      ++nextDelIdx;
    }
    else
    {
      // Copy current event.
      newEvents[newIdx] = m_events[oldIdx];
      ++newIdx;
    }
  }

  // Swap event vector with new one.
  m_events.swap(newEvents);

  // Restore indices
  cout << "\t-I- Restoring class indices.\n";
  int minIdx = 0;
  for(size_t idx = 0; idx < m_events.size(); idx++)
  {
    if(idx == m_events.size() - 1 || m_events[idx].first != m_events[idx + 1].first)
      {
	// Find class
	for(size_t k = 0; k < m_classes.size(); k++)
	  {
	    // Update class info
	    if(m_classes[k].Name == m_events[idx].first)
	      {
		m_classes[k].NExamples = minEvtCnt;
		m_classes[k].StartIdx = minIdx;
		m_classes[k].EndIdx = idx;
	      }
	  }

	minIdx = idx + 1;
      }
  }
  std::cout << "<INFO> Finished Trimming." << '\n';
}

/**
 * Normalize event dataset using one of available methods.
 *@param t Normalization type (VARX, MINMAX, MEDIAN).
 */
void PndMvaDataSet::NormalizeDataSet(NormType type)
{
  m_NormType = type;

  switch(type)
  {
  case MINMAX:
    cout << "<INFO> Normalizing dataset using Min,"
	 << "Max spread and mid.\n";
    MinMaxDiff();
    break;
    
  case MEDIAN:
    cout << "<INFO> Normalizing the dataset "
	 << "using Median and Inter Quartile Distance (IQR).\n";
    DetermineMedian();
    break;
    
  case VARX:
    cout << "<INFO> Normalizing the dataset "
	 << "using sample Variance and mean.\n";
    ComputeVariance();
    break;

  case NONORM:
  default:
    cout << "<INFO> No normalization scheme was selected.\n";
    break;
  }
  std::cout << "===================================================\n" ;

  // Event Loop
  for(size_t ev = 0; ev < m_events.size(); ev++)
  {
    // Parameters Loop
    for(size_t i = 0; i < m_vars.size(); i++)
    {
      // Avoid zero division
      //assert(m_vars[i].NormFactor != 0);
      assert( (m_vars[i].NormFactor > 0) || (m_vars[i].NormFactor < 0) );

      // [A = (A - mean)/var] (Var = NormFactor)
      (m_events[ev].second)->at(i) = (m_events[ev].second)->at(i) - (m_vars[i].Mean);
      (m_events[ev].second)->at(i) = (m_events[ev].second)->at(i) / (m_vars[i].NormFactor);
    }
  }
}

/**
 * Initialize the class conditional means vectors.
 */
void PndMvaDataSet::InitClsCondMeans()
{
  //void CompClsCondMean(const std::string& clsName);
  for(size_t cls = 0; cls < m_classes.size(); cls++)
  {
    CompClsCondMean(m_classes[cls].Name);
  }
}

/**
 * Write the normalized DataSet to the out-put file.
 * @param  outFile  File name to write to
 */
void PndMvaDataSet::WriteDataSet(std::string const& outFile)
{
  std::cout << "<INFO> Writing DataSet samples to file: "
	    << outFile
	    << '\n';

  /* 
   * Open out put file and write coordinates of the prototypes
   */
  TFile out (outFile.c_str(),"RECREATE","DataSetOutput", 9);
  
  for(size_t cls = 0; cls < m_classes.size(); cls++)
  {
    vector<float> buffer(m_vars.size(), 0.0);
    
    string name = m_classes[cls].Name;
    string desc = "Description Of " + name;
    char const* treeName = name.c_str();
    char const* treeDesc = desc.c_str();
    
    // Create a tree
    TTree sig (treeName, treeDesc);
    
    // Create branches and bind the variables
    for(size_t j = 0; j < m_vars.size(); j++)
    {
      string vname = m_vars[j].Name;
      string leaf  = vname + "/F" ;
      char const* bname = vname.c_str();
      char const* lname = leaf.c_str();
      
      // Bind the parameters to the tree elements.
      sig.Branch(bname, &buffer[j], lname);
    }
    
    // Fill The tree
    for(size_t i = 0; i < m_events.size(); i++)
    {
      if(m_events[i].first == name)
      {
        for(size_t k = 0; k < buffer.size(); k++)
        {
          buffer[k] = (m_events[i].second)->at(k);
        }
        sig.Fill();
      }
    }
    // Write the created tree
    sig.Write();
  }//End for cls

  //_____________ Write normalization and transformation data.
  // Write normFactors
  vector<float> buffer(m_vars.size(), 0.0);
  string name = "NormFact";
  string desc = "desc of " + name;
  
  TTree fact(name.c_str(), desc.c_str());

  // Create branches and bind the variables
  for(size_t j = 0; j < m_vars.size(); j++)
  {
    string vname = m_vars[j].Name;
    string leaf  = vname + "/F" ;
    char const* bname = vname.c_str();
    char const* lname = leaf.c_str();
    
    // Bind the parameters to the tree elements.
    fact.Branch(bname, &buffer[j], lname);
  }
  
  // Fill the trees.
  for(size_t i = 0; i < m_vars.size(); i++)
  {
    buffer[i] = m_vars[i].NormFactor;
  }
  fact.Fill();
  fact.Write();
  
  // Write mean
  name = "Means";
  desc = "desc of " + name;
  
  TTree meanTree(name.c_str(), desc.c_str());

  // Create branches and bind the variables
  for(size_t j = 0; j < m_vars.size(); j++)
  {
    string vname = m_vars[j].Name;
    string leaf  = vname + "/F" ;
    char const* bname = vname.c_str();
    char const* lname = leaf.c_str();
    
    // Bind the parameters to the tree elements.
    meanTree.Branch(bname, &buffer[j], lname);
  }
  
  // Fill the trees.
  for(size_t i = 0; i < m_vars.size(); i++)
  {
    buffer[i] = m_vars[i].Mean;
  }
  meanTree.Fill();
  meanTree.Write();

  //Close open file
  out.Close();
}

/**
 * Read input event data.
 */
void PndMvaDataSet::ReadInput()
{
  cout << "<INFO> Reading data from  "
       << m_input
       << '\n';

  // Open the input file for reading event data.
  TFile InPutFile(m_input.c_str(),"READ");

  int minIdx, maxIdx;
  minIdx = 0; maxIdx = -1;

  // Fetch the class trees and read the event data.
  for(size_t cls = 0; cls < m_classes.size(); cls++)
  {
    // Tree name
    char const* name = m_classes[cls].Name.c_str();
    std::cout << "<INFO> Reading events for "
	      <<  m_classes[cls].Name << '\n';
    
    // Get the tree object
    TTree *t = (TTree*) InPutFile.Get(name);
    if(!t)
    {
      std::cerr<< "\t<ERROR> Could not find data tree "
	       << name
	       << std::endl;
      assert (t);
    }

    //disable all branches
    t->SetBranchStatus("*",0);

    // Init a container to bind to the tree branches
    vector<float> ev (m_vars.size(), 0.0);
    
    // Bind the parameters to the tree branches
    for(size_t j = 0; j < m_vars.size(); j++)
    {
      char const* branchName = m_vars[j].Name.c_str();
      // Activate branches
      t->SetBranchStatus(branchName, 1);
      
      //Binding the branches
      t->SetBranchAddress(branchName, &(ev[j]));
    }
    // Tree parameters are bounded
    
    // Fetch the number of examples available for the current class
    m_classes[cls].NExamples = t->GetEntriesFast();

    // Find-out the min and max indices  per class
    maxIdx = maxIdx + t->GetEntriesFast();
    m_classes[cls].StartIdx = minIdx;
    m_classes[cls].EndIdx = maxIdx;
    minIdx = maxIdx + 1;
    
    // Fetch and store the variables to per class variable container
    for(unsigned int k = 0; k < t->GetEntriesFast(); k++)
    {
      t->GetEntry(k);
      
      // Container to store the vent data read from the input tree
      vector<float>* eventData = new vector<float>();
      
      for(size_t idx = 0; idx < m_vars.size(); idx++)
      {
        eventData->push_back(ev[idx]);
      }
      
      // Store the event and its class name
      m_events.push_back(make_pair(m_classes[cls].Name, eventData));
    }
    
    // We are done and can delete the tree pointer
    delete t;
  }// End of for(cls) loop for all classes

  //__________________ Read Norm facts
  TTree* fact = (TTree*) InPutFile.Get("NormFact");
  
  if(fact)
  {
    cout << "<INFO> Reading normalization values.\n";
    vector<float> normVars(m_vars.size(), 0.0);
    
    // Bind the parameters to the tree branches
    for(size_t j = 0; j < m_vars.size(); j++)
    {
      char const* branchName = m_vars[j].Name.c_str();
      
      //Binding the branches
      fact->SetBranchAddress(branchName, &(normVars[j]));
    }// Tree parameters are bounded
    
    // Fetch and store the variables to variable container
    for(unsigned int k = 0; k < fact->GetEntriesFast(); k++)
    {
      fact->GetEntry(k);
      
      for(size_t idx = 0; idx < m_vars.size(); idx++)
      {
        m_vars[idx].NormFactor = normVars[idx];
      }
    }//End of tree loop
    delete fact;
  }

  //__________________________ Read Means
  TTree* m = (TTree*) InPutFile.Get("Means");
  
  if(m)
  {
    cout << "<INFO> Reading mean values.\n";
    
    vector<float> meanVals(m_vars.size(), 0.0);
    
    // Bind the parameters to the tree branches
    for(size_t j = 0; j < m_vars.size(); j++)
    {
      char const* branchName = m_vars[j].Name.c_str();
      
      //Binding the branches
      m->SetBranchAddress(branchName, &(meanVals[j]));
    }// Parameters are bounded
    
    // Fetch and store the variables to variable container
    for(unsigned int k = 0; k < m->GetEntriesFast(); k++)
    {
      m->GetEntry(k);
      
      for(size_t idx = 0; idx < m_vars.size(); idx++)
      {
        m_vars[idx].Mean = meanVals[idx];
      }
    }//End of tree loop
    delete m;
  }
  
  //________________________ Get PCA data.
  //If PCA has been applied.  
  if( InPutFile.Get("PCAMeans") && InPutFile.Get("PCAEigenVectors") )
  {
    std::cout << "<INFO> Found PCA transformed values.\n";

    // PCA Means.
    TVectorT<double>* pca_mean = (TVectorT<double>*) InPutFile.Get("PCAMeans");
    m_PCA.SetMeanVector( *(pca_mean) );

    std::cout << "-I- Mean vector:\n";
    pca_mean->Print();

    // PCA Eigen vectors.
    TMatrixT<double>* pca_EigVect = (TMatrixT<double>*) InPutFile.Get("PCAEigenVectors");
    m_PCA.SetEigenVectors( *(pca_EigVect) );

    std::cout << "-I- Eigen Vectors:\n";
    pca_EigVect->Print();

    m_UsePCA = true;

    // Delete un-needed pointers.
    delete pca_mean;
    delete pca_EigVect;
  }

  // Close the open file.
  InPutFile.Close();
}

/**
 * Read Weights and parameters from file.
 */
void PndMvaDataSet::ReadWeightsFromFile()
{
  std::cerr <<"<ERROR> NOT IMPLEMENTED YET.\n"
	    << "IMPLEMENT ME."
	    << std::endl;
}

/**
 * Class conditional mean for a given class. Stored in class
 * conditional means container.
 */
void PndMvaDataSet::CompClsCondMean(std::string const& clsName)
{
  cout << "<INFO> Determining class conditional mean for "
       << clsName << ".\n";

  // Find the class.
  size_t classNum = 0;  
  for(size_t i = 0; i < m_classes.size(); i++)
  {
    if(clsName == m_classes[i].Name)
    {
      classNum = i;
      break;
    }
  }

  vector <float>* vec = new vector <float> (m_vars.size(),0.0);
  for(size_t i = 0; i < m_events.size(); i++)
  {
    if( clsName == m_events[i].first )
    {
      for(size_t j = 0; j < m_vars.size(); j++ )
      {
        vec->at(j) += (m_events[i].second)->at(j);
      }
    }
  }// All available events are visited

  /* 
   * Dividing by the total number of available examples
   */
  int numExam = m_classes[classNum].NExamples;
  for(size_t k = 0; k < vec->size(); k++)
  {
    vec->at(k) = (vec->at(k)/(static_cast<float>(numExam)));
  }
  // Add to the Class Conditional Means container
  m_ClassCondMeans.insert( make_pair(clsName, vec) );
}

/**
 * Computes Variance (unbiased estimator) for each parameter in the
 * feature list.
 * @param clsName The name of the class of events for with we want
 * to compute Var(X).
 */
void PndMvaDataSet::ComputeVariance()
{
  vector<float> vec(m_events.size(), 0.0);
  float mean = 0.0;
  float variance = 0.0;

  //Variable Loop
  for(size_t i = 0; i < m_vars.size(); i++ )
  {
    // Event Loop
    for(size_t ev = 0; ev < m_events.size(); ev++)
    {
      vec[ev] = (m_events[ev].second)->at(i);
      mean   += (m_events[ev].second)->at(i);
    }
    // Compute variable mean and variance
    mean /= static_cast<float>(m_events.size());
    
    for(size_t ev = 0; ev < m_events.size(); ev++)
    {
      variance += (vec[ev] - mean) * (vec[ev] - mean);
    }
    // dividing by (N-1), unbiased sigma or (sigma)^2.
    variance = variance / static_cast<float>(m_events.size() - 1 );
    variance = sqrt(variance);
    
    // Insert
    m_vars[i].NormFactor = variance;
    m_vars[i].Mean = mean;
    cout << m_vars[i].Name << "\t mean = " << mean
	      << "\t\tVar(X) = " << variance << '\n';
    // Reset
    mean = 0.0;
    variance = 0.0;
  }
}

/**
 * Determines the median for parameters of the loaded DataSet.
 */
void PndMvaDataSet::DetermineMedian()
{
  cout << "\t<INFO> Determining median for each parameter.\n";
  
  float median = 0.00; float Fquartil = 0.00;
  vector <float> varVect(m_events.size(), 0.00);    

  // We need to determine the median and inter-quartile distances per
  // feature.
  
  // Loop through the Variables
  for(size_t i = 0; i < m_vars.size(); i++)
  {
    // Loop through the Examples
    for(size_t ev = 0; ev < m_events.size(); ev++)
    {
      varVect[ev] = (m_events[ev].second)->at(i);
    }
    // Sort the variables
    sort(varVect.begin(), varVect.end());
    
    if(varVect.size() % 2 == 0)
    {
      median = ( varVect[(varVect.size() / 2) - 1] + varVect[(varVect.size() / 2)]) / static_cast<float>(2);
      Fquartil = varVect[( (varVect.size() + 2)/4) - 1 ];
    }
    else
    {
      median = varVect[(varVect.size() / 2)];
      Fquartil = varVect[( (varVect.size() + 1)/4) - 1 ];
    }

    // Store values
    m_vars[i].NormFactor = Fquartil;
    m_vars[i].Mean = median;

    cout << m_vars[i].Name << "\tmedian = " << median 
	 << "\t IntQuartDist = " << Fquartil 
	 << '\n';
  }
}

/**
 * Determine Min Max difference.
 */
void PndMvaDataSet::MinMaxDiff()
{
  vector<float> vec(m_events.size(), 0.0);

  // Variables Loop
  for(size_t i = 0; i < m_vars.size(); i++)
  {
    // Event loop
    for(size_t j = 0; j < m_events.size(); j++)
    {
      vec[j] = (m_events[j].second)->at(i);
    }
    // Sort variables
    sort( vec.begin(), vec.end() );
    
    cout << m_vars[i].Name << "\tmin = " << vec[0] 
	      << "\t\t max = " << vec[vec.size() - 1 ];
    
    float diff   = vec[ vec.size() - 1 ] - vec[0];
    float middle = vec[ static_cast<unsigned int>(vec.size()/2)];

    // Store values
    m_vars[i].NormFactor = diff;
    m_vars[i].Mean = middle;
    cout << "\t\t diff  = " << diff  << '\n'
	 << "\t\t midle = " << middle << '\n';
  }
}

/**
 * Parameter decorrelation.
 *
 * Performs PCA (Principal component analysis) on the input dataset.
 */
void PndMvaDataSet::PCATransForm()
{
  // Init PCA transformation object.
  m_PCA.InitPCATranformation(m_events);

  m_UsePCA = true;

  std::cout << "<INFO> (PCA) Transforming the events from the current data set.\n";

  // Events loop
  for(size_t evt = 0; evt < m_events.size(); evt++)
  {  
    // Current event vector
    std::vector<float>* curEvt = (m_events[evt]).second;
    
    // Transform current event
    std::vector<float>* trsEvt = m_PCA.Transform(*curEvt);
    
    // Copy values to the original vector.
    for(size_t i = 0; i < curEvt->size(); i++)
    {
      curEvt->at(i) = trsEvt->at(i);
    }
    
    // Delete object
    delete trsEvt;
  }
}

// Init Classe.
void PndMvaDataSet::InitClasses(std::vector<std::string> const& classNames)
{
  // Initialize classes
  for(size_t i = 0; i < classNames.size(); i++)
  {
    m_classes.push_back(PndMvaClass(classNames[i]));
  }
}

//Init Variables.
void PndMvaDataSet::InitVariables(std::vector<std::string> const& varNames)
{
  // Init varaiables
  for(size_t i = 0; i < varNames.size(); i++)
  {
    m_vars.push_back(PndMvaVariable(varNames[i]));
  }
}

// Validate the input file.
bool PndMvaDataSet::ValidateWeightFile()
{
  std::cout << "<INFO> Scanning the File: "
	    << m_input
	    << '\n';

  // Open the input file for reading event data.
  TFile inF(m_input.c_str(), "READ");
  
  // Get the list of available labels.
  TObjArray* Labels = (TObjArray*) inF.Get("Labels");
  
  // Get the list of variables.
  TObjArray* Variables = (TObjArray*) inF.Get("Variable");
  
  // Number of classes and variables
  size_t numLabels = static_cast<size_t>(Labels->GetEntriesFast());
  size_t numVars   = static_cast<size_t>(Variables->GetEntriesFast());

  //If the class and variable Names agree.
  std::cout << "-I- The file containes data for the following labels:\n\t";
  for(size_t i = 0; i < numLabels; ++i)
  {
    TObjString* cur = (TObjString*) Labels->At(i);
    std::cout << " " << (cur->GetString()).Data();
  }
  
  std::cout << "\n-I- Available variables are:\n\t";
  for(size_t i = 0; i < numVars; ++i)
  {
    TObjString* cur = (TObjString*) Variables->At(i);
    std::cout << " " << (cur->GetString()).Data();
  }
  std::cout << '\n';
  
  // Equal # of labels
  if( numLabels != m_classes.size() )
  {
    std::cerr << "<ERROR> The number of labels mismatch."
	      << std::endl;
    //assert( numLabels == m_classes.size() );
  }
  // Equal # of variables  
  if( numVars != m_vars.size() )
  {
    std::cerr << "<ERROR> The number of variables mismatch."
	      << std::endl;
    //assert( numVars != m_vars.size() );
  }
  // Close open file.
  inF.Close();

  return (
	  (numLabels == m_classes.size()) && 
	  (numVars   == m_vars.size())
	  );
  /*
  // Get list of objects from the file.
  TList* objKeys = inF.GetListOfKeys();

  // Loop the list of objects.
  for(int obk= 0; obk < objKeys->GetEntries(); ++obk)
  {
    // Get object key name.
    std::string keyName ( (objKeys->At(obk))->GetName() );
    
    // Get Object key.
    TKey* key = inF.FindKey(keyName.c_str());
    
    // FIXME FIXME Hier ben je bezig.
    std::cout << key->GetClassName() << " "
	      << key->GetName()      << " "
	      << key->GetTitle()     << "\n";
    //------======================================
    //First get the class labels.
    }*/
}
