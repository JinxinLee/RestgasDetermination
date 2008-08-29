#ifndef PndLVQClassify_H
#define PndLVQClassify_H

// Standard C++ libraries
#include <iostream>
#include <vector>
#include <map>

//OPENMP header
//#include <omp.h>

// Root includes
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TStopwatch.h"

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
   *@param result.  Classification results.
   */
  void Classify(std::vector<float> &EvtData, std::map<std::string,float>& result);

 protected:
  /* 
   * Computes the Euclidean distance between two given vectors of
   * event features.
   */
  float ComputeDist(std::vector<float> &EvtData, std::vector<float> &proto);

 private:
  std::vector<std::string> m_ClassNames;
  std::vector<std::string> m_VarNames;

  // Holds the LVQ proto-types
  std::vector< std::pair<std::string, std::vector<float>*> > m_protoContainer;
};
#endif
//end of interface definition
