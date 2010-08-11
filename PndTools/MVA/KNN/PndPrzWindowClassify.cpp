/* ***************************************
 * ParzenWindow based classifier.        *
 * Author: M.Babai@rug.nl                *
 * Version: 0.1 beta1                    *
 * license:                              *
 * ***************************************
 */
/*
 * Based on the straight ParzenWindow algorithm Using a linear search
 * data structure.
 */

#include "PndPrzWindowClassify.h"

using namespace std;
/*
 * @param inputFile: The name of the file that holds the weights
 * @param classNames: The names of classes to which an event might be
 * assigned to.
 * @param varNames: Variable names from which the feature vector is
 * built.
 */
PndPrzWindowClassify::PndPrzWindowClassify(const string& inputFile,
					   const vector<string>& classNames, 
					   const vector<string>& varNames)
  : PndGpidClassifier(inputFile, classNames, varNames)
{}

//! Destructor
PndPrzWindowClassify::~PndPrzWindowClassify()
{}

/**
 * Given a feature vector describing the pattern. Classifies the pattern.
 *@param EvtData Input vector describing the pattern.
 *@return The name of the class to which the current pattern is assigned.
 */
const std::string& PndPrzWindowClassify::Classify(std::vector<float> EvtData)
{
  EvtData.clear();
  std::string* out = new std::string("NotAvailable");
  return (*out);
}

/**
 * Classification function.
 * @param EvtData: Feature vector of the current event which we want
 * to classify.
 * @param result: Holds the normalized results of classification
 * for every class of events.
 */
void PndPrzWindowClassify::GetMvaValues(vector<float> eventData,
					map<string, float>& result)
{
  eventData.clear();
  result.clear();
}

/**
 * A very simple implementation of a kernel function. It can be
 * considerd as a multi-dimensional histogram with constant binsize
 * in each dimension.
 *@param evtDat Vector containing event data.
 *@return 1 if the current test sample inside the window, else 0;
 */
int PndPrzWindowClassify::histKernel( const std::vector<float>& evtDat,
				      const std::vector<float>& trSample)
{
  // Fetch the variables.
  const std::vector<PndMvaVariable>& variables = m_dataSets.GetVars();
  
  // Temporary Container
  std::vector <float> tmpPar (variables.size());
  
  for(size_t i = 0; i < variables.size(); i++){
    tmpPar[i] = abs(evtDat[i] - trSample[i])/(m_Wsize[variables[i].Name]);
  }
  
  //sort container
  std::sort(tmpPar.begin(), tmpPar.end());
  
  //last element is the largest
  if(tmpPar[tmpPar.size() - 1] <= 0.5){
    return 1;// inside
  }
  else{
    return 0;// outside
  }
}

/**
 * Set the window size. Equal size in every dimension.
 *@param wsize Window (Bin) size for all dimensions.
 */
void PndPrzWindowClassify::setWindowSize(float wsize)
{
  // Fetch the variables.
  const std::vector<PndMvaVariable>& variables = m_dataSets.GetVars();

  // init window sizes.
  for(size_t i = 0; i < variables.size(); i++){
    m_Wsize[variables[i].Name] = wsize;
  }
}
