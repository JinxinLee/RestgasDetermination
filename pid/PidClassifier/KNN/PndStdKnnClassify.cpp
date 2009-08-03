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

PndStdKnnClassify::~PndStdKnnClassify()
{}

void PndStdKnnClassify::Classify(vector<float> eventData,
				 map<string, float>& result)
{
  if(m_Knn == 0)
  {
    cerr << "\t<ERROR> Number neighbours cannot be zero."
	 << endl;
    return;
  }

  const vector<PndMvaVariable>& vars = m_dataSets.GetVars();
  const vector<PndMvaClass>& classes = m_dataSets.GetClasses();
  const vector<pair<string, vector<float>*> >& events = m_dataSets.GetData();

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
  
  if(m_Knn > m_distances.size())
  {
    cerr << "<ERROR> Requested number of Neighbours is too large."
	 << endl;
    assert(m_Knn <= m_distances.size());
  }
  
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
