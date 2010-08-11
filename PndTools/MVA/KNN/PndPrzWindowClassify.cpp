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
  : PndGpidClassifier(inputFile, classNames, varNames), m_volumeN(0)
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
  for(size_t i = 0; i < classes.size(); i++){
    std::string curName = classes[i].Name;
    if( TMPres[curName] > Curprob){
      Curprob = TMPres[curName];
      CurWin  = curName;
    }
  } 
  // Create and return the result object (string).
  std::string* outPut = new std::string(CurWin);
  return *outPut;
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
  assert( m_volumeN != 0.0 );
  
  // Get examples.
  const vector <pair<string, vector<float>*> >& events = m_dataSets.GetData();
  
  // Get labels.
  const vector<PndMvaClass>& labels = m_dataSets.GetClasses();

  // Get variables.
  const vector<PndMvaVariable>& vars = m_dataSets.GetVars();

  // Normalize current Event
  for(size_t k = 0; k < vars.size(); k++){
    assert(vars[k].NormFactor != 0);
    eventData[k] -= vars[k].Mean;
    eventData[k] /= vars[k].NormFactor;
  }

  // Init output (result) map
  for(size_t cl = 0; cl < labels.size(); cl++){
    result[labels[cl].Name] = 0.0;
  }

  float numSamples = static_cast<float>(events.size());
  float phi = 0.00;
  
  // Loop through available classes.
  for(size_t cl = 0; cl < labels.size(); cl++){
    // Get current label
    std::string curLabel = labels[cl].Name;

    // Loop through examples
    for(size_t ex = 0; ex < events.size(); ex++){
      if( events[ex].first == curLabel){//Same labels
	phi = histKernel( eventData, *(events[ex].second) );
	//result[curLabel] += (1.00/m_volumeN) * phi;
	result[curLabel] += (phi/m_volumeN);
      }
    }//examples loop
  }// labels loop

  // Normalize with number of available samples.
  for(size_t cl = 0; cl < labels.size(); cl++){
    //result[labels[cl].Name] = (1.00 /numSamples) * (result[labels[cl].Name]);
    result[labels[cl].Name] = ( (result[labels[cl].Name]) / numSamples );
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

  // Set hypercube volume.
  m_volumeN = CompHyperCubeVolume();
}

/**
 * A very simple implementation of a kernel function. It can be
 * considerd as a multi-dimensional histogram with constant binsize
 * in each dimension.
 *@param evtDat Vector containing event data.
 *@return 1 if the current test sample inside the window, else 0;
 */
float PndPrzWindowClassify::histKernel( const std::vector<float>& evtDat,
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
    return 1.00;// inside
  }
  else{
    return 0.00;// outside
  }
}

/** 
 * Compute the hypercube volume
 */
float PndPrzWindowClassify::CompHyperCubeVolume()
{
  float volume = 1;
  
  //Compute the hypercube volume
  for(std::map <std::string, float>::const_iterator d = m_Wsize.begin(); d != m_Wsize.end(); ++d){
    volume = volume * d->second;
  }
  return volume;
}
