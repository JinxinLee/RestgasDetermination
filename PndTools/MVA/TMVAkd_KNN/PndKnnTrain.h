#pragma once
#ifndef PndKnnTrain_H
#define PndKnnTrain_H

// Standard C++ libraries
#include <iostream>
#include <sstream>
#include <vector>

// Root includes
#include "TFile.h"
#include "TTree.h"

class PndKnnTrain{
 public:
  /**
   * Constructor.
   * @param OutPut: File name to hold the weight values.
   * @param ClassNames: Class names for wich the classifier is
   * trained.
   * @param VarNames: Variable names creating the feature vector.
   */
  PndKnnTrain(const char *OutPut, 
	      const std::vector<std::string>& ClassNames, 
	      const std::vector<std::string>& VarNames);
  /**
   * Class destructor.
   */
  virtual ~PndKnnTrain();
  
  /**
   * Train the classifier.
   * @param EvtData: Current event example.
   * @param cls: The class of the current example.
   */
  void Train(const std::vector<float> &EvtData, const std::string cls);
  
  //protected:
  
 private:
  TFile *m_OutPutF;/**< Write the weights in this file. */
  int   m_numClass;/**< Number of classes. */
  int   m_numVars;/**< Dimension of the variables. */
  
  std::vector<TTree *> m_SigTrees;
  std::vector< std::vector<float>* > m_varContainer;
  std::vector<std::string> m_ClassNames;
};
#endif//end of interface definition
