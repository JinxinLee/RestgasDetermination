/* ***********************************************
 *  Author: M. Babai (M.Babai@rug.nl)            *
 *                                               *
 *  pid classifier                               *
 *                                               *
 * Modified:                                     *
 *   Corrected by Qader Dorosti                  *
 *    -Density estimation was wrongly normalized *
 * q.dorosti.hasankiadeh@rug.nl                  *
 * ***********************************************/
#include "PndKnnClassify.h"

using namespace std;

/**
 * Constructor.
 * @param inputFile: File that holds the weights
 * @param classNames: Class names.
 * @param varNames: Variable names from which the feature vector is
 * built.
 */
PndKnnClassify::PndKnnClassify(string const& inputFile,
			       vector<string> const& classNames, 
			       vector<string> const& varNames)
  : PndMvaClassifier(inputFile, classNames, varNames),
    m_knn(0), m_ScaleFact(0.8), m_weight(1.0),
    m_module(new TMVA::kNN::ModulekNN())
{
  // Initialize the class indices map.
  for(size_t cls = 0; cls < classNames.size(); cls++)
  {
    m_classIndices.insert(make_pair(classNames[cls], cls));
  }

  // Initialize TMVA KNN module (everything is done by this module.)
  //m_module = new TMVA::kNN::ModulekNN();

  // Set the deafult value for scale factor
  //m_ScaleFact = 0.8;
  //m_knn = 0;
  //m_weight = 1.0;
}

//! Destructor
PndKnnClassify::~PndKnnClassify()
{
  if(m_module)
  {
    m_module->Clear();
    delete m_module;
    m_module = 0;
  }

  m_classIndices.clear();
}

/**
 * Initialize the KNN classifier.
 */
void PndKnnClassify::InitKNN()
{
  std::cout << "<INFO> Initializing KNN classifier with KNN = "
	    << m_knn
	    << ".\n";

  // Zero number of neighbors.
  if(m_knn == 0)
  {
    cerr << "\t<ERROR> Number neighbours can not be zero."
              << endl;
    assert (m_knn != 0);
  }
  
  // Get variables.
  vector<PndMvaVariable> const& vars = m_dataSets.GetVars();

  // Get examples.
  vector<pair<string, vector<float>*> > const& events = m_dataSets.GetData();
  
  // Read the events and insert into the module
  for(size_t j = 0; j < events.size(); j++)
  {
    vector<float>* evtVect = events[j].second;
    int cls = m_classIndices[events[j].first];
    // Create and add the current event. 
    TMVA::kNN::Event Evt(*evtVect, m_weight, cls);
    m_module->Add(Evt);
  }

  // Fill module and optimize
  m_module->Fill( static_cast<unsigned int> (vars.size()),
  		  static_cast<unsigned int> (100.0 * m_ScaleFact),
  		  "");//"metric"

  std::cout << "<INFO> Done initializing." << '\n';
}

/**
 * Given a feature vector describing the pattern. Classifies the pattern.
 *@param EvtData Input vector describing the pattern.
 *@return The name of the class to which the current pattern is assigned.
 */
std::string* PndKnnClassify::Classify(std::vector<float> EvtData)
{
  // Zero number of neighbors.
  // if( m_knn == 0 )
  // {
  //   std::cerr << "\t<ERROR> Number neighbours cannot be zero."
  //             << std::endl;
  //   assert (m_knn != 0);
  // }
  
  // Get the Mva-value.
  std::map<std::string, float> TMPres;
  
  GetMvaValues(EvtData, TMPres);
  
  // Densities are estimated. Report the winner.
  // Get labels.
  vector<PndMvaClass> const& classes = m_dataSets.GetClasses();
  
  // Temporary variables for the winning class name and density.
  std::string CurWin;
  
  float Curprob = std::numeric_limits <float>::min();
  
  // Find the maximum Mva Val.
  for(size_t i = 0; i < classes.size(); i++)
  {
    std::string curName = classes[i].Name;

    if( TMPres[curName] > Curprob)
    {
      Curprob = TMPres[curName];
      CurWin  = curName;
    }
  }
  // Create and return the result object (string).
  return (new std::string(CurWin));
}

/**
 * Classification function.
 * @param eventData: Feature vector of the current event.
 * @param result: Holds the normalized results of classification
 */
void PndKnnClassify::GetMvaValues(std::vector<float> eventData, 
				  std::map<string, float>& result)
{
  // if(m_knn == 0)
  // {
  //   cerr << "\t<ERROR> Number neighbours can not be zero."
  //             << endl;
  //   assert (m_knn != 0);
  // }

  // Get Variables.
  vector <PndMvaVariable> const& vars = m_dataSets.GetVars();
  
  // Get labels.
  vector <PndMvaClass> const& classes = m_dataSets.GetClasses();
  
  // Get examples.
  vector<pair<string, vector<float>*> > const& events = m_dataSets.GetData();
  
  if( m_knn > events.size() )
  {
    cerr << "<ERROR> Requested number of Neighbours is too large."
	 << endl;
    assert( m_knn <= events.size());
  }

  // Accept or reject eventData.
  if( eventData.size() != vars.size() )
  {
    cerr << "\t<ERROR> Number of elements do not match."
	 << endl;
    return;
  }

  // Initialize results
  result.clear();
  
  for(size_t cls = 0; cls < classes.size(); cls++)
  {
    result.insert(make_pair(classes[cls].Name, 0.00));
  }

  // Normalize current Event
  NormalizeEvent(eventData);

  // Create event and fetch Neighbours events from the tree
  TMVA::kNN::Event evt(eventData, m_weight, 20);
  m_module->Find(evt, m_knn + 2);
  
  vector <size_t> countsPerClass(classes.size(), 0);

  // Fetch the results list
  ResList lst = m_module->GetkNNList();
  
  ResList::iterator iter;
  
  for(iter = lst.begin(); iter != lst.end(); ++iter)
  {
    // Fetch the node
    TMVA::kNN::Node<TMVA::kNN::Event> const* node = (*iter).first;
    
    // Fetch the event from the Node
    TMVA::kNN::Event const& event = node->GetEvent();
    
    // Type corresponds with the class name
    int type = event.GetType();
    // Store per class counts in the search results
    countsPerClass[type] += 1;
  }
  
  // Normalizing the results
  float probSum = 0.0;
  
  // Fill result map with per class counts
  for(size_t cls = 0; cls < classes.size(); cls++)
  {
    result[classes[cls].Name] = countsPerClass[cls];
    probSum += countsPerClass[cls];
  }
  // Now probSum = total number of events available in the current
  // volume ( = K )
  
  for(size_t cls = 0; cls < classes.size(); cls++)
  {
    string const& clName = classes[cls].Name;
    result[clName] /= probSum;
  }
}

void PndKnnClassify::Initialize()
{
  PndMvaClassifier::Initialize();
  InitKNN();
}
