/* ***************************************
 * KNN based classifier.                 *
 * Author: M.Babai@rug.nl                *
 * Edited: E.A.Dijck@student.rug.nl      *
 * Version: 0.1 beta1                    *
 * license:                              *
 * ***************************************
 * Based on the straight KNN algorithm
 * Using a linear search data structure.
 */

#include "PndStdKnnClassify.h"

using namespace std;

/*
 * @param inputFile: The name of the file that holds the weits
 * @param classNames: The names of classes to which an event might be
 * assigned to.
 * @param varNames: Variable names from which the feature vector is
 * built.
 */
PndStdKnnClassify::PndStdKnnClassify(const string& inputFile,
				     const vector<string>& classNames, 
				     const vector<string>& varNames)
  : PndGpidClassifier(inputFile, classNames, varNames), 
    m_Knn(0)
{
  const vector<pair<string, vector<float>*> >& events = m_dataSets.GetData();
  for(size_t evt = 0; evt < events.size(); evt++)
  {
    m_distances.push_back(PndMvaDistObj());
  }
}

//! Destructor
PndStdKnnClassify::~PndStdKnnClassify()
{}

/**
 * Given a feature vector describing the pattern. Classifies the pattern.
 *@param EvtData Input vector describing the pattern.
 *@return The name of the class to which the current pattern is assigned.
 */
const std::string& PndStdKnnClassify::Classify(std::vector<float> EvtData)const
{
  EvtData.clear();
  std::string* re = new std::string("Not implemented yet");
  std::cerr << "Not implemented yet" << std::endl;
  return *re;
}

/**
 * Classification function.
 * @param EvtData: Feature vector of the current event which we want
 * to classify.
 * @param result: Holds the normalized results of classification
 * for every class of events.
 */
void PndStdKnnClassify::GetMvaValues(vector<float> eventData,
				     map<string, float>& result)
{
  if(m_Knn == 0)
  {
    cerr << "\t<ERROR> Number neighbours cannot be zero."
	 << endl;
    assert (m_Knn != 0);
  }
  // Get variables.
  const vector<PndMvaVariable>& vars = m_dataSets.GetVars();
  // Get labels.
  const vector<PndMvaClass>& classes = m_dataSets.GetClasses();
  // Get examples.
  const vector<pair<string, vector<float>*> >& events = m_dataSets.GetData();

  if(m_Knn > events.size())
  {
    cerr << "<ERROR> Requested number of Neighbours is too large."
	 << endl;
    assert(m_Knn <= events.size());
  }

  // Initialize results
  result.clear();
  
  for(size_t cls = 0; cls < classes.size(); cls++)
  {
    result.insert( make_pair( classes[cls].Name, 0.0 ) ); 
  }
  
  // Normalize current Event
  for(size_t k = 0; k < vars.size(); k++)
  {
    assert(vars[k].NormFactor != 0);
    eventData[k] -= vars[k].Mean;
    eventData[k] /= vars[k].NormFactor;
  }

  // Now we need to compute distances to all available proto types and
  // store the results.

  for(size_t evt = 0; evt < events.size(); evt++)
  {
    vector<float>* ProtoVals = (events[evt]).second;
    
    float dist = ComputeDist(eventData, *ProtoVals);

    m_distances[evt].m_idx  = -1;
    m_distances[evt].m_dist = dist;
    m_distances[evt].m_cls  = (events[evt]).first;
    
  }//For evt

  //All distances are determined
  sort(m_distances.begin(), m_distances.end());
    
  for(size_t id = 0; id < m_Knn; id++)
  {
    string clas = (m_distances[id]).m_cls; //Find the Object class
    // Increment the number of objects per class
    result[clas] += 1.0;
  }
  
  //Normalizing the results
  float Psum = 0.0;
  for(size_t cls = 0; cls < classes.size(); cls++)
  {
    int num = classes[cls].NExamples;
    result[classes[cls].Name] /= (static_cast<float>(num));
    Psum += result[classes[cls].Name];
  }
  
  for(size_t cls = 0; cls < classes.size(); cls++)
  {
    result[classes[cls].Name] /= Psum;
  }
}
