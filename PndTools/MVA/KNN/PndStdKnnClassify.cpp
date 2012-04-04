/* ***************************************
 * KNN based classifier.                 *
 * Author: M.Babai@rug.nl                *
 * Edited: E.A.Dijck@student.rug.nl      *
 * Version: 0.1 beta1                    *
 * license:                              *
 * ***************************************
 */
/*
 * Based on the straight KNN algorithm
 * Using a linear search data structure.
 */

#include "PndStdKnnClassify.h"

using namespace std;

/*
 * @param inputFile: The name of the file that holds the weights
 * @param classNames: The names of classes to which an event might be
 * assigned to.
 * @param varNames: Variable names from which the feature vector is
 * built.
 */
PndStdKnnClassify::PndStdKnnClassify(string const& inputFile,
				     vector<string> const& classNames, 
				     vector<string> const& varNames)
  : PndMvaClassifier(inputFile, classNames, varNames),
    m_Knn(0)
{}

void PndStdKnnClassify::Initialize()
{
  // Call parents function
  PndMvaClassifier::Initialize();

  // Get events.
  vector<pair<string, vector<float>*> > const& events = m_dataSets.GetData();
  
  // Init distances container
  for(size_t evt = 0; evt < events.size(); evt++)
  {
    m_distances.push_back(PndMvaDistObj());
  }
}

//! Destructor
PndStdKnnClassify::~PndStdKnnClassify()
{
  m_distances.clear();
}

/**
 * Given a feature vector describing the pattern. Classifies the pattern.
 *@param EvtData Input vector describing the pattern.
 *@return The name of the class to which the current pattern is assigned.
 */
std::string* PndStdKnnClassify::Classify(std::vector<float> EvtData)
{
  // Zero number of neighbors.
  if( m_Knn == 0 )
  {
    std::cerr << "\t<ERROR> Number neighbours cannot be zero."
	      << std::endl;
    assert (m_Knn != 0);
  }
  
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
  return (new std::string(CurWin) );
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

  // Get labels.
  vector<PndMvaClass> const& classes = m_dataSets.GetClasses();
  
  // Get examples.
  vector<pair<string, vector<float>*> > const& events = m_dataSets.GetData();

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
  NormalizeEvent(eventData);

  // Now we need to compute distances to all available proto types
  // (examples) and store the results.

  for(size_t evt = 0; evt < events.size(); evt++)
  {
    vector<float> const* ProtoVals = (events[evt]).second;
    
    float dist = ComputeDist( eventData, (*ProtoVals) );

    m_distances[evt].m_idx  = -1;
    m_distances[evt].m_dist = dist;
    m_distances[evt].m_cls  = (events[evt]).first;
    
  }//For evt

  // All distances are determined. We need to sort the distances.
  sort(m_distances.begin(), m_distances.end());

  // Find the number of examples per class within first m_Knn
  // examples.
  for(size_t id = 0; id < m_Knn; ++id)
  {
    string clas = (m_distances[id]).m_cls; //The Object class
    // Determine (Increment) the number of objects per class
    result[clas] += 1.0;
  }
  
  // Normalizing the results (K_i/K)
  for(size_t cls = 0; cls < classes.size(); ++cls)
  {
    result[classes[cls].Name] /= m_Knn;
  }

  // float Psum = 0.0;
  // for(size_t cls = 0; cls < classes.size(); cls++)
  // {
  //   int num = classes[cls].NExamples;
  //   result[classes[cls].Name] /= (static_cast<float>(num));
  //   Psum += result[classes[cls].Name];
  // }
  
  // for(size_t cls = 0; cls < classes.size(); cls++)
  // {
  //   result[classes[cls].Name] /= Psum;
  // }
}
