/* ***************************************
 * MVA Data Set implementation           *
 *                                       *
 * Author: M.Babai@rug.nl                *
 * Edited: E.A.Dijck@student.rug.nl      *
 *                                       *
 * LICENSE:                              *
 * Version: 0.1 beta1.                   *
 * ***************************************
 */
#include "PndMvaDataSet.h"

using namespace std;

// ============================ Public =====================================
/**
 * Constructor.
 *@param inputFilename  Input File name.
 *@param classNames    Names of available classes.
 *@param varNames     Available variabl names.
 */
PndMvaDataSet::PndMvaDataSet(const string& inputFilename,
			     const vector<string>& classNames,
			     const vector<string>& varNames)
  : m_input(inputFilename)
{
  // Initialize classes
  for(size_t i = 0; i < classNames.size(); i++)
  {
    m_classes.push_back(PndMvaClass(classNames[i]));
  }

  // Init varaiables
  for(size_t i = 0; i < varNames.size(); i++)
  {
    m_vars.push_back(PndMvaVariable(varNames[i]));
  }
  // Read input file
  ReadInput();
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
}

/**
 * Creates a data set with equal number of events for each class.
 */
void PndMvaDataSet::Trim()
{
  std::cout << "<INFO> Triming data set." << std::endl;
  
  // If the dtaset is initialized.
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
  cout << "<INFO> Restoring class indices." << endl;
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
}

/**
 * Normalize event dataset using one of available methods.
 *@param t Normalization type (VARX, MINMAX, MEDIAN).
 */
void PndMvaDataSet::NormalizeDataSet(const NormType type)
{
  switch(type)
  {
  case MINMAX:
    cout << "<INFO> Normalizing dataset using Min, Max spread and mid."
	 << endl;
    MinMaxDiff();
    break;
    
  case MEDIAN:
    cout << "<INFO> Normalizing the dataset "
	 << "using Median and Inter Quartile Distance (IQR)."
	 << endl;
    DetermineMedian();
    break;
    
  case VARX:
    cout << "<INFO> Normalizing the dataset "
	 << "using samle Variance and mean."
	 << endl;
    ComputeVariance();
    break;
    
  default:
    // case NONE:
    cout << "\n<INFO> No normalization scheme was selected.\n"
	 << endl;
    break;
  }
  std::cout << "===================================================" 
	    << std::endl;

  // Event Loop
  for(size_t ev = 0; ev < m_events.size(); ev++)
  {
    // Parameters Loop
    for(size_t i = 0; i < m_vars.size(); i++)
    {
      // Avoid zero division
      assert(m_vars[i].NormFactor != 0);
      // A = (A - mean)/var
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
void PndMvaDataSet::WriteDataSet(const string& outFile)
{
  cerr << "<INFO> Writing samples to file: "
       << outFile << endl;
  /* Open out put file and write coordinates of the prototypes */
  TFile out (outFile.c_str(),"RECREATE","DataSetOutput", 9);
  
  for(size_t cls = 0; cls < m_classes.size(); cls++)
  {
    vector<float> buffer(m_vars.size(), 0.0);
    
    string name = m_classes[cls].Name;
    string desc = "Description Of " + name;
    const char* treeName = name.c_str();
    const char* treeDesc = desc.c_str();
    
    // Create a tree
    TTree sig (treeName, treeDesc);
    
    // Create branches and bind the variables
    for(size_t j = 0; j < m_vars.size(); j++)
    {
      string vname = m_vars[j].Name;
      string leaf  = vname + "/F" ;
      const char* bname = vname.c_str();
      const char* lname = leaf.c_str();
      
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
    const char* bname = vname.c_str();
    const char* lname = leaf.c_str();
    
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
    const char* bname = vname.c_str();
    const char* lname = leaf.c_str();
    
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

// ============================ Protected ===================================
/**
 * Read input event data.
 */
void PndMvaDataSet::ReadInput()
{
  cout << "<INFO> Reading data from  "<< m_input
	    << endl;

  // Open the input file for reading event data.
  TFile InPutFile(m_input.c_str(),"READ");

  int minIdx, maxIdx;
  minIdx = 0; maxIdx = -1;

  // Fetch the class trees and read the event data.
  for(size_t cls = 0; cls < m_classes.size(); cls++)
  {
    // Tree name
    const char *name = m_classes[cls].Name.c_str();
    std::cout << "<INFO> Reading events for "
	      <<  m_classes[cls].Name << std::endl;
    
    // Get the tree object
    TTree *t = (TTree*) InPutFile.Get(name);
    if(!t)
    {
      cerr<< "\t<ERROR> Could not find data tree " << name 
	       << endl;
      assert (t);
    }
    // Init a container to bind to the tree branches
    vector<float> ev (m_vars.size(),0.0);
    
    // Bind the parameters to the tree branches
    for(size_t j = 0; j < m_vars.size(); j++)
    {
      const char* branchName = m_vars[j].Name.c_str();
      //Binding the branches
      t->SetBranchAddress(branchName, &(ev[j]));
    }// Tree parameters are bounded
    
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

  // Read Norm facts
  TTree* fact = (TTree*) InPutFile.Get("NormFact");
  
  if(fact)
  {
    cout << "<INFO> Reading normalization values."<< endl;
    vector<float> normVars(m_vars.size(), 0.0);
    
    // Bind the parameters to the tree branches
    for(size_t j = 0; j < m_vars.size(); j++)
    {
      const char* branchName = m_vars[j].Name.c_str();
      
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

  // Read Means
  TTree* m = (TTree*) InPutFile.Get("Means");
  
  if(m)
  {
    cout << "<INFO> Reading mean values."<< endl;
    
    vector<float> meanVals(m_vars.size(), 0.0);
    
    // Bind the parameters to the tree branches
    for(size_t j = 0; j < m_vars.size(); j++)
    {
      const char* branchName = m_vars[j].Name.c_str();
      
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

  // Close the open file and delete the file pointer
  InPutFile.Close();
}

// ============================ Private =====================================
/**
 * Class conditional mean for a given class. Stored in class
 * conditional means container.
 */
void PndMvaDataSet::CompClsCondMean(const string& clsName)
{
  cout << "<INFO> Determining class conditional mean for "
       << clsName << "." << endl;

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
    variance = variance / static_cast<float>(m_events.size() - 1 );
    variance = sqrt(variance);
    
    // Insert
    m_vars[i].NormFactor = variance;
    m_vars[i].Mean = mean;
    cout << m_vars[i].Name << "\t mean = " << mean
	      << "\t\tVar(X) = " << variance << endl;
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
  cout << "\t<INFO> Determining median for each parameter." 
            << endl;
  
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
	      << "\t IntQuartDist = " << Fquartil << endl;
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
    sort(vec.begin(),vec.end());
    
    cout << m_vars[i].Name << "\tmin = " << vec[0] 
	      << "\t\t max = " << vec[vec.size() - 1 ];
    
    float diff   = vec[ vec.size() - 1 ] - vec[0];
    float middle = vec[ static_cast<unsigned int>(vec.size()/2)];

    // Store values
    m_vars[i].NormFactor = diff;
    m_vars[i].Mean = middle;
    cout << "\t\t diff  = " << diff  << endl;
    cout << "\t\t midle = " << middle << endl;
  }
}
