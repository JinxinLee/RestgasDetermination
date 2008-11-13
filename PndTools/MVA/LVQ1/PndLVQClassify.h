/* ***************************************
 * LVQ  Classifier                       *
 * Author: M.Babai@rug.nl                *
 * LICENSE:                              *
 * Version 1.0 beta1.                    *
 * ***************************************
 */
#ifndef PndLVQClassify_H
#define PndLVQClassify_H

// Standard C++ libraries
#include <iostream>
#include <vector>
#include <map>

// Root includes
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"

class PndLVQClassify{
 public:
  /* Constructor
   * @param InPut, Input file name (Weights).
   * @param ClassNames, class names.
   * @param VarNames, variable names of the features.
   */
  PndLVQClassify(const char* InPut,
		 const std::vector<std::string>& ClassNames, 
		 const std::vector<std::string>& VarNames);
  /*
   * Destructor
   */
  virtual ~PndLVQClassify();
  
  /*
   *@param EvtData. Event data to be classified.
   *
   *@param result.  Classification results. Currently the shortest
   * distance for each class is stored in result.
   */
  void Classify(std::vector<float> &EvtData, std::map<std::string,float>& result);

 protected:
  /* 
   * Computes the Euclidean distance between two given vectors of
   * event features.
   */
  float ComputeDist(std::vector<float> &EvtData, std::vector<float> &proto);

 private:
  // Holds the class Names
  std::vector<std::string> m_ClassNames;
  // Holds the Variable Names
  std::vector<std::string> m_VarNames;
  // Holds the LVQ proto-types
  std::vector< std::pair<std::string, std::vector<float>*> > m_protoContainer;
};
#endif//end of interface definition
