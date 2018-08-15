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

#define DEBUG_DATASET 0

/**
 * Constructor.
 *@param InputEvtsParam  Input event data (attributes).
 *@param classNames      Names of available Labels (classes).
 *@param varNames        Available variable names.
 *@param type            Application Type.
 *
 * Note: The data from "InputEvtsParam" is copied into the internal
 * container. Use with caution in case of large data sets.
 */
PndMvaDataSet::PndMvaDataSet(std::vector< std::pair<std::string, std::vector<float>*> > const& Input,
			     std::vector<std::string> const& classNames,
			     std::vector<std::string> const& varNames,
			     AppType type
			     )
  : m_input("NO_INPUT_FILE"),
    m_UsePCA(false),
    m_NormType(NONORM),
    m_AppType(type),
    m_trim(true),
    m_RND_seed(0)
{
  // Init labels.
  InitClasses(classNames);
  
  // Init variables.
  InitVariables(varNames);
  
  // Copyt input data
  /*
   * This is (absolutly) not elegant.
   * BUT, maybe I will solve this later
   * Warning: MAYBE Needs change in future.
   */
  size_t evtCnt, tmpIdx;
  tmpIdx = 0;

  // Label loop
  for(size_t lb = 0; lb < m_classes.size(); ++lb)
  {
    evtCnt = 0;
    std::string& curLabel = m_classes[lb].Name;
    // Event data loop.
    for(size_t ev = 0; ev < Input.size(); ++ev)
    {
      // If current event belongs to the current class.
      if( Input[ev].first == curLabel)
      {
	std::vector<float>* evt = new std::vector<float>( (*(Input[ev]).second ) );
	m_events.push_back(std::make_pair(curLabel, evt) );
	evtCnt++;
      }
    }
    // Update indices.
    m_classes[lb].NExamples = evtCnt;
    m_classes[lb].StartIdx  = tmpIdx;
    tmpIdx += (evtCnt - 1);
    m_classes[lb].EndIdx    = tmpIdx;
    tmpIdx++;
  }
  
#if DEBUG_DATASET > 0
  std::cout << "========================================================\n"
	    << "<DEBUG> Info on classes constructed from data vector.\n"
	    << " Total number of evnts in Input vector = " << Input.size()
	    << " total number of added events = " << m_events.size()
	    << '\n';
  for(size_t lb = 0; lb < m_classes.size(); ++lb)
  {
    std::cout << " labels ["     << lb << "]\n"
	      << "\tName     = " << m_classes[lb].Name
	      << " NExamples = " << m_classes[lb].NExamples
	      << " StartIdx  = " << m_classes[lb].StartIdx
	      << " EndIdx    = " << m_classes[lb].EndIdx
	      << " NTrainEx  = " << m_classes[lb].NTrainEx
	      <<'\n';
  }
  std::cout << "========================================================\n";
#endif
}

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
    m_AppType(type),
    m_trim(true),
    m_RND_seed(0)
{
  // Init labels.
  InitClasses(classNames);
  
  // Init variables.
  InitVariables(varNames);
}

//! Destructor
PndMvaDataSet::~PndMvaDataSet()
{
  // Clean up event data container
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
    std::cout << "\n\n<INFO> Controle is completely passed to TMVA.\n"
	      << "\tFor available parameters, options and how to use\n"
	      << "\tthe available methods, read the TMVA manuals.\n\n";
    break;
  case CLASSIFY:
    // Validate the weight File
    try
    {
      // See if the file containes the requested properties.
      ValidateWeightFile();
      // Read input file
      ReadInput();
    }    
    catch (PndMvaDataSetException &ex)
    {
      std::cerr << ex.what()
		<< std::endl;
      exit(EXIT_FAILURE);
    }
    break;
  case TRAIN:
    // Read input file
    ReadInput();
    // NO BREAK; Do not add break

    // Pre read data into a vector.
  case PRE_INIT_EVTS:
    // Trim if required.
    if(m_trim)
    {
      Trim();
    }
    
    // Normalize input events.
    NormalizeDataSet();
    
    // PCA transformation
    if(m_UsePCA)
    {
      PCATransForm();
    }
    break;
  case UNKAPP:
  default:
    std::cerr << "<ERROR> Unknown application type.\n"
	      << "I do not know what to do.\n"
	      << std::endl;
    exit(EXIT_FAILURE);
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

  //size_t   m_RND_seed = seconds;
  if(m_RND_seed == 0)
  {
    time_t seconds;
    seconds = time (NULL);
    m_RND_seed = seconds;
  }
  TRandom3 rnd(m_RND_seed);

  // Number of examples of the class with the smallest number of members.
  size_t minEvtCnt   = std::numeric_limits<size_t>::max();

  // Find the class with minimum number of examples.
  for(size_t i = 0; i < m_classes.size(); i++)
  {
    if(m_classes[i].NExamples < minEvtCnt)
    {
      minEvtCnt = m_classes[i].NExamples;
    }
  }

  // Indices of examples to deleted.
  std::set < size_t > delIdxs;
  size_t totCnt = 0;
  
  // Select random indices per class
  for(size_t j = 0; j < m_classes.size(); j++)
  {
    std::cout << "\t-I- Selecting events for " << m_classes[j].Name
	      << '\n';
    
    size_t diff = m_classes[j].NExamples  - minEvtCnt;

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
  set<size_t>::const_iterator nextDelIdx = delIdxs.begin();// set is sorted

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
  std::cout << "<INFO> Finished Trimming.\n";
}

/**
 * Normalize event dataset using one of available methods.
 */
void PndMvaDataSet::NormalizeDataSet()
{
  if( m_events.size() == 0 )
  {
    std::cerr << "<ERROR> Un-Initialized data set.\n"
	      << "        Number of training examples equals zero.\n"
	      << "        Solve this and try again."
	      << std::endl;
    exit(1);
  }

  switch(m_NormType)
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

  case VARNORM:
    std::cout << "<INFO> Normalizing the dataset "
	      << "using Variable Normalize Transform.\n";
    VarNormalize();
    break;

  case NONORM:
  default:
    std::cout << "<INFO> No normalization scheme was selected.\n";
    break;
  }
  std::cout << "===================================================\n" ;

  // Event Loop
  for(size_t ev = 0; ev < m_events.size(); ev++)
  {
    // Parameters Loop
    for(size_t i = 0; i < m_vars.size(); i++)
    {
      // Avoid zero division (should never happen)
      //assert(m_vars[i].NormFactor != 0);
      assert( (m_vars[i].NormFactor > 0) || (m_vars[i].NormFactor < 0) );

      // [A = (A - mean)/var] (Var = NormFactor)
      (m_events[ev].second)->at(i) = (m_events[ev].second)->at(i) - (m_vars[i].Mean);
      (m_events[ev].second)->at(i) = (m_events[ev].second)->at(i) / (m_vars[i].NormFactor);
    }
  }
  // Find variable extrema.
  FindMinMax();
}

/**
 * Initialize the class conditional means vectors.
 */
void PndMvaDataSet::InitClsCondMeans(std::set <size_t> const& excludeIndxs)
{
  // Labels loop.
  for(size_t lb = 0; lb < m_classes.size(); ++lb)
  {
    CompClsCondMean(m_classes[lb].Name, excludeIndxs);
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
  std::cerr << "<ERROR> NOT IMPLEMENTED YET.\n"
	    << "IMPLEMENT ME."
	    << std::endl;
}

/**
 * Class conditional mean for a given class. Stored in class
 * conditional means container.
 */
void PndMvaDataSet::CompClsCondMean(std::string const& clsName,
				    std::set <size_t> const& exCluds
				    )
{
  cout << "<INFO> Determining class conditional mean vector for "
       << clsName
       << ".\n";

  // No labels makes no sense.
  assert( !m_classes.empty() );

  // Find the index of the given label.
  size_t labelIdx = 0;

  while( (labelIdx <= m_classes.size() ) && 
	 (clsName != m_classes[labelIdx].Name)
	 )
  {
    labelIdx++;
  }

  // Should never happen
  assert( labelIdx < m_classes.size() );
  
  // Temporary vector.
  vector <float>* vec = new vector <float> (m_vars.size(), 0.000);

  // Number of train events for this class.
  size_t NumTrEvts = 0;

  // Events loop
  for(size_t i = 0; i < m_events.size(); i++)
  {
    if( ( clsName == m_events[i].first ) && // Same label
	( exCluds.find(i) == exCluds.end() )// Not in test events.
	)
    {
      // Add the values.
      for(size_t j = 0; j < m_vars.size(); j++ )
      {
        vec->at(j) += (m_events[i].second)->at(j);
      }
      // Count as train event.
      NumTrEvts++;
    }
  }// All available events are visited

  /*
   * Dividing by the total number of available examples
   */
  for(size_t k = 0; k < vec->size(); k++)
  {
    vec->at(k) = vec->at(k) / static_cast <float> (NumTrEvts);
  }

  // Update labels data member.
  m_classes[labelIdx].NTrainEx = NumTrEvts;
  
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
      median = ( varVect[(varVect.size() / 2) - 1] + 
		 varVect[(varVect.size() / 2)]) / static_cast<float>(2);
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
  std::cout << '\n';

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
    std::sort( vec.begin(), vec.end() );

    // Store values for each variable.
    m_vars[i].Min  = vec[0];// Minimum
    m_vars[i].Max  = vec[vec.size() - 1 ];//Maximum
    // Distance between the Min and the Max
    m_vars[i].NormFactor = m_vars[i].Max - m_vars[i].Min;
    // The value at the middle of the list.
    m_vars[i].Mean = vec[ static_cast<size_t>(vec.size()/2)];

    std::cout << m_vars[i].Name 
	      << ": min = "    << m_vars[i].Min
	      << ", max = "    << m_vars[i].Max
	      << ", diff  = "  << m_vars[i].NormFactor
	      << ", middle = " << m_vars[i].Mean
	      << '\n';
  }
  std::cout << '\n';
}

/*
 * For each variable determine the minum and the maximum value.
*/
void PndMvaDataSet::FindMinMax()
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
    std::sort( vec.begin(), vec.end() );
        
    // Store values for each variable.
    m_vars[i].Min  = vec[0];
    m_vars[i].Max  = vec[vec.size() - 1 ];

    cout << "\n\t"
	 << m_vars[i].Name
	 << ": min = "   << m_vars[i].Min
	 << ", max = "   << m_vars[i].Max
	 << '\n';
  }
  std::cout << '\n';
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

/**
 * VariableNormalizeTransform. Linear interpolation.
 */
void PndMvaDataSet::VarNormalize()
{
  //float min, max, offset, scale;
  //min = max = offset = scale = 0.00;
  // Maybe we need to implement this.
  std::cerr << "\n\n\t<ERROR> Not implemented method.\n\n"
            << std::endl;
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
void PndMvaDataSet::ValidateWeightFile() /* throw (PndMvaDataSetException) */
{
  std::cout << "<INFO> Scanning the File: "
	    << m_input
	    << '\n';
  
  // Open the input file for reading event data.
  TFile inF(m_input.c_str(), "READ");
  
  // Get the list of available labels.
  TObjArray* Labels = (TObjArray*) inF.Get("Labels");
  if(!Labels)
  {
    throw (PndMvaDataSetException("<ERROR> Unknown file content. Labels are missing."));
  }
  // Get the list of variables.
  TObjArray* Variables = (TObjArray*) inF.Get("Variable");
  if(!Variables)
  {
    throw (PndMvaDataSetException("<ERROR> Unknown file content. Vars are missing."));
  }

  // Get modifiers.
  TObjArray* modifs = (TObjArray*) inF.Get("Modifiers");
  if(!modifs)
  {
    throw (PndMvaDataSetException("<ERROR> Unknown file content. Missing information."));
  }

  // Number of classes and variables
  size_t numLabels = static_cast<size_t>(Labels->GetEntriesFast());
  size_t numVars   = static_cast<size_t>(Variables->GetEntriesFast());
  size_t numModif  = static_cast<size_t>(modifs->GetEntriesFast());

  //If the class and variable Names agree.
  std::cout << "-I- The file containes data for the following labels:\n\t";
  for(size_t i = 0; i < numLabels; ++i)
  {
    TObjString* cur = (TObjString*) Labels->At(i);
    std::string CurLabel((cur->GetString()).Data());
    std::cout << " " << CurLabel;
    /*
      std::vector<PndMvaClass>::iterator FindResult;
      FindResult = std::find_if(m_classes.begin(), m_classes.end(), CurLabel);
      if(FindResult == m_classes.end())
      {
      throw (PndMvaDataSetException("<ERROR> The file does not contain information for " + 
      CurLabel + "."));
      }
    */
  }
  
  std::cout << "\n-I- Available variables are:\n\t";
  for(size_t i = 0; i < numVars; ++i)
  {
    TObjString* cur = (TObjString*) Variables->At(i);
    std::cout << " " << (cur->GetString()).Data();
  }
  std::cout << '\n';
  
  // Equal # of labels (classes)
  if( numLabels != m_classes.size() )
  {
    throw (PndMvaDataSetException("<ERROR> The number of labels mismatch."));
  }
  // Equal # of variables  
  if( numVars != m_vars.size() )
  {
    throw (PndMvaDataSetException("<ERROR> The number of variables mismatch."));
  }
  // If the modifiers are correctly included and available in the
  // weight file.
  for(size_t i = 0; i < numModif; ++i)
  {
    TObjString* cur = (TObjString*) modifs->At(i);
    TObject* obj = (TObject*) inF.Get( (cur->GetString()) );    
    
    if(!obj)
    {
      std::string errorTxt( (cur->GetString()).Data() );
      errorTxt = "<ERROR> Failed to fetch " + errorTxt + " from input file.";
      throw(PndMvaDataSetException(errorTxt));
    }
  }
  // Close open file.
  inF.Close();
}
