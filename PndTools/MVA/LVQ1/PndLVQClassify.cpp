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
 * Given a feature vector describing the pattern. Classifies the pattern.
 *@param EvtData Input vector describing the pattern.
 *@return The name of the class to which the current pattern is assigned.
 */
std::string* PndLVQClassify::Classify(std::vector<float> EvtData)
{
  // Temporary Store Values.
  std::map<std::string, float> TMPres;
  GetMvaValues(EvtData, TMPres);

  // Fetch labels (classes)
  const vector<PndMvaClass>& classes = m_dataSets.GetClasses();

  // Temporary variables for the winning class name.
  std::string CurWin = "UNKNOWN_WINNER";

  float CurMvaVal = std::numeric_limits<float>::max();

  // Find minimum the distance.
  for(size_t i = 0; i < classes.size(); i++){
    std::string curName = classes[i].Name;
    
    if( TMPres[curName] < CurMvaVal){
      CurMvaVal = TMPres[curName];
      CurWin    = curName;
    }
  }
  
  // Create and return the result object (string).
  return (new std::string(CurWin));
}

/**
 * @param eventData: Event data to be classified.
 * @param result:  Classification results. Currently the shortest
 *                 distance for each class is stored in result.
 */
void PndLVQClassify::GetMvaValues(vector<float> eventData,
				  map<string,float>& result)
{
  // Fetch variables
  //const vector<PndMvaVariable>& vars = m_dataSets.GetVars();

  // Fetch labels (classes)
  const vector<PndMvaClass>& classes = m_dataSets.GetClasses();

  // Fetch prototypes.
  const vector<pair<string, vector<float>*> >& ProtoList = m_dataSets.GetData();
  
  // Initialize results
  result.clear();
  
  for(size_t id = 0; id < classes.size(); id++)
  {
    result.insert(make_pair(classes[id].Name, numeric_limits<float>::max()));
  }

  // Normalize current Event
  /*
    for(size_t k = 0; k < vars.size(); k++)
    {
    assert(vars[k].NormFactor != 0);
    eventData[k] -= vars[k].Mean;
    eventData[k] /= vars[k].NormFactor;
    }
  */
  NormalizeEvent(eventData);

  // Loop trough the prototypes list and compute the distances
  float dist = 0.0;
  for(size_t i = 0; i < ProtoList.size(); i++)
  {
    string clsName    = ProtoList[i].first;
    vector<float>* ev = ProtoList[i].second;
    
    dist = ComputeDist(*ev, eventData);
    
    if( dist < result[clsName] )
    {
      result[clsName] = dist;
    }
  }
  
  // Normalize Resul map
  float Sum = 0.0;
  for(size_t i = 0; i < classes.size(); i++)
  {
    string clsName = classes[i].Name;
    Sum += result[clsName];
  }
  
  for(size_t i = 0; i < classes.size(); i++)
  {
    string clsName = classes[i].Name;
    result[clsName] /= Sum;
    // result[clsName] = 1.0 - result[clsName]; // Do we need this?
  }
  
}
