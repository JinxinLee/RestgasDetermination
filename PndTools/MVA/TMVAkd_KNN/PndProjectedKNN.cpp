/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  pid classifier                    *
 *                                    *
 * Modified:                          *
 *                                    *
 * ************************************/

#include "PndProjectedKNN.h"

using namespace std;

//========================= Public functions and vars.

//! Constructor
PndProjectedKNN::PndProjectedKNN(const string& inputFile,
				 const vector<string>& classNames,
				 const vector<string>& varNames,
				 const vector<vector<string> >& varCombinations
				 )
  : PndMvaClassifier(inputFile, classNames, varNames),
    m_varCombinations(varCombinations)
{
  cout << "\t<INFO> Projected_KNN: Initialization." 
       << '\n';
}

//! Destructor.
PndProjectedKNN::~PndProjectedKNN()
{
  destroy();
  m_varCombinations.clear();
}

/**
 * !Classify
 * Given a feature vector describing the pattern. Classifies the pattern.
 *@param EvtData Input vector describing the pattern.
 *@return The name of the class to which the current pattern is assigned.
 */
std::string* PndProjectedKNN::Classify(std::vector<float> EvtData)
{
  // Zero number of neighbors.
  if( m_knn == 0 )
  {
    std::cerr << "\t<ERROR> Number neighbours cannot be zero."
              << std::endl;
    assert (m_knn != 0);
  }
  
  // Get the Mva-value.
  std::map<std::string, float> TMPres;
  
  GetMvaValues(EvtData, TMPres);
  
  // Densities are estimated. Report the winner.
  // Get labels.
  const vector<PndMvaClass>& classes = m_dataSets.GetClasses();
  
  // Temporary variables for the winning class name and density.
  std::string CurWin;
  float Curprob = std::numeric_limits <float>::min();
  
  // Find the maximum Mva Val.
  for(size_t i = 0; i < classes.size(); ++i)
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

//! Get pdf's
void PndProjectedKNN::GetMvaValues(std::vector<float> eventData,
				   std::map<string, float>& result)
{
  // Zero number of neighbors!!???
  if( m_knn == 0 )
  {
    cerr << "\t<ERROR> Number neighbours can not be zero."
	      << endl;
    exit(EXIT_FAILURE);
  }

  // Get labels.
  const vector<PndMvaClass>& classes = m_dataSets.GetClasses();
  
  // Get examples.
  const vector<pair<string, vector<float>*> >& events = m_dataSets.GetData();
  
  if( m_knn > events.size() )
  {
    cerr << "<ERROR> Requested number of Neighbours is too large."
	 << endl;
    assert( m_knn <= events.size() );
  }

  // Initialize results
  result.clear();
  for(size_t cls = 0; cls < classes.size(); cls++)
  {
    result.insert(make_pair(classes[cls].Name, 0.00));
    //result.insert(make_pair(classes[cls].Name, 1.00));
  }
  
  // Normalize current Event
  NormalizeEvent(eventData);

  // Each classifier produces number of classes results
  vector< map<string, float>* > tempResult;

  for(size_t i = 0; i < m_classifiers.size(); ++i)
  {
    map<string, float>* rs = new map<string, float>();
    vector<int>* ixs = m_classifiers[i].second;
    vector<float> evt;
    
    for(size_t pr = 0; pr < ixs->size(); pr++)
    {
      evt.push_back(eventData[ixs->at(pr)]);
    }
 
    PndKnnClassify* clsfy = m_classifiers[i].first;
    clsfy->GetMvaValues(evt, *rs);
    tempResult.push_back(rs);
  }
  
  // Normalize output
  int numClassifiers = tempResult.size();
  map<string, float>::iterator it;
  
  for(int i = 0; i < numClassifiers ; i++)
  {
    // Copy by value (Not really optimal but simple)
    map<string, float> crs = *tempResult[i];
    
    for(it = crs.begin(); it != crs.end(); ++it)
    {
      // Fair voting     
      result[(*it).first] += ( crs[(*it).first] / static_cast<float>(numClassifiers) );

      // Indep. probs.
      // result[(*it).first] *= crs[(*it).first];
    }
  }
  
  // We are done clean tempResult
  for(size_t i = 0; i < tempResult.size(); i++)
  {
    (tempResult[i])->clear();
    delete tempResult[i];
  }
  tempResult.clear();  
}

//! Init classifiers
void PndProjectedKNN::InitKNN()
{
  // Fetch variables.
  const vector<PndMvaVariable>& vars = m_dataSets.GetVars();
  
  // Fetch labels.
  const vector<PndMvaClass>& classes = m_dataSets.GetClasses();
  
  // Get input file name.
  const string& inputFile = m_dataSets.GetInFileName();

  vector<string> varNames;
  vector<string> classNames;

  for(size_t cls = 0; cls < classes.size(); cls++) {
    classNames.push_back(classes[cls].Name);
  }

  for(size_t v = 0; v < vars.size(); v++) {
    varNames.push_back(vars[v].Name);
  }

  for(size_t i = 0; i < m_varCombinations.size(); i++)
  {
    vector<string>& pars = m_varCombinations[i];

    PndKnnClassify* cls = new PndKnnClassify(inputFile, classNames, pars);
    vector<int>* indxs = new vector<int>();
    
    // Find parameter indices.
    for(size_t j = 0; j < pars.size(); j++)
    {
      for(size_t k = 0; k < vars.size(); k++)
      {
        if(pars[j] == vars[k].Name)
        {
          indxs->push_back(k);
        }
      }
    }

    // Set Classifier params and init.
    cls->SetEvtParam(m_ScaleFact, m_weight);
    cls->SetKnn(m_knn);
    
    // <WARNING FIX> Maybe this must be changed. 
    // The input file is read n-times.
    cls->Initialize();
    m_classifiers.push_back(make_pair(cls, indxs)); 
  }
}

//========================= Private functions and vars.
/// Free allocated memory.
void PndProjectedKNN::destroy()
{
  for(size_t i = 0; i < m_classifiers.size(); i++)
  {
    (m_classifiers[i].second)->clear();
    delete m_classifiers[i].first;
    delete m_classifiers[i].second;
  }
  m_classifiers.clear();
}
