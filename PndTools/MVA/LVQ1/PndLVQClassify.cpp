/* ***************************************
 * LVQ  Classifier                       *
 * Author: M.Babai@rug.nl                *
 * Edited: E.A.Dijck@student.rug.nl      *
 * LICENSE:                              *
 * Version:                              *
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
PndLVQClassify::PndLVQClassify(string const& inputFile,
			       vector<string> const& classNames, 
			       vector<string> const& varNames)
  : PndMvaClassifier(inputFile, classNames, varNames)
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
  // Temporary!. Store Values.
  std::map<std::string, float> TMPres;
  GetMvaValues(EvtData, TMPres);

  // Fetch labels (classes)
  vector<PndMvaClass> const& classes = m_dataSets.GetClasses();

  // Temporary variables for the winning class name.
  std::string CurWin = "UNKNOWN_WINNER_LABEL";

  float CurMvaVal = std::numeric_limits<float>::max();

  // Find minimum distance.
  for(size_t i = 0; i < classes.size(); i++)
  {
    std::string curName = classes[i].Name;
    
    if( TMPres[curName] < CurMvaVal)
    {
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
  // Fetch labels (classes)
  vector<PndMvaClass> const& classes = m_dataSets.GetClasses();

  // Fetch prototypes.
  vector<pair<string, vector<float>*> > const& ProtoList = m_dataSets.GetData();
  
  // Initialize results
  result.clear();
  
  for(size_t id = 0; id < classes.size(); ++id)
  {
    result.insert(make_pair(classes[id].Name, numeric_limits<float>::max()));
  }

  // Normalize current Event
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
  // Normalize (SUM distances = 1.0)
  // Is this really needed??????
  
  float sumOfDists = 0.00;
  for(size_t cls = 0; cls < classes.size(); ++cls)
  {
    std::string Name = classes[cls].Name;
    sumOfDists += result[Name];
  }
  
  for(size_t cls = 0; cls < classes.size(); ++cls)
  {
    std::string Name = classes[cls].Name;
    result[Name] /= sumOfDists;
  }
}
