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

/**
 * Constructor:
 * @param inputFile: Input file name (Weights).
 * @param classNames: class names.
 * @param varNames: variable names of the features.
 */
PndLVQClassify::PndLVQClassify(std::string const& inputFile,
			       std::vector<std::string> const& classNames, 
			       std::vector<std::string> const& varNames)
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
  std::vector<PndMvaClass> const& classes = m_dataSets.GetClasses();

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
void PndLVQClassify::GetMvaValues(std::vector <float> eventData,
				  std::map <std::string, float>& result)
{
  // Fetch labels (classes)
  std::vector<PndMvaClass> const& classes = m_dataSets.GetClasses();
  
  // Fetch prototypes.
  std::vector< std::pair< std::string, std::vector<float>* > > const& ProtoList = m_dataSets.GetData();
  
  // Initialize results
  result.clear();
  
  for(size_t id = 0; id < classes.size(); ++id)
  {
    result.insert(make_pair(classes[id].Name, std::numeric_limits<float>::max()));
  }

  // Normalize current Event
  NormalizeEvent(eventData);

  // Loop trough the prototypes list and compute the distances
  float dist = 0.0;

  /*
   * Compute the distance to al of the available protypes.
   */
  for(size_t i = 0; i < ProtoList.size(); i++)
  {
    // Current class Name.
    std::string clsName    = ProtoList[i].first;

    // Current prototype.
    std::vector<float>* ev = ProtoList[i].second;
    
    dist = ComputeDist(*ev, eventData);
    
    if( dist < result[clsName] )
    {
      result[clsName] = dist;
    }
  }
  // Normalize the output. SUM( distances )= 1.0
  // The smaller the better.  
  float sumOfDists = 0.00;

  for(size_t cls = 0; cls < classes.size(); ++cls)
  {
    std::string Name = classes[cls].Name;
    sumOfDists += result[Name];
  }

  // Divide by the sum .
  for(size_t cls = 0; cls < classes.size(); ++cls)
  {
    std::string Name = classes[cls].Name;
    result[Name] /= sumOfDists;
  }
}
