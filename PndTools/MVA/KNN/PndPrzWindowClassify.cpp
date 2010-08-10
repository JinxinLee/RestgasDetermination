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
int PndPrzWindowClassify::histKernel(const std::vector<float>& evtDat)
{
  evtDat.size();
  return 0;
}

void setWindowSize(float wsize)
{
  std::cout << wsize;
}
void setWindowSize(const std::map<std::string, float>& wsize)
{
  std::cout << wsize.size();
}
