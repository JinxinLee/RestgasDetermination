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
  : PndGpidClassifier(inputFile, classNames, varNames), m_varCombinations(varCombinations)
{
  cout << "\t<INFO> Projected_KNN: Initialization." 
	    << endl;
}

//! Destructor.
PndProjectedKNN::~PndProjectedKNN()
{
  destroy();
}

//!Classify
const std::string& PndProjectedKNN::Classify(std::vector<float> EvtData)const
{
  EvtData.clear();
  std::string* re = new std::string("Not implemented yet.");
  std::cout << "Not implemented yet." << std::endl;
  return *re;
}

//!Classify
void PndProjectedKNN::GetMvaValues(vector<float> eventData,
				   map<string, float>& result)
{
  if(m_knn == 0)
  {
    cerr << "\t<ERROR> Number neighbours can not be zero."
	      << endl;
    return;
  }

  const vector<PndMvaVariable>& vars = m_dataSets.GetVars();
  const vector<PndMvaClass>& classes = m_dataSets.GetClasses();
  
  // Initialize results
  result.clear();
  for(size_t cls = 0; cls < classes.size(); cls++)
  {
    result.insert(make_pair(classes[cls].Name, 0.0));
  }
  
  // Normalize current Event
  for(size_t k = 0; k < vars.size(); k++)
  {
    assert(vars[k].NormFactor != 0);
    eventData[k] -= vars[k].Mean;
    eventData[k] /= vars[k].NormFactor;
  }
  
  // Each classifier produces number of classes results
  vector<map<string, float>* > tempResult;
  
  for(size_t i = 0; i < m_classifiers.size(); i++)
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
  
  //Normalize output
  int numClassifiers = tempResult.size();
  map<string, float>::iterator it;
  
  for(int i = 0; i < numClassifiers ; i++)
  {
    // Copy by value (Not really optimal but simple)
    map<string, float> crs = *tempResult[i];
    
    for(it = crs.begin(); it != crs.end(); it++)
    {
      result[(*it).first] += (crs[(*it).first] / static_cast<float>(numClassifiers));
    }
  }
  
  //We are done clean tempResult
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
  const vector<PndMvaVariable>& vars = m_dataSets.GetVars();
  const vector<PndMvaClass>& classes = m_dataSets.GetClasses();
  const string& inputFile = m_dataSets.GetInFileName();

  vector<string> varNames, classNames;

  for(size_t cls = 0; cls < classes.size(); cls++) {
    classNames.push_back(classes[cls].Name);
  }
  for(size_t v = 0; v < vars.size(); v++) {
    varNames.push_back(vars[v].Name);
  }

  for(size_t i = 0; i < m_varCombinations.size(); i++)
  {
    vector<string> pars = m_varCombinations[i];
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
    
    cls->SetEvtParam(m_ScaleFact, m_weight);
    cls->SetKnn(m_knn);
    
    // FIXME <WARNING> Maybe this must be changed. 
    // The input file is read n-times.
    cls->InitKNN();
    m_classifiers.push_back(make_pair(cls, indxs)); 
  }
}

//========================= Private functions and vars.
///Free allocated memory.
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
