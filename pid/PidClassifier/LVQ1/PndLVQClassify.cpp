/* ***************************************
 * LVQ  Classifier                       *
 * Author: M.Babai@rug.nl                *
 * Edited: E.A.Dijck@student.rug.nl      *
 * LICENSE:                              *
 * Version: 0.1 beta1.                   *
 * License:                              *
 * ***************************************
 */

#include "PndLVQClassify.h"

using namespace std;

/**
 * Constructor:
 * @param inputFile: Input file name (Weights).
 * @param classNames: class names.
 * @param varNames: variable names of the features.
 */
PndLVQClassify::PndLVQClassify(const string& inputFile,
			       const vector<string>& classNames, 
			       const vector<string>& varNames)
  : PndGpidClassifier(inputFile, classNames, varNames)
{}

/**
 * Destructor
 */
PndLVQClassify::~PndLVQClassify()
{}

/**
 * @param eventData: Event data to be classified.
 * @param result:  Classification results. Currently the shortest
 *                 distance for each class is stored in result.
 */
void PndLVQClassify::Classify(vector<float> eventData,
			      map<string,float>& result)
{
  // Holds the number of available examples per class
  const vector<PndMvaVariable>& vars = m_dataSets.GetVars();
  const vector<PndMvaClass>& classes = m_dataSets.GetClasses();
  const vector<pair<string, vector<float>*> >& events = m_dataSets.GetData();
  
  // Initialize results
  result.clear();
  for(size_t id = 0; id < classes.size(); id++){
    result.insert(make_pair(classes[id].Name, numeric_limits<float>::max()));
  }

  // Normalize current Event
  for(size_t k = 0; k < vars.size(); k++)
  {
    assert(vars[k].NormFactor != 0);
    eventData[k] -= vars[k].Mean;
    eventData[k] /= vars[k].NormFactor;
  }
  
  // Loop trough the prototypes list and compute the distances
  float dist = 0.0;
  for(size_t i = 0; i < events.size(); i++)
  {
    string clsName = events[i].first;
    vector<float>* ev = events[i].second;
    
    dist = ComputeDist(*ev, eventData);
    
    if(dist < result[clsName]){
      result[clsName] = dist;
    }
  }
}
